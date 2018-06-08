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
  if (Instruction *inst = dyn_cast<Instruction>(v)) {
    return new InstructionNode(inst);
  }
  else if (Constant *cnst = dyn_cast<Constant>(v)) {
    return new ConstantNode(cnst);
  }
  else if (Argument *arg = dyn_cast<Argument>(v)) {
    return new ArgumentNode(arg);
  }
  else if (BasicBlock *bb = dyn_cast<BasicBlock>(v)) {
    return new BasicBlockNode(bb);
  }
  else {
    assert(false);
  }
}