// Pull in C-style asserts for invariant-checking.
#include <cassert>

// Pull in standard file-handling headers.
#include <fstream>

// Pull in the function prototypes.
#include "passes/DependencePass.h"

// Pull in various graph utilities to simplify accesses/augmentations.
#include "graphs/edges/EdgeInfo.h"

// Pull in some utilities to aid with the pass.
#include "passes/PassUtils.h"

// Pulling in various LLVM analysis classes to help with the construction.
#include "llvm/Analysis/DependenceAnalysis.h"
#include "llvm/Analysis/ScalarEvolutionExpressions.h"

// Pulling in various LLVM classes for prototype signatures.
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/Support/Casting.h"

// Shared namespace within the project.
using namespace apollo;

// No identifier for this pass (the default approach).
char DependencePass::ID = 0;

// Register this LLVM pass with the pass manager.
static RegisterPass<DependencePass> rp("prog-dep", "Program-Dependence Pass");

// See header file.
StringRef DependencePass::getPassName() const {
  return "Program-Dependence Pass";
}

// See header file.
void DependencePass::getAnalysisUsage(AnalysisUsage &au) const {
  au.addRequired<DependenceAnalysisWrapperPass>();
  au.setPreservesAll();
}

// See header file.
DependencyGraph DependencePass::getGraph() {
  return depGraph;
}

// See header file.
std::map<Value*,Node*> DependencePass::getValueMap() {
  return valueMap;
}

// See header file.
bool DependencePass::runOnFunction(Function &func) {
  // We will only consider the labelled functions for now.
  if (PassUtils::isKernelFunction(func)) {
    addDataEdges(func);
    addControlEdges(func);
    addPhiEdges(func);
    addMemoryEdges(func);
    addLoopCarryEdges(func);
    exportToFiles();
  }
  return false;
}

// See header file.
void DependencePass::addDataEdges(Function &func) {
  // Loop over all of the instructions in the function via basic blocks
  for (auto &bb : func) {
    for (auto &inst : bb) {
      // Create a node for the instruction
      auto node = PassUtils::create(&inst, valueMap, depGraph);

      // Ignore phi nodes for uses -- not a true data dependence
      if (!isa<PHINode>(&inst)) {
        for (Use &use : inst.operands()) {
          // Ignore basic blocks, as we will handle them later
          Value *v = use.get();
          if (!isa<BasicBlock>(v)) {
            auto val = PassUtils::createOrFind(v, valueMap, depGraph);
            depGraph.addEdge(val, node, Edge_Data);
          }
        }
      }
    }
  }
}

// See header file.
void DependencePass::addControlEdges(Function &func) {
  // Initially pass through and add all of the basic blocks
  for (auto &bb : func) {
    PassUtils::create(&bb, valueMap, depGraph);
  }

  // Then add control edges as needed for terminator-to-successors
  for (auto &bb : func) {
    // Get the terminator instruction and add a node for it
    TerminatorInst* term = bb.getTerminator();
    auto src = PassUtils::createOrFind(term, valueMap, depGraph);

    // Loop through the successors and add edges
    for (int i = 0; i < term->getNumSuccessors(); i++) {
      auto &dest = valueMap.at(term->getSuccessor(i));
      depGraph.addEdge(src, dest, Edge_Control);
    }
  }
}

// See header file.
void DependencePass::addPhiEdges(Function &func) {
  for (auto &bb : func) {
    // Get the node corresponding to this basic block
    auto bbSrc = valueMap.at(&bb);

    for (auto &phiNode : bb.phis()) {
      // Get the phi instruction and add a node for it
      auto phiDest = valueMap.at(&phiNode);

      // Add a control edge from the basic block to the phi node
      depGraph.addEdge(bbSrc, phiDest, Edge_Control);

      // Add control edges for other incoming values into the phi node
      for (int i = 0; i < phiNode.getNumIncomingValues(); i++) {
        auto phiSrc = PassUtils::createOrFind(phiNode.getIncomingValue(i), valueMap, depGraph);
        depGraph.addEdge(phiSrc, phiDest, Edge_PhiData);
      }
    }
  }
}

// See header file.
void DependencePass::addMemoryEdges(Function &func) {
  // Get all of the dependence info from the existing pass
  DependenceInfo *info = &getAnalysis<DependenceAnalysisWrapperPass>().getDI();

  // Loop through all of the instructions and find memory dependencies
  for (auto currInst = inst_begin(func), end = inst_end(func); currInst != end; currInst++) {
    for (auto nextInst = currInst; nextInst != end; nextInst++) {
      auto *instSrc  = &*currInst;
      auto *instDest = &*nextInst;

      // Get the equivalent nodes corresponding to the instructions
      auto *src  = PassUtils::createOrFind(instSrc,  valueMap, depGraph);
      auto *dest = PassUtils::createOrFind(instDest, valueMap, depGraph);

      // If the source is neither a load nor a store, ignore it
      if (!isa<LoadInst>(instSrc) && !isa<StoreInst>(instSrc)) continue;

      // If the destination is neither a load nor a store, ignore it
      if (!isa<LoadInst>(instDest) && !isa<StoreInst>(instDest)) continue;

      // If the source and destination are both loads, ignore them
      // TODO: Think about memory-consistency models? Can ignore in accelerators?
      if (isa<LoadInst>(instSrc) && isa<LoadInst>(instDest)) continue;

      // Get the dependence info for this pair of instructions
      std::unique_ptr<Dependence> infoPtr = info->depends(instSrc, instDest, true);
      auto *dep = infoPtr.get();

      // Finally, use the dependence info to perform static memory analysis
      if (dep && !dep->isInput()) {
        if (dep->isConfused())
          assert(false);
        else if (dep->isLoopIndependent()) {
          if (dep->isConsistent())
            depGraph.addEdge(src, dest, Edge_Memory);
          else
            depGraph.addEdge(src, dest, Edge_MemoryMaybe);
        }
      }
    }
  }
}

// See header file.
void DependencePass::addLoopCarryEdges(Function &func) {
  // Requires some analysis with Polly.

  // Older version accessible from GitHub. Use this tag on `master` branch:
  //   6e263e40f650ff7badcf3951061fe12f01b30861

  // Get all of the dependence info from the existing pass
  DependenceInfo *info = &getAnalysis<DependenceAnalysisWrapperPass>().getDI();

  // Loop through all of the instructions and find memory dependencies
  for (auto currInst = inst_begin(func), end = inst_end(func); currInst != end; currInst++) {
    for (auto nextInst = currInst; nextInst != end; nextInst++) {
      auto *instSrc  = &*currInst;
      auto *instDest = &*nextInst;

      // Get the equivalent nodes corresponding to the instructions
      auto *src  = PassUtils::createOrFind(instSrc,  valueMap, depGraph);
      auto *dest = PassUtils::createOrFind(instDest, valueMap, depGraph);

      // If the source is neither a load nor a store, ignore it
      if (!isa<LoadInst>(instSrc) && !isa<StoreInst>(instSrc)) continue;

      // If the destination is neither a load nor a store, ignore it
      if (!isa<LoadInst>(instDest) && !isa<StoreInst>(instDest)) continue;

      // If the source and destination are both loads, ignore them
      // TODO: Same as above.
      if (isa<LoadInst>(instSrc) && isa<LoadInst>(instDest)) continue;

      // Get the dependence info for this pair of instructions
      std::unique_ptr<Dependence> infoPtr = info->depends(instSrc, instDest, true);
      auto *dep = infoPtr.get();

      // Finally, use the dependence info to perform basic loop-carry analysis
      if (dep && !dep->isInput()) {
        if (dep->isConfused())
          assert(false);
        else if (!dep->isLoopIndependent()) {
          // Pre-loop initialization
          long distance = -1;
          unsigned maxLevels = dep->getLevels();

          // Use scalar evolution techniques to determine node distances for the
          // weighted edge map in the dependency graph
          for (unsigned level = 1; level <= maxLevels; level++) {
            const SCEV *dist = dep->getDistance(level);
            if (const auto *sc = dyn_cast_or_null<SCEVConstant>(dist))
              distance = sc->getValue()->getSExtValue();
          }

          // Populate the weighted maps with the distance info as well
          if (dep->isConsistent())
            depGraph.addEdge(src, dest, Edge_LoopCarry, distance);
          else
            depGraph.addEdge(src, dest, Edge_LoopCarryMaybe, distance);
        }
      }
    }
  }
}

// See header file.
void DependencePass::exportToFiles() {

  // This version is from the `master` branch. It needs to be modified to use
  // the `clean` branch's structure. This will require a different approach and
  // can be done with some simple refactoring, as the interface of DependencyGraph
  // directly supports it.

  // TODO: Fix this!

//  std::ofstream cfile ("../data/exported-graph.txt");
//  if (cfile.is_open()) {
//    // Initial basic block
//    cfile << "0\n";
//    // Number of basic blocks
//    std::vector<BasicBlock*> bbids;
//    std::vector<Instruction*> instids;
//    std::map<Instruction*,int> instToBB;
//    int bbIdx = 0;
//    int instIdx = 0;
//    for (auto &bb : f) {
//      bbids.push_back(&bb);
//      for (auto &inst : bb) {
//        instids.push_back(&inst);
//        instToBB[&inst] = bbIdx;
//        instIdx++;
//      }
//      bbIdx++;
//    }
//    cfile << bbIdx << "\n";
//    // Number of nodes
//    int numNodes = nodes.size();
//    cfile << numNodes << "\n";
//    // Number of edges
//    int numEdges = 0;
//    std::map<Node*,int> nodeMap;
//    int counter = 0;
//    for (auto &node : nodes) {
//      nodeMap[node] = counter;
//      numEdges += node->c_adjs.size();
//      numEdges += node->d_adjs.size();
//      numEdges += node->m_adjs.size();
//      numEdges += node->mm_adjs.size();
//      numEdges += node->p_adjs.size();
//      numEdges += node->lc_adjs.size();
//      numEdges += node->lcm_adjs.size();
//      counter++;
//    }
//    cfile << numEdges << "\n";
//    for (int id = 0; id < numNodes; id++) {
//      int instType = -1;
//      Instruction* inst = instids[id];
//      unsigned opcode = inst->getOpcode();
//
//      if (opcode == Instruction::Add) {
//        instType = 1;
//      } else if (opcode == Instruction::Sub) {
//        instType = 2;
//      } else if (opcode == Instruction::And || opcode == Instruction::Or || opcode == Instruction::Xor) {
//        instType = 3;
//      } else if (opcode == Instruction::Mul) {
//        instType = 4;
//      } else if (opcode == Instruction::SDiv || opcode == Instruction::UDiv) {
//        instType = 5;
//      } else if (isa<LoadInst>(inst)) {
//        instType = 6;
//      } else if (isa<StoreInst>(inst)) {
//        instType = 7;
//      } else if (isa<TerminatorInst>(inst)) {
//        instType = 8;
//      } else if (isa<PHINode>(inst)) {
//        instType = 9;
//      } else {
//        instType = 0;
//      }
//
//      cfile << id << "," << instType << "," << instToBB[inst] << "," << "placeholder" << "\n";
//    }
//    for (auto &src : nodes) {
//      for (auto &dest : src->c_adjs) {
//        cfile << nodeMap[src] << "," << nodeMap[dest] << "0" << "\n";
//      }
//      for (auto &dest : src->d_adjs) {
//        cfile << nodeMap[src] << "," << nodeMap[dest] << "1" << "\n";
//      }
//      for (auto &dest : src->m_adjs) {
//        cfile << nodeMap[src] << "," << nodeMap[dest] << "2" << "\n";
//      }
//      for (auto &dest : src->mm_adjs) {
//        cfile << nodeMap[src] << "," << nodeMap[dest] << "3" << "\n";
//      }
//      for (auto &dest : src->p_adjs) {
//        cfile << nodeMap[src] << "," << nodeMap[dest] << "4" << "\n";
//      }
//      for (auto &dest : src->lc_adjs) {
//        cfile << nodeMap[src] << "," << nodeMap[dest] << "5" << "\n";
//      }
//      for (auto &dest : src->lcm_adjs) {
//        cfile << nodeMap[src] << "," << nodeMap[dest] << "6" << "\n";
//      }
//    }
//  } else {
//    cfile << "Unable to open file\n";
//  }
}