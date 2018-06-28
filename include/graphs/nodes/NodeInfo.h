#ifndef APOLLO_GRAPHS_NODES_NODEINFO
#define APOLLO_GRAPHS_NODES_NODEINFO

// Pull in some standard data structures.
#include <map>
#include <string>

// Shared namespace within the project.
namespace apollo {

// A type for each of the nodes in the hierarchy, needed for LLVM-style RTTI.
  enum NodeKind {
    Node_Base,        // Node (ValueNode)
    Node_Instruction, // InstructionNode
    Node_BasicBlock,  // BasicBlockNode
    Node_Constant,    // ConstantNode
    Node_Argument     // ArgumentNode
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

    /* [createStrMap] creates a mapping between node kinds and their equivalent
     *   string representations. Utility for the [toString] function.
     *
     * Non-overridable.
     */
    static std::map<NodeKind, std::string> createStrMap();
  };

}

#endif