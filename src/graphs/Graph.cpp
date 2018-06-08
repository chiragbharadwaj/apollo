// Pull in the function prototypes.
#include "graphs/Graph.h"

// Shared namespace within the project.
using namespace apollo;

// See header file.
template <typename V>
bool Graph<V>::isEmpty() {
  return adjList.empty();
}

// See header file.
template <typename V>
void Graph<V>::clear() {
  adjList.clear();
}

// See header file.
template <typename V>
int Graph<V>::size() {
  return adjList.size();
}

// See header file.
template <typename V>
int Graph<V>::deg(V *v) {
  return adjList.at(v).size();
}

// See header file.
template <typename V>
bool Graph<V>::isAdj(V *u, V *v) {
  return (adjList[u].find(v) != adjList[v].end());
}

// See header file.
template <typename V>
const std::set<V*> Graph<V>::adjs(V *v) {
  return adjList.at(v);
}

// See header file.
template <typename V>
void Graph<V>::addNode(V *v) {
  adjList[v];
}

// See header file.
template <typename V>
void Graph<V>::removeNode(V *v) {
  // Loop through and remove all edge connections to this node.
  for (const auto &entry : adjList) {
    auto node = entry.first;
    auto adjs = entry.second;
    adjs.erase(v);
  }
  // Finally, remove this node itself.
  adjList.erase(v);
}

// See header file.
template <typename V>
void Graph<V>::addEdge(V *u, V *v) {
  adjList[u].insert(v);
}

// See header file.
template <typename V>
void Graph<V>::removeEdge(V *u, V *v) {
  adjList.at(u).erase(v);
}

// See header file.
template <typename V>
typename Graph<V>::iterator Graph<V>::begin() {
  return adjList.begin();
}

// See header file.
template <typename V>
typename Graph<V>::iterator Graph<V>::end() {
  return adjList.end();
}

// See header file.
template <typename V>
typename Graph<V>::const_iterator Graph<V>::cbegin() {
  return adjList.cbegin();
}

// See header file.
template <typename V>
typename Graph<V>::const_iterator Graph<V>::cend() {
  return adjList.cend();
}