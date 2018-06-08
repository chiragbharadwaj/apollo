#ifndef APOLLO_GRAPHS_GRAPH
#define APOLLO_GRAPHS_GRAPH

// Pull in some standard data structures.
#include <map>
#include <set>

// Shared namespace within the project.
namespace apollo {

// Base class for all graphs, parameterized on a node/vertex type.
template <typename V>
class Graph {
public:
  // Iterator on the actual nodes themselves.
  typedef typename std::map<V*, std::set<V*>>::iterator
    iterator;
  // Constant version of the iterator on actual nodes themselves.
  typedef typename std::map<V*, std::set<V*>>::const_iterator
    const_iterator;

  /* [isEmpty] returns true if the graph is empty (i.e. it returns whether the
   *   graph contains no nodes).
   *
   * Default: As above.
   */
  virtual bool isEmpty();

  /* [clear] clears the graph so that it contains no nodes or edges.
   *
   * Default: As above.
   */
  virtual void clear();

  /* [size] returns the number of nodes in the graph.
   *
   * Default: As above.
   */
  virtual int size();

  /* [deg] returns the (out-)degree of the node [v] in the graph.
   *     [v]: The node whose degree is to be determined.
   *
   * Requires: [v] is present in the graph.
   * Default: As above.
   */
  virtual int deg(V *v);

  /* [isAdj] returns true if and only if [v] is adjacent to [u] in the graph,
   *   i.e. it returns whether [v] is in the adjacency list of [u].
   *     [u]: The node to be tested against for adjacency.
   *     [v]: The node source to test.
   *
   * Requires: [u] and [v] are present in the graph.
   * Default: As above.
   */
  virtual bool isAdj(V *u, V *v);

  /* [adjs] returns the list of nodes that are adjacent to [v] in the graph, i.e.
   *   it returns the adjacency list of [v].
   *     [v]: The node whose adjacency list is to be determined.
   *
   * Requires: [v] is present in the graph.
   * Default: As above.
   */
  virtual const std::set<V*> adjs(V *v);

  /* [addNode] adds the node [v] to the graph. It is initially not adjacent
   *   to any other node, i.e. its adjacency list is empty.
   *     [v]: The node to add.
   *
   * Requires: [v] is not already present in the graph.
   * Default: As above.
   */
  virtual void addNode(V *v);

  /* [removeNode] deletes the node [v] from the graph.
   *     [v]: The node to remove.
   *
   * Requires: [v] is present in the graph.
   * Default: As above.
   */
  virtual void removeNode(V *v);

  /* [addEdge] adds a directed edge from [u] to [v] in the graph. Thus, [v] is
   *   added to the end of the adjacency list of [u]. Parallel edges are not
   *   allowed, so this operation will fail if there is already an edge u->v.
   *     [u]: The starting node of the directed edge.
   *     [v]: The ending node of the directed edge.
   *
   * Requires: [u] and [v] are present in the graph.
   * Default: As above.
   */
  virtual void addEdge(V *u, V *v);

  /* [removeEdge] deletes the directed edge from [u] to [v] in the graph. Thus,
   *   [v] is removed from the adjacency list of [u].
   *     [u]: The starting node of the directed edge to remove.
   *     [v]: The ending node of the directed edge to remove.
   *
   * Requires: [v] is in the adjacency list of [u] in the graph.
   * Default: As above.
   */
  virtual void removeEdge(V *u, V *v);

  /* [begin] returns the starting position for looping over this graph with a
   *   simple iterator. Useful for range-based for loops in C++14.
   *
   * Non-overridable.
   */
  iterator begin();

  /* [end] returns the ending position for looping over this graph with a
   *   simple iterator. Useful for range-based for loops in C++14.
   *
   * Non-overridable.
   */
  iterator end();

  /* [begin] returns the starting position for looping over this graph with a
   *   constant iterator. Useful for range-based for loops in C++14.
   *
   * Non-overridable.
   */
  const_iterator cbegin();

  /* [end] returns the ending position for looping over this graph with a
   *   constant iterator. Useful for range-based for loops in C++14.
   *
   * Non-overridable.
   */
  const_iterator cend();

private:
  // Actual graph structure, represented as an adjacency list (set) of nodes.
  std::map<V*, std::set<V*>> adjList;
};

}

#endif