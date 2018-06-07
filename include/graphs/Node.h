#ifndef APOLLO_GRAPHS_NODE
#define APOLLO_GRAPHS_NODE

// Pull in assertions to guarantee run-time safety for invariants.
#include <assert.h>

// Pulling in various LLVM classes for prototype signatures.
#include "llvm/IR/Value.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Casting.h"

// To avoid having to preface every LLVM class name.
using namespace llvm;

// Shared namespace within the project.
namespace apollo {

// Base class for all custom node types. Wraps around LLVM Values. Other wrapper
// classes for fine-grained LLVM types simply inherit from this base class.
class Node {
public:
  /* Constructor for the default node type.
   *     [v]: The LLVM Value around which to wrap this node.
   *
   * Default: Increments the internal number of alive nodes for the next node
   *          and saves the name of the Value.
   */
  Node(const Value *v) : val(v) {
    id++;
    raw_string_ostream stream(name);
    v->print(stream);
    name = stream.str();
  }

  /* Destructor for the default node type.
   *
   * Default: Decrements the internal number of alive nodes for the next node.
   */
  ~Node() {
    assert(id > 0); // This should never fail
    id--;
  }

  /* Returns the internal ID corresponding to this node at creation-time.
   *
   * Non-overridable.
   */
  int getID();

  /* [getValue] returns a pointer to the LLVM Value around which this node wraps.
   *
   * Non-overridable.
   */
  const Value* getValue();

  /* [getName] returns the name of the LLVM Value around which this node wraps.
   *
   * Default: Provides the name created by the internal Value printer.
   */
  virtual std::string getName();

  /* [getBasicBlock] returns a pointer to the basic block that contains this node
   *   in the original LLVM IR, if such a notion makes sense.
   *
   * Default: Returns a null pointer.
   */
  virtual const BasicBlock* getBasicBlock();

private:
  // A name corresponding to this node.
  std::string name;

  // An LLVM Value, which is the top-most type in the hierarchy.
  const Value *val;

  // An ID corresponding to the number of alive nodes at creation-time.
  static int id;
};

// Provide a simple initialization for the internal counter.
int Node::id = 0;

}

#endif