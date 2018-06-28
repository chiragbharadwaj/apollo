#ifndef APOLLO_GRAPHS_NODES_ARGUMENTNODE
#define APOLLO_GRAPHS_NODES_ARGUMENTNODE

// Pulling in various LLVM classes for prototype signatures.
#include "llvm/IR/Argument.h"
#include "llvm/Support/Casting.h"

// Pull in the base node type.
#include "graphs/nodes/Node.h"

// To avoid having to preface every LLVM class name.
using namespace llvm;

// Shared namespace within the project.
namespace apollo {

// Wrapper around LLVM Argument types.
class ArgumentNode : public Node {
public:
  /* Override: Maintain behavior of the base node class.
   */
  ArgumentNode(const Argument *arg) : Node(Node_Argument, arg) { }

  /* Override: Maintain behavior of the base node class.
   */
  ~ArgumentNode() { }

  /* [classof] returns true if [n] is of this class' type. Returns false otherwise.
   *   For LLVM-style RTTI support in the local type hierarchy.
   */
  static bool classof(const Node *n);
};

}

#endif