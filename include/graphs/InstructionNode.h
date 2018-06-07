#ifndef APOLLO_GRAPHS_INSTRUCTIONNODE
#define APOLLO_GRAPHS_INSTRUCTIONNODE

// Pulling in various LLVM classes for prototype signatures.
#include "llvm/IR/Instruction.h"
#include "llvm/Support/Casting.h"

// Pull in the base node type.
#include "graphs/Node.h"

// To avoid having to preface every LLVM class name.
using namespace llvm;

// Shared namespace within the project.
namespace apollo {

// Wrapper around LLVM Instruction types.
class InstructionNode : public Node {
public:
  /* Constructor for the Instruction-based node type.
   *     [inst]: The LLVM Instruction around which to wrap this node.
   *
   * Override: Maintain behavior of the base node class.
   */
  InstructionNode(const Instruction *inst) : Node(inst) {
    parent = inst->getParent();
  }

  /* Destructor for the Instruction-based node type.
   *
   * Override: Maintain behavior of the base node class.
   */
  ~InstructionNode() { }

  /* [getBasicBlock] returns a pointer to the basic block that contains this node
   *   in the original LLVM IR, if such a notion makes sense.
   *
   * Override: Returns the actual saved parent pointer.
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