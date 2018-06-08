// Pull in the function prototypes.
#include "graphs/edges/EdgeInfo.h"

// Shared namespace within the project.
using namespace apollo;

// See header file.
const std::map<EdgeKind, std::string>
  EdgeInfo::strMap = EdgeInfo::createStrMap();

// See header file.
std::string EdgeInfo::toString(EdgeKind nk) {
  return strMap.at(nk);
}

// See header file.
std::map<EdgeKind, std::string> EdgeInfo::createStrMap() {
  return { {Edge_Control,        "control dependence"},
           {Edge_Data,           "data dependence, regular"},
           {Edge_Memory,         "memory dependence"},
           {Edge_MemoryMaybe,    "memory dependence, unknown"},
           {Edge_PhiData,        "data dependence, phi-node"},
           {Edge_LoopCarry,      "loop-carried dependence"},
           {Edge_LoopCarryMaybe, "loop-carried dependence, unknown"},
         };
}