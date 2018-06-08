// Pull in the function prototypes.
#include "graphs/nodes/NodeInfo.h"

// Shared namespace within the project.
using namespace apollo;

// See header file.
const std::map<NodeKind, std::string>
  NodeInfo::strMap = NodeInfo::createStrMap();

// See header file.
std::string NodeInfo::toString(NodeKind nk) {
  return strMap.at(nk);
}

// See header file.
std::map<NodeKind, std::string> NodeInfo::createStrMap() {
  return { {Node_Base,        "(value) node"},
           {Node_Instruction, "instruction node"},
           {Node_BasicBlock,  "basic-block node"},
           {Node_Constant,    "constant node"},
           {Node_Argument,    "argument node"}
         };
}