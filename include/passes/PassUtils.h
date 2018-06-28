#ifndef APOLLO_PASSES_PASSUTILS
#define APOLLO_PASSES_PASSUTILS

// Pull in some standard data structures.
#include <map>

// Include all of the node types for dynamic-type creation.
#include "graphs/nodes/Nodes.h"

// Pull in the dependency graph class for static analysis.
#include "graphs/DependencyGraph.h"

// Pulling in various LLVM classes for prototype signatures.
#include "llvm/IR/Function.h"
#include "llvm/IR/Value.h"

// To avoid having to preface every LLVM class name.
using namespace llvm;

// Shared namespace within the project.
namespace apollo {

// Utility class for static LLVM IR passes.
class PassUtils {
public:
  // The string that we will use in front of functions of interest.
  #define KERNEL_STR "_kernel_"

  /* [isKernelFunction] returns true if [func] has KERNEL_STR in its name.
   *   Returns false otherwise. Intended to act as a filter for functions of
   *   interest when performing static pass analyses on LLVM IR.
   *
   * Non-overridable.
   */
  static bool isKernelFunction(Function &func);

  /* [createDynamicNode] creates a node with an appropriate dynamic type in the
   *   class hierarchy based on the dynamic type of the LLVM Value [v].
   *     [v]: An LLVM Value whose dynamic type determines the dynamic type of
   *          the node that wraps around this argument.
   *
   * Non-overridable.
   */
  static Node* createDynamicNode(Value *v);

  /* [create] creates a node corresponding to [v], inserting it into [valueMap]
   *   and [depGraph]. A pointer to this node is returned.
   *     [v]: An LLVM Value whose presence or absence in the graph is TBD.
   *     [valueMap]: The mapping between LLVM Values and internal node types.
   *     [depGraph]: The dependency graph in which to insert a node wrapping
   *                 around [v].
   *
   * Requires: [v] is not present as a key in [valueMap].
   * Non-overridable.
   */
  static Node* create(Value *v, std::map<Value*,Node*> &valueMap, DependencyGraph &depGraph);

  /* [createOrFind] either creates a node corresponding to [v], inserting it into
   *   [valueMap] and [depGraph], or it retrieves it from [valueMap] if it has
   *   already been inserted. In either case, a pointer to this node is returned.
   *     [v]: An LLVM Value whose presence or absence in the graph is TBD.
   *     [valueMap]: The mapping between LLVM Values and internal node types.
   *     [depGraph]: The dependency graph in which to insert a node wrapping
   *                 around [v] in the case that it is not in the graph already.
   *
   * Non-overridable.
   */
  static Node* createOrFind(Value *v, std::map<Value*,Node*> &valueMap, DependencyGraph &depGraph);
};

}

#endif