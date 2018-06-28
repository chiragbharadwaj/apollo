#ifndef APOLLO_PASSES_DEPENDENCEPASS
#define APOLLO_PASSES_DEPENDENCEPASS

// Pull in some standard data structures.
#include <map>

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
  class DependencePass : public FunctionPass {
  public:
    // Identifier for this pass, a replacement for typeid.
    static char ID;

    /* Simple constructor that just invokes the parent constructor by default and
     *   initializes the internal state variables.
     */
    DependencePass()
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

    /* [getValueMap] returns the mapping between LLVM Values and the nodes in the
     *   internal static program graph, for ease of use/operability.
     *
     * Non-overridable.
     */
    std::map<Value*,Node*> getValueMap();

  private:
    // An internal representation of the graph with "filtered" edge types.
    DependencyGraph depGraph;

    // A mapping between LLVM Values and nodes, to avoid re-insertions.
    std::map<Value*,Node*> valueMap;

    /* [addDataEdges] statically analyzes [func] and adds data-dependence edges
     *   to the internal static program graph.
     *     [func]: The function to statically analyze.
     *
     * Non-overridable.
     */
    void addDataEdges(Function &func);

    /* [addControlEdges] statically analyzes [func] and adds control-flow edges
     *   to the internal static program graph.
     *     [func]: The function to statically analyze.
     *
     * Non-overridable.
     */
    void addControlEdges(Function &func);

    /* [addPhiEdges] statically analyzes [func] and adds control-flow edges to
     *   phi nodes in the internal static program graph. The phi-edges are simply
     *   a type of control-flow dependency.
     *     [func]: The function to statically analyze.
     *
     * Non-overridable.
     */
    void addPhiEdges(Function &func);

    /* [addMemoryEdges] statically analyzes [func] and adds both known memory
     *   address-aliasing edges as well as potential address-aliasing edges in
     *   the internal static program graph. These are also referred to as the
     *   "memory edges" and "memory maybe edges".
     *     [func]: The function to statically analyze.
     *
     * Non-overridable.
     */
    void addMemoryEdges(Function &func);

    /* [addLoopCarryEdges] statically analyzes [func] and adds edges between
     *   nodes in the internal static program graph for which a data dependence
     *   can occur between successive loop iterations. Like with [addMemoryEdges],
     *   identifies both the known dependencies as well as the potential ones.
     *   These are also referred to as the "loop-carried edges" and the "loop-
     *   carried maybe edges".
     *     [func]: The function to statically analyze.
     *
     * Non-overridable.
     */
    void addLoopCarryEdges(Function &func);

    /* [exportToFiles] creates a text file containing a representation of the
     *   internal static program graph created by the various dependency analyses.
     *   Also generates a visualization file containing a DOT representation of
     *   the graph that Graphviz can easily read. The text file is human readable.
     *
     * Non-overridable.
     */
    void exportToFiles();
  };

}

#endif