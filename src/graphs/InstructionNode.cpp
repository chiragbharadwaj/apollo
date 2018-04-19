// Pull in the function prototypes.
#include "graphs/InstructionNode.h"

// Pull in the various visitor classes.
#include "visitors/Visitors.h"

// Shared namespace within the project.
using namespace apollo;

InstructionNode::InstructionNode(Instruction *inst)
  : BaseNode(ApolloInstruction), inst(inst) { }

void InstructionNode::accept(Visitor &v) {
  v.visit(this);
}

void InstructionNode::accept(DependenceVisitor &v) {
  v.visit(this);
}