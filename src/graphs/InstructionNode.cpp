// Pull in the function prototypes.
#include "graphs/InstructionNode.h"
#include "graphs/NodeInfo.h"

// Shared namespace within the project.
using namespace apollo;

// See header file.
const BasicBlock* InstructionNode::getBasicBlock() {
  return parent;
}

// See header file.
bool InstructionNode::classof(const Node *n) {
  return (n->getKind() == Kind_Instruction);
}