// Pull in the function prototypes.
#include "graphs/nodes/ConstantNode.h"

// Shared namespace within the project.
using namespace apollo;

// See header file.
bool ConstantNode::classof(const Node *n) {
  return (n->getKind() == Node_Constant);
}