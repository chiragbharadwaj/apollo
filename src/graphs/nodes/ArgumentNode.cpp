// Pull in the function prototypes.
#include "graphs/nodes/ArgumentNode.h"

// Shared namespace within the project.
using namespace apollo;

// See header file.
bool ArgumentNode::classof(const Node *n) {
  return (n->getKind() == Node_Argument);
}