// Pull in the function prototypes.
#include "graphs/OperatorNode.h"

// Pull in the various visitor classes.
#include "visitors/VisualizationVisitor.h"

// Shared namespace within the project.
using namespace apollo;

OperatorNode::OperatorNode(Operator *op)
  : BaseNode(ApolloOperator), op(op) { }

OperatorNode::~OperatorNode()
  { }

void OperatorNode::accept(VisualizationVisitor &v) {
  v.visit(this);
}