// Pull in the function prototypes.
#include "graphs/InstructionNode.h"

// Shared namespace within the project.
using namespace apollo;

// See header file.
const BasicBlock* InstructionNode::getBasicBlock() {
  return parent;
}

// See header file.
bool InstructionNode::classof(const Node *n) {
  return false;
}