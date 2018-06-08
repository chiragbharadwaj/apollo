#ifndef APOLLO_GRAPHS_DEPENDENCYGRAPH
#define APOLLO_GRAPHS_DEPENDENCYGRAPH

// Pull in some standard data structures.
#include <map>
#include <set>
#include <utility>

// Pull in the base graph class to extend.
#include "graphs/Graph.h"

// Pull in the base node type.
#include "graphs/nodes/Node.h"

// Pulling in the edge type discriminator for fine-grained classification.
#include "graphs/edges/EdgeInfo.h"

// Shared namespace within the project.
namespace apollo {

// Represents the static program graph, which is only over nodes. Wraps around
// the base graph class but augments some of the functionality. Provides "filtered"
// versions of the adjacency list by edge type so that classification is simpler.
class DependencyGraph : public Graph<Node> {
public:
  /* Override: Also clears ALL of the various internal edge lists.
   */
  virtual void clear() override;

  /* Override: Also clears the edges attached to the specified node from ALL of
   *           the various internal edge lists.
   */
  virtual void removeNode(Node *v) override;

  /* Override: Also adds an edge between the two nodes to the internal edge list
   *           corresponding to the specified edge kind. Also adds the weight
   *           given by the specified distance, but only if the edge kind is the
   *           loop-carry dependence kind. This last parameter is optional.
   */
  virtual void addEdge(Node *u, Node *v, EdgeKind ek, int dist = -1);

  /* Override: Also removes the edge between the two nodes from ALL of the various
   *           internal edge lists.
   */
  virtual void removeEdge(Node *u, Node *v) override;

private:
  // Rename the complex types so that they are simpler to read.
  typedef typename std::map<Node*, std::set<Node*>> EdgeMap;
  typedef typename std::map<Node*, std::map<Node*,int>> WeightedEdgeMap;

  /* [eraseConnections] removes all connections from and to [v] in the map [em].
   *   All edges are removed and then [v] itself is removed at the end.
   *     [v]: A node to sever all ties with.
   *     [em]: A map of edges from which to purge the node [v].
   *
   * Non-overridable.
   */
  void eraseConnections(Node *v, EdgeMap em);

  /* [eraseConnections] removes all connections from and to [v] in the map [wem].
   *   All edges are removed and then [v] itself is removed at the end.
   *     [v]: A node to sever all ties with.
   *     [wem]: A map of weighted edges from which to purge the node [v].
   *
   * Non-overridable.
   */
  void eraseConnections(Node *v, WeightedEdgeMap wem);

  // The list of all control-flow edges.
  EdgeMap controlEdges;

  // The list of all data-dependence edges.
  EdgeMap dataEdges;

  // The list of all certain memory-dependence edges.
  EdgeMap memoryEdges;

  // The list of all unknown memory-dependence edges.
  EdgeMap memoryMaybeEdges;

  // The list of all phi-node data-dependence edges.
  EdgeMap phiDataEdges;

  // The list of all certain loop-carried memory-dependence edges. These edges
  // are special because they are also weighted, i.e. they carry distance info.
  WeightedEdgeMap loopCarryEdges;

  // The list of all unknown loop-carried memory-dependence edges. These edges
  // are special because they are also weighted, i.e. they carry distance info.
  WeightedEdgeMap loopCarryMaybeEdges;
};

}

#endif