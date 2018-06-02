// Pull in the function prototypes.
#include "graph/Graph.h"

// Shared namespace within the project.
using namespace apollo;

// See header file for comments.
template <typename T>
bool Graph<T>::isEmpty() {
  return nodes.empty();
}

// See header file for comments.
template <typename T>
void Graph<T>::clear() {
  nodes.clear();
}

// See header file for comments.
template <typename T>
int Graph<T>::deg(T *v) {
  return nodes.at(v).size();
}

// See header file for comments.
template <typename T>
bool Graph<T>::isAdj(T *u, T *v) {
  return (nodes[u].find(v) != nodes[v].end());
}

// See header file for comments.
template <typename T>
const std::set<T*> Graph<T>::adj(T *v) {
  return nodes.at(v);
}

// See header file for comments.
template <typename T>
void Graph<T>::addNode(T *v) {
  nodes[v];
}

// See header file for comments.
template <typename T>
void Graph<T>::removeNode(T *v) {
  // Loop through and remove all edge connections to this node.
  for (const auto &entry : nodes) {
    auto node = entry.first;
    auto adjs = entry.second;
    adjs.erase(v);
  }
  // Finally, remove this node itself.
  nodes.erase(v);
}

// See header file for comments.
template <typename T>
void Graph<T>::addEdge(T *u, T *v) {
  nodes[u].insert(v);
}

// See header file for comments.
template <typename T>
void Graph<T>::removeEdge(T *u, T *v) {
  nodes.at(u).erase(v);
}

// See header file for comments.
template <typename T>
typename Graph<T>::iterator Graph<T>::begin() {
  return nodes.begin();
}

// See header file for comments.
template <typename T>
typename Graph<T>::iterator Graph<T>::end() {
  return nodes.end();
}

// See header file for comments.
template <typename T>
typename Graph<T>::const_iterator Graph<T>::cbegin() {
  return nodes.cbegin();
}

// See header file for comments.
template <typename T>
typename Graph<T>::const_iterator Graph<T>::cend() {
  return nodes.cend();
}