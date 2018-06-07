// Pull in the function prototypes.
#include "graphs/BasicBlockNode.h"

// Shared namespace within the project.
using namespace apollo;

// See header file.
std::string BasicBlockNode::getName() {
  return name;
}

// See header file.
const BasicBlock* BasicBlockNode::getBasicBlock() {
  const Value *v = this->getValue();
  return dyn_cast<BasicBlock>(v); // Should work without failing!
}

// See header file.
bool BasicBlockNode::classof(const Node *n) {
  return false;
}