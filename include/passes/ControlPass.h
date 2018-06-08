#ifndef APOLLO_PASSES_CONTROLPASS
#define APOLLO_PASSES_CONTROLPASS

// Pulling in various LLVM classes for prototype signatures.
#include "llvm/Pass.h"
#include "llvm/PassAnalysisSupport.h"
#include "llvm/IR/Function.h"
#include "llvm/ADT/StringRef.h"

// Pull in the dependency graph class for static analysis.
#include "graphs/DependencyGraph.h"

// To avoid having to preface every LLVM class name.
using namespace llvm;

// Shared namespace within the project.
namespace apollo {

// Use a pass over functions in the LLVM IR to augment the dependency graph.
class ControlPass : public FunctionPass {
public:
  // Identifier for this pass, a replacement for typeid.
  static char ID;

  /* Simple constructor that just invokes the parent constructor by default and
   *   initializes the internal state variables.
   */
  ControlPass()
    : FunctionPass(ID) { }

  /* [runOnFunction] is called on every function [func] present in the original
   *   program's IR. It statically-analyzes the program structure to augment the
   *   static program graph structure. Returns false, as the pass does NOT modify
   *   the original IR in-place.
   *     [func]: The function on which to run this pass.
   *
   * Requires: [func] is present in the original program's IR.
   */
  virtual bool runOnFunction(Function &func) override;

  /* [getPassName] returns a string representation of this pass' name.
   */
  virtual StringRef getPassName() const override;

  /* [getAnalysisUsage] fills [au] with the appropriate metadata on whether this
   *   pass analyzes or transforms the program IR.
   *     [au]: The pass state manager that tracks pass usages over time.
   */
  void getAnalysisUsage(AnalysisUsage &au) const override;

  /* [getGraph] returns the static program graph that this pass synthesis and
   *   augments as a result of function analysis at the IR level.
   *
   * Non-overridable.
   */
  DependencyGraph getGraph();

private:
  // An internal representation of the graph with "filtered" edge types.
  DependencyGraph depGraph;

  /* [addControlEdges] statically analyzes [func] and adds control-flow edges
   *   to the internal static program graph. Some phi-edges are also added here,
   *   since they can be involved with all types of dependencies.
   */
  void addControlEdges(Function &func);
};

}

#endif