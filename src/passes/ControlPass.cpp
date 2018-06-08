// Pull in the function prototypes.
#include "passes/ControlPass.h"

// Pull in various graph utilities to simplify accesses/augmentations.
#include "graphs/edges/EdgeInfo.h"

// Pull in some utilities to aid with the pass.
#include "passes/PassUtils.h"

// Pulling in various LLVM classes for prototype signatures.
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"

// Shared namespace within the project.
using namespace apollo;

// No identifier for this pass (the default approach).
char ControlPass::ID = 0;

// Register this LLVM pass with the pass manager.
static RegisterPass<ControlPass> rp("control-flow", "Control-Flow Pass");

// See header file.
StringRef ControlPass::getPassName() const {
  return StringRef("Control-Flow Pass");
}

// See header file.
void ControlPass::getAnalysisUsage(AnalysisUsage &au) const {
  au.setPreservesAll();
}

// See header file.
DependencyGraph ControlPass::getGraph() {
  return depGraph;
}

// See header file.
bool ControlPass::runOnFunction(Function &func) {
  // We will only consider the labelled functions for now.
  if (PassUtils::isKernelFunction(func)) {
    addControlEdges(func);
  }
  return false;
}

// See header file.
void ControlPass::addControlEdges(Function &func) {
  // Maintain a map between basic blocks and their corresponding nodes
  std::map<BasicBlock*,Node*> bbMap;

  // Initially pass through and add all of the basic blocks
  for (auto &bb : func) {
    auto node = PassUtils::createDynamicNode(&bb);
    depGraph.addNode(node);
    bbMap[&bb] = node;
  }

  // Then add control edges as needed for terminator-to-successors
  for (auto &bb : func) {
    // Get the node corresponding to this basic block
    auto bbSrc = bbMap.at(&bb);

    // Get the terminator instruction and add a node for it
    TerminatorInst* term = bb.getTerminator();
    auto src = PassUtils::createDynamicNode(term);
    depGraph.addNode(src);

    // Loop through the successors and add edges
    for (int i = 0; i < term->getNumSuccessors(); i++) {
      auto &dest = bbMap.at(term->getSuccessor(i));
      depGraph.addEdge(src, dest, Edge_Control);
    }

    // Add phi-based control edges at the end
    for (auto &phiNode : bb.phis()) {
      // Get the phi instruction and add a node for it
      auto phiDest = PassUtils::createDynamicNode(&phiNode);
      depGraph.addNode(phiDest);

      // Add a control edge from the basic block to the phi node and others
      depGraph.addEdge(bbSrc, phiDest, Edge_Control);
      for (int i = 0; i < phiNode.getNumIncomingValues(); i++) {
        auto phiSrc = PassUtils::createDynamicNode(phiNode.getIncomingValue(i));
        depGraph.addEdge(phiSrc, phiDest, Edge_PhiData);
      }
    }
  }
}