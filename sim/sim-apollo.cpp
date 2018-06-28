//=======================================================================
// Copyright 2018 Princeton University.
//=======================================================================

#include "sim-apollo.hpp"
#include "DRAMSim.h"

using namespace apollo;
using namespace std;

// TODO: Handle 0-latency instructions correctly
// TODO: Handle 0-cycle context creation in a more general way
// TODO: MSHR request merge


class MemOp {
public:
  uint64_t addr;
  DNode d;
  bool started=false;
  bool completed=false;
  MemOp(uint64_t addr, DNode d) : addr(addr), d(d){}
};

class LoadStoreQ {
public:
  map<DNode, MemOp*> tracker;
  deque<MemOp> q;
  bool initialized = false;
  int size;
  void initialize(int s) { 
    size = s;
    initialized = true;
  }
  bool checkSize(int requested_space) {
    if(q.size() <= size - requested_space) {
      return true;
    }
    else {
      int ct = 0;
      for(deque<MemOp>::iterator it = q.begin(); it!= q.end(); ++it) {
        if(it->completed)
          ct++;
        else
          break;
      }
      if(ct > requested_space) {
        for(int i=0; i<requested_space; i++)
          q.pop_front();
        return true;
      }
      return false;
    }
  }
  void insert(uint64_t addr, DNode d) {
    assert(initialized);
    MemOp *temp = new MemOp(addr, d);
    q.push_back(*temp);
    tracker.insert(make_pair(d, temp));
  }
  bool exists_unresolved_memop (DNode in, TInstr op_type) {    
    for(deque<MemOp>::iterator it = q.begin(); it!= q.end(); ++it) {
      Node *n = it->d.first;
      int cid = it->d.second;
      if(it->d == in)
        return false;
      else if(cid > in.second || ((cid == in.second) && (n->id > in.first->id)))
        return false;
      if(!(it->started) && n->typeInstr == op_type)
        return true;
    }
    return false;
  }
  bool exists_conflicting_memop (DNode in, TInstr op_type) {
    uint64_t addr = tracker.at(in)->addr;
    for(deque<MemOp>::iterator it = q.begin(); it!= q.end(); ++it) {
      Node *n = it->d.first;
      int cid = it->d.second;
      if(it->d == in)
        return false;
      else if(cid > in.second || ((cid == in.second) && (n->id > in.first->id)))
        return false;
      if(it->started && !(it->completed) && n->typeInstr == op_type && it->addr == addr)
        return true;
    }
    return false;
  }
};

class Context {
public:
  bool live;
  int id;
  int bbid;
  int processed;
  std::vector<Node*> active_list;
  std::set<Node*> start_set;
  std::set<Node*> next_start_set;
  std::vector<Node *> next_active_list;
  std::vector<Node *> resource_limited_list;

  std::map<Node*, int> remaining_cycles_map;  // tracks remaining cycles for each node
  std::map<Node*, int> pending_parents_map;   // tracks the # of pending parents (intra BB)
  std::map<Node*, int> pending_external_parents_map; // tracks the # of pending parents (across BB)

  Context(int id) : live(true), id(id), bbid(-1), processed(0) {}
  
  void initialize(BasicBlock *bb, std::map<Node*, std::pair<int, bool> > &curr_owner, std::map<DNode, std::vector<DNode> > &deps, std::map<int, std::set<Node*> > &handled_phi_deps, LoadStoreQ &lsq, map<int, queue<uint64_t> >& memory) {
 
    assert(bbid == -1);
    bbid = bb->id;
    int mem_count = 0;
    for ( int i=0; i<bb->inst.size(); i++ ) {
      Node *n = bb->inst.at(i);
      if (n->typeInstr == ST || n->typeInstr == LD)
        mem_count++;
    }
    if(!lsq.checkSize(mem_count))
      assert(false); // Context creation failed due to limited LSQ size
    active_list.push_back(bb->entry);
    remaining_cycles_map.insert(std::make_pair(bb->entry, 0));
    
    // traverse all the BB's instructions and initialize the pending_parents map
    for ( int i=0; i<bb->inst.size(); i++ ) {
      Node *n = bb->inst.at(i);

      if (n->typeInstr == ST || n->typeInstr == LD) {
        // add entries to LSQ
        uint64_t addr = memory.at(n->id).front();
        lsq.insert(addr, make_pair(n, id));
        memory.at(n->id).pop();
      }
          
      if(n->typeInstr == PHI)
        pending_parents_map.insert(std::make_pair(n, n->parents.size()+1));
      else
        pending_parents_map.insert(std::make_pair(n, n->parents.size()));
      pending_external_parents_map.insert(std::make_pair(n, n->external_parents.size()));
    }

    /* Handle Phi */
    if (handled_phi_deps.find(id) != handled_phi_deps.end()) {
      std::set<Node*>::iterator it;
      for(it = handled_phi_deps.at(id).begin(); it != handled_phi_deps.at(id).end(); ++it) {
        pending_parents_map.at(*it)--;
      }
      handled_phi_deps.erase(id);
    }

    /* Handle External Edges */
    for (int i=0; i<bb->inst.size(); i++) {
      Node *n = bb->inst.at(i);
      if (n->external_dependents.size() > 0) {
        if(curr_owner.find(n) == curr_owner.end())
          curr_owner.insert(make_pair(n, make_pair(id, false)));
        else {
          curr_owner.at(n).first = id;
          curr_owner.at(n).second = false;
        }
      }
      if (n->external_parents.size() > 0) {
        std::set<Node*>::iterator it;
        for (it = n->external_parents.begin(); it != n->external_parents.end(); ++it) {
          Node *s = *it;
          int src_cid = curr_owner.at(s).first;
          bool done = curr_owner.at(s).second;
          DNode src = make_pair(s, src_cid);
          if(done)
            pending_external_parents_map.at(n)--;
          else {
            if(deps.find(src) == deps.end()) {
              deps.insert(make_pair(src, vector<DNode>()));
            }
            deps.at(src).push_back(make_pair(n, id));
          }
        }
      }
    }
    // No need to call launchNode; entryBB will trigger them instead. 
  }

  void launchNode(Node *n) {
    // check if node <n> can be launched
    if(pending_parents_map.at(n) > 0 || pending_external_parents_map.at(n) > 0)
      return;
    next_active_list.push_back(n);
    next_start_set.insert(n);
    std::cout << "Node [" << n->name << " @ context " << id << "]: Ready to Execute\n";
    remaining_cycles_map.insert(std::make_pair(n, n->lat));
  }
};


class Simulator
{ 
public:  
  class DRAMSimCallBack {
    public:
      map< uint64_t, queue< pair<Node*, Context*> > > outstanding_accesses_map;
      Simulator* sim;
      DRAMSimCallBack(Simulator* sim):sim(sim){}
    
      void read_complete(unsigned id, uint64_t addr, uint64_t mem_clock_cycle) {
        assert(outstanding_accesses_map.find(addr) != outstanding_accesses_map.end());
        queue<std::pair<Node*, Context*>> &q = outstanding_accesses_map.at(addr);
        Node *n = q.front().first;
        Context *c = q.front().second;
        sim->lsq.tracker.at(make_pair(n,c->id))->completed = true;
        cout << "Node [" << n->name << " @ context " << c->id << "]: Read Transaction Returns "<< addr << "\n";
        if (c->remaining_cycles_map.find(n) == c->remaining_cycles_map.end())
          cout << *n << " / " << c->id << "\n";
        c->remaining_cycles_map.at(n) = 0; // mark "Load" as finished
        q.pop();
        if (q.size() == 0)
          outstanding_accesses_map.erase(addr);
      }
      void write_complete(unsigned id, uint64_t addr, uint64_t clock_cycle) {
        assert(outstanding_accesses_map.find(addr) != outstanding_accesses_map.end());
        queue<std::pair<Node*, Context*>> &q = outstanding_accesses_map.at(addr);
        Node *n = q.front().first;
        Context *c = q.front().second;
        sim->lsq.tracker.at(make_pair(n,c->id))->completed=true;
        cout << "Node [" << n->name << " @ context " << c->id << "]: Write Transaction Returns "<< addr << "\n";
        q.pop();
        if(q.size() == 0)
          outstanding_accesses_map.erase(addr);
      }
  };

  Graph g;
  Config cfg;
  DRAMSimCallBack cb=DRAMSimCallBack(this); 
  DRAMSim::MultiChannelMemorySystem *mem;
  
  int cycle_count = 0;
  vector<Context*> context_list;
  /* Resources */
  map<TInstr, int> FUs;
  int ports[2]; // ports[0] = loads; ports[1] = stores;

  vector<int> cf; // List of basic blocks in "sequential" program order 
  int cf_iterator = 0;
  map<int, queue<uint64_t> > memory; // List of memory accesses per instruction in a program order
  /* Handling External Dependencies */
  map<Node*, pair<int, bool> > curr_owner; // Source of external dependency (Node), Context ID for the node (cid), Finished
  map<DNode, vector<DNode> > deps; // Source of external dependency (Node,cid), Destinations for that source (Node, cid)
  /* Handling Phi Dependencies */
  map<int, set<Node*> > handled_phi_deps; // Context ID, Processed Phi node
  LoadStoreQ lsq;

  void initialize() {
    DRAMSim::TransactionCompleteCB *read_cb = new DRAMSim::Callback<DRAMSimCallBack, void, unsigned, uint64_t, uint64_t>(&cb, &DRAMSimCallBack::read_complete);
    DRAMSim::TransactionCompleteCB *write_cb = new DRAMSim::Callback<DRAMSimCallBack, void, unsigned, uint64_t, uint64_t>(&cb, &DRAMSimCallBack::write_complete);
    mem = DRAMSim::getMemorySystemInstance("sim/config/DDR3_micron_16M_8B_x8_sg15.ini", "sim/config/dramsys.ini", "..", "Apollo", 16384); 
    mem->RegisterCallbacks(read_cb, write_cb, NULL);
    mem->setCPUClockSpeed(2000000000);  
    lsq.initialize(cfg.lsq_size);
    for(int i=0; i<NUM_INST_TYPES; i++) {
      FUs.insert(make_pair(static_cast<TInstr>(i), cfg.num_units[i]));
    }
    if (cfg.CF_one_context_at_once) {
      cf_iterator = 0;
      createContext( cf.at(cf_iterator) );  // create just the very firt context from <cf>
    }
    else if (cfg.CF_all_contexts_concurrently)  { // create ALL contexts to max parallelism
      for ( cf_iterator=0; cf_iterator < cf.size(); cf_iterator++ )
        createContext( cf.at(cf_iterator) );
    }
  }

  void createContext(int bbid)
  {
    assert(bbid < g.bbs.size());
    /* Create Context */
    int cid = context_list.size();
    Context *c = new Context(cid);
    context_list.push_back(c);
    BasicBlock *bb = g.bbs.at(bbid);
    c->initialize(bb, curr_owner, deps, handled_phi_deps, lsq, memory);
    cout << "Context [" << cid << "]: Created (BB=" << bbid << ")\n";     
  }

  void process_context(Context *c)
  {
    // traverse ALL the active instructions in the context
    for (int i=0; i < c->active_list.size(); i++) {
      Node *n = c->active_list.at(i);
      if (c->start_set.find(n) != c->start_set.end()) {
        bool canExecute = true;
        // check resource (FU) availability 
        if (FUs.at(n->typeInstr) != -1) {
          if (FUs.at(n->typeInstr) == 0) {
            cout << "Node [" << n->name << "]: cannot start due to lack of FUs \n";
            canExecute = false;
          }
        }
        // Memory Ports 
        if (n->typeInstr == LD && ports[0] == 0)
          canExecute = false;
        if (n->typeInstr == ST && ports[1] == 0)
          canExecute = false;
        // Memory dependency
        if (n->typeInstr == LD || n->typeInstr == ST) {
          DNode d = make_pair(n,c->id);
          bool stallCondition = false;
          if (n->typeInstr == ST)
            stallCondition = lsq.exists_unresolved_memop(d, ST) || lsq.exists_unresolved_memop(d, LD) || lsq.exists_conflicting_memop(d, ST) || lsq.exists_conflicting_memop(d, LD);
          else if (n->typeInstr == LD)
            stallCondition = lsq.exists_unresolved_memop(d, ST) || lsq.exists_conflicting_memop(d, ST);
          canExecute &= !stallCondition;
          uint64_t addr = lsq.tracker.at(d)->addr;
          canExecute &= mem->willAcceptTransaction(addr);
        }
        // Execution (if possible)
        if(canExecute) {
          cout << "Node [" << n->name << " @ context " << c->id << "]: Starts Execution \n";
          if (FUs.at(n->typeInstr) != -1) {
            FUs.at(n->typeInstr)--;
            cout << "Node [" << n->name << "]: acquired FU (new free FUs: " << FUs.at(n->typeInstr) << ")\n";
          }
          if (n->typeInstr == LD || n->typeInstr == ST) {
            DNode d = make_pair(n,c->id);
            uint64_t addr = lsq.tracker.at(d)->addr;
            lsq.tracker.at(d)->started = true;
            cout << "Node [" << n->name << " @ context " << c->id << "]: Inserts Memory Transaction for Address "<< addr << "\n";
            if (n->typeInstr == LD) { 
              ports[0]--;
              mem->addTransaction(false, addr);
            }
            else if (n->typeInstr == ST) {
              ports[1]--;
              mem->addTransaction(true, addr);
            }
            if (cb.outstanding_accesses_map.find(addr) == cb.outstanding_accesses_map.end())
              cb.outstanding_accesses_map.insert(make_pair(addr, queue<std::pair<Node*, Context*>>()));
            cb.outstanding_accesses_map.at(addr).push(make_pair(n, c));
          }
        }
        else {   // if cannot execute delay <node> until next cycle
          c->next_active_list.push_back(n);
          c->next_start_set.insert(n);
          continue;
        }
      }
      // decrease the remaining # of cycles for current node <n>
      int remaining_cycles = c->remaining_cycles_map.at(n);
      if (remaining_cycles > 0)
        remaining_cycles--;
      if ( remaining_cycles > 0 || remaining_cycles == -1 ) {  // Node <n> will continue execution in next cycle 
        // A remaining_cycles == -1 represents a outstanding memory request being processed currently by DRAMsim
        c->remaining_cycles_map.at(n) = remaining_cycles;
        c->next_active_list.push_back(n);  
      }
      else if (remaining_cycles == 0) { // Execution Finished for node <n>
        cout << "Node [" << n->name << " @ context " << c->id << "]: Finished Execution \n";

        // Handle Resource
        if ( FUs.at(n->typeInstr) != -1 ) { // FU limited operation
          FUs.at(n->typeInstr)++; // release FU
          cout << "Node [" << n->name << "]: released FU, new free FU: " << FUs.at(n->typeInstr) << endl;
        }

        // If node <n> is a TERMINATOR create new context with next bbid in <cf> (a bbid list)
        //     iff <simulation mode> is "CF_one_context_at_once"
        if (cfg.CF_one_context_at_once && n->typeInstr == TERMINATOR) {
          if ( cf_iterator < cf.size()-1 ) {  // if there are more pending contexts in the <cf> vector
            cf_iterator++;
            createContext( cf.at(cf_iterator) );
          }
        }
        c->remaining_cycles_map.erase(n);
        if (n->typeInstr != ENTRY)
          c->processed++;
        
        // Since node <n> ended, update dependents: decrease each dependent's parent count & try to launch each dependent
        set<Node*>::iterator it;
        for (it = n->dependents.begin(); it != n->dependents.end(); ++it) {
          Node *d = *it;
          c->pending_parents_map.at(d)--;
          c->launchNode(d);
        }

        // The same for external dependents: decrease parent's count & try to launch them
        if (n->external_dependents.size() > 0) {
          DNode src = make_pair(n, c->id);
          if (deps.find(src) != deps.end()) {
            vector<DNode> users = deps.at(src);
            for (int i=0; i<users.size(); i++) {
              Node *d = users.at(i).first;
              Context *cc = context_list.at(users.at(i).second);
              cc->pending_external_parents_map.at(d)--;
              cc->launchNode(d);
            }
            deps.erase(src);
          }
          curr_owner.at(n).second = true;
        }

        // Same for Phi dependents
        for (it = n->phi_dependents.begin(); it != n->phi_dependents.end(); ++it) {
          int next_cid = c->id+1;
          Node *d = *it;
          if ((cf.size() > next_cid) && (cf.at(next_cid) == d->bbid)) {
            if (context_list.size() > next_cid) {
              Context *cc = context_list.at(next_cid);
              cc->pending_parents_map.at(d)--;
              cc->launchNode(d);
            }
            else {
              if (handled_phi_deps.find(next_cid) == handled_phi_deps.end()) {
                handled_phi_deps.insert(make_pair(next_cid, set<Node*>()));
              }
              handled_phi_deps.at(next_cid).insert(d);
            }
          }
        }
      }
      else
        assert(false);
    } //end for

    // Continue with following active instructions
    c->start_set = c->next_start_set;
    c->active_list = c->next_active_list;
    c->next_start_set.clear();
    c->next_active_list.clear();
    // Check if the current context is done
    if ( c->processed == g.bbs.at(c->bbid)->inst_count ) {
      cout << "Context [" << c->id << "]: Finished Execution (Executed " << c->processed << " instructions) \n";
      c->live = false;
    }
  }

  void process_memory()
  {
    mem->update();
  }

  bool process_cycle()
  {
    cout << "[Cycle: " << cycle_count << "]\n";
    cycle_count++;
    bool simulate = false;
    assert(cycle_count < 2000);
    ports[0] = cfg.load_ports;
    ports[1] = cfg.store_ports;
    // process all live contexts
    for (int i=0; i<context_list.size(); i++) {
      if (context_list.at(i)->live) {
        simulate = true;
        process_context(context_list.at(i));
      }
    }

    process_memory();
    return simulate;
  }

  void run()
  {
    bool simulate = true;
    while (simulate) {
      simulate = process_cycle();
    }
    mem->printStats(false);
  }
};

int main(int argc, char const *argv[])
{
  Simulator sim;
  Reader r;
  r.readCfg("input/config.txt", sim.cfg);
  r.readGraph("input/graph2.txt", sim.g, sim.cfg);
  r.readProfMemory("input/memory.txt", sim.memory);
  r.readProfCF("input/ctrl.txt", sim.cf);
  //r.read(sim);
  sim.initialize();
  sim.run();
  return 0;
} 
