#ifndef APOLLO_GRAPHS_BASENODE
#define APOLLO_GRAPHS_BASENODE

// Pull in the dynamic type lookup information.
#include "graphs/Type.h"

// Pull in LLVM-style RTTI functions.
#include "llvm/Support/Casting.h"

// Shared namespace within the project.
namespace apollo {

// All the types of visitors that are allowed to exist in our custom graph types.
// Necessary for the visit() and accept() functions when using the recursive
// Visitor pattern in node classes.
class Visitor;
class DependenceVisitor;

// Base class for all nodes (including top-level graph nodes).
class BaseNode {
public:
  /* Constructor for all nodes.
   *     [t]: The dynamic type of the node. Cannot be changed.
   *
   * Default: Just save the dynamic node type.
   */
  BaseNode(const NodeType t) : type(t) { }

  /* Destructor for all nodes.
   *
   * Default: Use C++'s default destruction process.
   */
  virtual ~BaseNode() { }

  /* Returns the dynamic type of the node being used.
   *
   * Default: Non-overridable.
   */
  const NodeType getType() const {
    return type;
  }

  /* [accept] records actions from the generic visitor [v].
   *   Returns nothing.
   *     [v]: A generic visitor that operates on all types of graphs.
   *
   * Default: Purely virtual.
   */
  virtual void accept(Visitor &v) = 0;

  /* [accept] records actions from the overall-dependence visitor [v].
   *   Returns nothing.
   *     [v]: A visitor that operates on dependence graphs.
   *
   * Default: Purely virtual.
   */
  virtual void accept(DependenceVisitor &v) = 0;

private:
  // The dynamic type of the node being used.
  const NodeType type;
};

}

#endif
