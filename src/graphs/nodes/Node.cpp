// Pull in the function prototypes.
#include "graphs/nodes/Node.h"

// Shared namespace within the project.
using namespace apollo;

// See header file.
int Node::id = 0;

// See header file.
int Node::getMonotonicID() {
  return id;
}

// See header file.
const NodeKind Node::getKind() const {
  return kind;
}

// See header file.
const Value* Node::getValue() {
  return val;
}

// See header file.
std::string Node::getName() {
  return name;
}

// See header file.
const BasicBlock* Node::getBasicBlock() {
  return nullptr;
}

// See header file.
bool Node::classof(const Node *n) {
  return (n->getKind() >= Node_Base) &&
         (n->getKind() <= Node_Argument);
}