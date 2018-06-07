// Pull in the function prototypes.
#include "graphs/Node.h"

// Shared namespace within the project.
using namespace apollo;

// See header file.
int Node::getID() {
  return id;
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