// Pull in the function prototypes.
#include "graphs/Node.h"

// Shared namespace within the project.
using namespace apollo;

// See header file.
int Node::getID() {
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
  return (n->getKind() >= Kind_Base) &&
         (n->getKind() <= Kind_Argument);
}