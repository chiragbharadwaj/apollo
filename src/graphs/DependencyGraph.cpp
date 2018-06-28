// Pull in the function prototypes.
#include "graphs/DependencyGraph.h"

// Shared namespace within the project.
using namespace apollo;

// See header file.
void DependencyGraph::eraseConnections(Node *v, EdgeMap em) {
  // Loop through and remove all edge connections to this node.
  for (const auto &entry : em) {
    auto adjs = entry.second;
    adjs.erase(v);
  }
  // Finally, remove this node itself.
  em.erase(v);
}

// See header file.
void DependencyGraph::eraseConnections(Node *v, WeightedEdgeMap wem) {
  // Loop through and remove all edge connections to this node.
  for (const auto &entry : wem) {
    auto adjs = entry.second;
    adjs.erase(v);
  }
  // Finally, remove this node itself.
  wem.erase(v);
}

// See header file.
void DependencyGraph::clear() {
  // Call the base graph's function first.
  Graph<Node>::clear();

  // Then extend the functionality.
  controlEdges.clear();
  dataEdges.clear();
  memoryEdges.clear();
  memoryMaybeEdges.clear();
  phiDataEdges.clear();
  loopCarryEdges.clear();
  loopCarryMaybeEdges.clear();
}

// See header file.
void DependencyGraph::removeNode(Node *v) {
  // Call the base graph's function first.
  Graph<Node>::removeNode(v);

  // Then extend the functionality.
  eraseConnections(v, controlEdges);
  eraseConnections(v, dataEdges);
  eraseConnections(v, memoryEdges);
  eraseConnections(v, memoryMaybeEdges);
  eraseConnections(v, phiDataEdges);
  eraseConnections(v, loopCarryEdges);
  eraseConnections(v, loopCarryMaybeEdges);
}

// See header file.
void DependencyGraph::addEdge(Node *u, Node *v, EdgeKind ek, long dist) {
  // Call the base graph's function first.
  Graph<Node>::addEdge(u, v);

  // Then extend the functionality.
  switch(ek) {
    case Edge_Control:
      controlEdges[u].insert(v);
      break;
    case Edge_Data:
      dataEdges[u].insert(v);
      break;
    case Edge_Memory:
      memoryEdges[u].insert(v);
      break;
    case Edge_MemoryMaybe:
      memoryMaybeEdges[u].insert(v);
      break;
    case Edge_PhiData:
      phiDataEdges[u].insert(v);
      break;
    case Edge_LoopCarry:
      loopCarryEdges[u].insert({v,dist});
      break;
    case Edge_LoopCarryMaybe:
      loopCarryMaybeEdges[u].insert({v,dist});
      break;
    default:
      break;
  }
}

// See header file.
void DependencyGraph::removeEdge(Node *u, Node *v) {
  // Call the base graph's function first.
  Graph<Node>::removeEdge(u, v);

  // Then extend the functionality.
  controlEdges       .at(u).erase(v);
  dataEdges          .at(u).erase(v);
  memoryEdges        .at(u).erase(v);
  memoryMaybeEdges   .at(u).erase(v);
  phiDataEdges       .at(u).erase(v);
  loopCarryEdges     .at(u).erase(v);
  loopCarryMaybeEdges.at(u).erase(v);
}

// See header file.
const DependencyGraph::EdgeMap DependencyGraph::getControlEdges() {
  return controlEdges;
}

// See header file.
const DependencyGraph::EdgeMap DependencyGraph::getDataEdges() {
  return dataEdges;
}

// See header file.
const DependencyGraph::EdgeMap DependencyGraph::getMemoryEdges() {
  return memoryEdges;
}

// See header file.
const DependencyGraph::EdgeMap DependencyGraph::getMemoryMaybeEdges() {
  return memoryMaybeEdges;
}

// See header file.
const DependencyGraph::EdgeMap DependencyGraph::getPhiDataEdges() {
  return phiDataEdges;
}

// See header file.
const DependencyGraph::WeightedEdgeMap DependencyGraph::getLoopCarryEdges() {
  return loopCarryEdges;
}

// See header file.
const DependencyGraph::WeightedEdgeMap DependencyGraph::getLoopCarryMaybeEdges() {
  return loopCarryMaybeEdges;
}