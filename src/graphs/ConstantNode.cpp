// Pull in the function prototypes.
#include "graphs/ConstantNode.h"
#include "graphs/NodeInfo.h"

// Shared namespace within the project.
using namespace apollo;

// See header file.
bool ConstantNode::classof(const Node *n) {
  return (n->getKind() == Kind_Constant);
}