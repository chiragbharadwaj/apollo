#ifndef APOLLO_GRAPHS_EDGES_EDGEINFO
#define APOLLO_GRAPHS_EDGES_EDGEINFO

// Pull in some standard data structures.
#include <map>
#include <string>

// Shared namespace within the project.
namespace apollo {

// A type for each of the edges in the static program graph, for classification.
enum EdgeKind {
  Edge_Control,
  Edge_Data,
  Edge_Memory,
  Edge_MemoryMaybe,
  Edge_PhiData,
  Edge_LoopCarry,
  Edge_LoopCarryMaybe
};

// Utility class for retrieving information about the various edge types.
class EdgeInfo {
public:
  /* [toString] returns a string representation of the edge type [ek].
   *     [ek]: A kind of edge in static program graphs.
   *
   * Non-overridable.
   */
  static std::string toString(EdgeKind ek);

private:
  // Keep a mapping between edge kinds and their string representations.
  static const std::map<EdgeKind, std::string> strMap;

  /* [createStrMap] creates a mapping between edge kinds and their equivalent
   *   string representations. Utility for the [toString] function.
   *
   * Non-overridable.
   */
  static std::map<EdgeKind, std::string> createStrMap();
};

}

#endif