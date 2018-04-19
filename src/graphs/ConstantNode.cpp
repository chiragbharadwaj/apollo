// Pull in the function prototypes.
#include "graphs/ConstantNode.h"

// Pull in the various visitor classes.
#include "visitors/Visitors.h"

// Shared namespace within the project.
using namespace apollo;

ConstantNode::ConstantNode(Constant *con)
  : BaseNode(ApolloConstant), con(con) { }

void ConstantNode::accept(Visitor &v) {
  v.visit(this);
}

void ConstantNode::accept(DependenceVisitor &v) {
  v.visit(this);
}