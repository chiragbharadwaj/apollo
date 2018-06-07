#ifndef APOLLO_GRAPHS_CONSTANTNODE
#define APOLLO_GRAPHS_CONSTANTNODE

// Pulling in various LLVM classes for prototype signatures.
#include "llvm/IR/Constant.h"
#include "llvm/Support/Casting.h"

// Pull in the base node type.
#include "graphs/Node.h"

// Pulling in the node type discriminator for LLVM-style class hierarchies.
#include "graphs/NodeInfo.h"

// To avoid having to preface every LLVM class name.
using namespace llvm;

// Shared namespace within the project.
namespace apollo {

// Wrapper around LLVM Constant types.
class ConstantNode : public Node {
public:
  /* Constructor for the Constant-based node type.
   *     [bb]: The LLVM Constant around which to wrap this node.
   *
   * Override: Maintain behavior of the base node class.
   */
  ConstantNode(const Constant *cnst) : Node(Kind_Constant, cnst) { }

  /* Destructor for the Constant-based node type.
   *
   * Override: Maintain behavior of the base node class.
   */
  ~ConstantNode() { }

  /* [classof] returns true if [n] is of this class' type. Returns false otherwise.
   *   For LLVM-style RTTI support in the local type hierarchy.
   */
  static bool classof(const Node *n);
};

}

#endif