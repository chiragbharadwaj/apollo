// Pull in the function prototypes.
#include "graphs/ArgumentNode.h"
#include "graphs/NodeInfo.h"

// Shared namespace within the project.
using namespace apollo;

// See header file.
bool ArgumentNode::classof(const Node *n) {
  return (n->getKind() == Kind_Argument);
}