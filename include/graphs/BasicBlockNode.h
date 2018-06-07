#ifndef APOLLO_GRAPHS_BASICBLOCKNODE
#define APOLLO_GRAPHS_BASICBLOCKNODE

// Pulling in various LLVM classes for prototype signatures.
#include "llvm/IR/BasicBlock.h"
#include "llvm/Support/Casting.h"

// Pull in the base node type.
#include "graphs/Node.h"

// To avoid having to preface every LLVM class name.
using namespace llvm;

// Shared namespace within the project.
namespace apollo {

// Wrapper around LLVM BasicBlock types.
class BasicBlockNode : public Node {
public:
  /* Constructor for the BasicBlock-based node type.
   *     [bb]: The LLVM BasicBlock around which to wrap this node.
   *
   * Override: Maintain behavior of the base node class.
   */
  BasicBlockNode(const BasicBlock *bb) : Node(bb) {
    name = bb->getName();
  }

  /* Destructor for the BasicBlock-based node type.
   *
   * Override: Maintain behavior of the base node class.
   */
  ~BasicBlockNode() { }

  /* [getName] returns the name of the LLVM Value around which this node wraps.
   *
   * Override: Provides the name of the basic block itself.
   */
  virtual std::string getName() override;

  /* [getBasicBlock] returns a pointer to the basic block that contains this node
   *   in the original LLVM IR, if such a notion makes sense.
   *
   * Override: Returns a pointer to the wrapped basic block itself.
   */
  virtual const BasicBlock* getBasicBlock() override;

  /* [classof] returns true if [n] is of this class' type. Returns false otherwise.
   *   For LLVM-style RTTI support in the local type hierarchy.
   */
  static bool classof(const Node *n);

private:
  // The overridden name for this node class, based on the wrapped BasicBlock's
  // name rather than the overlaying Value's intrinsic name.
  std::string name;
};

}

#endif