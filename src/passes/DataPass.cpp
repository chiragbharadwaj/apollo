// Pull in the function prototypes.
#include "passes/DataPass.h"

// Pull in all earlier passes that act as prerequisites for this pass.
#include "passes/ControlPass.h"

// Pull in various graph utilities to simplify accesses/augmentations.
#include "graphs/edges/EdgeInfo.h"

// Pull in some utilities to aid with the pass.
#include "passes/PassUtils.h"

// Pulling in various LLVM classes for prototype signatures.
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Use.h"
#include "llvm/IR/Value.h"

// Shared namespace within the project.
using namespace apollo;

// No identifier for this pass (the default approach).
char DataPass::ID = 0;

// Register this LLVM pass with the pass manager.
static RegisterPass<DataPass> rp("data-dependence", "Data-Dependence Pass");

// See header file.
StringRef DataPass::getPassName() const {
  return StringRef("Data-Dependence Pass");
}

// See header file.
void DataPass::getAnalysisUsage(AnalysisUsage &au) const {
  au.addRequired<ControlPass>();
  au.setPreservesAll();
}

// See header file.
DependencyGraph DataPass::getGraph() {
  return depGraph;
}

// See header file.
std::map<Value*,Node*> DataPass::getValueMap() {
  return valueMap;
}

// See header file.
bool DataPass::runOnFunction(Function &func) {
  // We will only consider the labelled functions for now.
  if (PassUtils::isKernelFunction(func)) {
    depGraph = getAnalysis<ControlPass>().getGraph();
    valueMap = getAnalysis<ControlPass>().getValueMap();
    addDataEdges(func);
  }
  return false;
}

// See header file.
void DataPass::addDataEdges(Function &func) {
  // Loop over all of the instructions in the function via basic blocks
  for (auto &bb : func) {
    for (auto &inst : bb) {
      auto node = PassUtils::createOrFind(&inst, valueMap, depGraph);

      // Ignore phi nodes in this case
      if (!isa<PHINode>(&inst)) {
        for (Use &use : inst.operands()) {
          // Ignore basic blocks, since we already handled earlier
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