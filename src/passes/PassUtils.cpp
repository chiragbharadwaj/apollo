// Pull in some standard data structures.
#include <string>

// Pull in C-style asserts for invariant-checking.
#include <cassert>

// Pull in the function prototypes.
#include "passes/PassUtils.h"

// Shared namespace within the project.
using namespace apollo;

// See header file.
bool PassUtils::isKernelFunction(Function &func) {
  return (func.getName().str().find(KERNEL_STR) != std::string::npos);
}

// See header file.
Node* PassUtils::createDynamicNode(Value *v) {
  if (auto *inst = dyn_cast<Instruction>(v)) {
    return new InstructionNode(inst);
  }
  else if (auto *cnst = dyn_cast<Constant>(v)) {
    return new ConstantNode(cnst);
  }
  else if (auto *arg = dyn_cast<Argument>(v)) {
    return new ArgumentNode(arg);
  }
  else if (auto *bb = dyn_cast<BasicBlock>(v)) {
    return new BasicBlockNode(bb);
  }
  else {
    assert(false);
  }
}

// See header file.
Node* PassUtils::create(Value *v, std::map<Value*,Node*> &valueMap, DependencyGraph &depGraph) {
  auto node = createDynamicNode(v);
  depGraph.addNode(node);
  valueMap[v] = node;
  return node;
}

// See header file.
Node* PassUtils::createOrFind(Value *v, std::map<Value*,Node*> &valueMap, DependencyGraph &depGraph) {
  Node* node;
  // Add a node for the instruction if one doesn't exist
  if (valueMap.find(v) == valueMap.end()) {
    node = createDynamicNode(v);
    depGraph.addNode(node);
    valueMap[v] = node;
  } else {
    node = valueMap.at(v);
  }
  return node;
}