#ifndef APOLLO_GRAPHS_ARGUMENTNODE
#define APOLLO_GRAPHS_ARGUMENTNODE

// Pulling in various LLVM classes for prototype signatures.
#include "llvm/IR/Argument.h"
#include "llvm/Support/Casting.h"

// Pull in the base node type.
#include "graphs/Node.h"

// Pulling in the node type discriminator for LLVM-style class hierarchies.
#include "graphs/NodeInfo.h"

// To avoid having to preface every LLVM class name.
using namespace llvm;

// Shared namespace within the project.
namespace apollo {

// Wrapper around LLVM Argument types.
class ArgumentNode : public Node {
public:
  /* Constructor for the Argument-based node type.
   *     [bb]: The LLVM Argument around which to wrap this node.
   *
   * Override: Maintain behavior of the base node class.
   */
  ArgumentNode(const Argument *arg) : Node(Kind_Argument, arg) { }

  /* Destructor for the Argument-based node type.
   *
   * Override: Maintain behavior of the base node class.
   */
  ~ArgumentNode() { }

  /* [classof] returns true if [n] is of this class' type. Returns false otherwise.
   *   For LLVM-style RTTI support in the local type hierarchy.
   */
  static bool classof(const Node *n);
};

}

#endif