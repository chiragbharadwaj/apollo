#ifndef APOLLO_GRAPHS_NODES_INSTRUCTIONNODE
#define APOLLO_GRAPHS_NODES_INSTRUCTIONNODE

// Pulling in various LLVM classes for prototype signatures.
#include "llvm/IR/Instruction.h"
#include "llvm/Support/Casting.h"

// Pull in the base node type.
#include "graphs/nodes/Node.h"

// To avoid having to preface every LLVM class name.
using namespace llvm;

// Shared namespace within the project.
namespace apollo {

// Wrapper around LLVM Instruction types.
class InstructionNode : public Node {
public:
  /* Override: Maintain behavior of the base node class.
   */
  InstructionNode(const Instruction *inst) : Node(Node_Instruction, inst) {
    parent = inst->getParent();
  }

  /* Override: Maintain behavior of the base node class.
   */
  ~InstructionNode() { }

  /* Override: Returns the actual saved parent pointer.
   */
  virtual const BasicBlock* getBasicBlock() override;

  /* [classof] returns true if [n] is of this class' type. Returns false otherwise.
   *   For LLVM-style RTTI support in the local type hierarchy.
   */
  static bool classof(const Node *n);

private:
  // A pointer to the basic block that is the parent of the LLVM Instruction.
  const BasicBlock* parent;
};

}

#endif