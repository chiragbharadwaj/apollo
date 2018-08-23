#ifndef DYNAMICNODE_H
#define DYNAMICNODE_H
using namespace std;
#include <map>
#include <set>
#include <queue>
#include "../graph/Graph.h"
class Graph;
class BasicBlock;
class Node;
class Simulator;
class DynamicNode;
class Context;
typedef pair<DynamicNode*, uint64_t> Operator;


class DynamicNode {
public:
  Node *n;
  Context *c;
  Simulator *sim;
  TInstr type;
  bool issued = false;
  bool completed = false;
  bool isMem;
  /* Memory */
  uint64_t addr;
  bool addr_resolved = false;
  bool speculated = false;
  int outstanding_accesses = 0;
  /* Depedency */
  int pending_parents;
  int pending_external_parents;
  vector<DynamicNode*> external_dependents;

  DynamicNode(Node *n, Context *c, Simulator *sim, uint64_t addr = 0);
  bool operator== (const DynamicNode &in);
  bool operator< (const DynamicNode &in) const;
  void print(string str, int level = 0);
  void handleMemoryReturn();
  void tryActivate(); 
  bool issueMemNode();
  bool issueCompNode();
  void finishNode();
};

class OpCompare {
public:
  bool operator() (const Operator &l, const Operator &r) const {
    if(r.second < l.second) 
      return true;
    else if (l.second == r.second)
      return (*(r.first) < *(l.first));
    else
      return false;
  }
};
class DynamicNodePointerCompare {
public:
  bool operator() (const DynamicNode* l, const DynamicNode* r) const {
     return *l < *r;
  }
};

class Context {
public:
  bool live;
  unsigned int id;
  
  Simulator* sim;
  BasicBlock *bb;

  int next_bbid;
  int prev_bbid;

  std::map<Node*, DynamicNode*> nodes;
  std::set<DynamicNode*, DynamicNodePointerCompare> issue_set;
  std::set<DynamicNode*, DynamicNodePointerCompare> speculated_set;
  std::set<DynamicNode*, DynamicNodePointerCompare> next_issue_set;
  std::set<DynamicNode*, DynamicNodePointerCompare> completed_nodes;
  std::set<DynamicNode*, DynamicNodePointerCompare> nodes_to_complete;

  priority_queue<Operator, vector<Operator>, OpCompare> pq;

  Context(int id, Simulator* sim) : live(false), id(id), sim(sim) {}
  Context* getNextContext();
  Context* getPrevContext();
  void insertQ(DynamicNode *d);
  void print(string str, int level = 0);
  void initialize(BasicBlock *bb, int next_bbid, int prev_bbid);
  void process();
  void complete();
};



#endif