// Pull in the function prototypes.
#include "graphs/NodeInfo.h"

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
  return { {Kind_Base,        "Node"},
           {Kind_Instruction, "InstructionNode"},
           {Kind_BasicBlock,  "BasicBlockNode"},
           {Kind_Constant,    "ConstantNode"},
           {Kind_Argument,    "ArgumentNode"}
         };
}