#ifndef APOLLO_GRAPHS_NODEINFO
#define APOLLO_GRAPHS_NODEINFO

// Pull in some standard data structures.
#include <map>

// Shared namespace within the project.
namespace apollo {

// A type for each of the nodes in the hierarchy, needed for LLVM-style RTTI.
enum NodeKind {
  Kind_Base,        // Node (ValueNode)
  Kind_Instruction, // InstructionNode
  Kind_BasicBlock,  // BasicBlockNode
  Kind_Constant,    // ConstantNode
  Kind_Argument     // ArgumentNode
};

// Utility class for retrieving information about the various node types.
class NodeInfo {
public:
  /* [toString] returns a string representation of the node type [nk].
   *     [nk]: A kind of node in the class hierarchy.
   *
   * Non-overridable.
   */
  static std::string toString(NodeKind nk);

private:
  // Keep a mapping between node kinds and their string representations.
  static const std::map<NodeKind, std::string> strMap;

  /* [createStringMap] creates a mapping between node kinds and their equivalent
   *   string representations. Utility for the [toString] function.
   *
   * Non-overridable.
   */
  static std::map<NodeKind, std::string> createStringMap();
};

}

#endif