#ifndef GRAPH_H
#define GRAPH_H
#include "../common.h"

using namespace std;

typedef enum {DATA_DEP, PHI_DEP} TEdge;
typedef enum {I_ADDSUB, I_MULT, I_DIV, I_REM, FP_ADDSUB, FP_MULT, FP_DIV, FP_REM, LOGICAL, 
              CAST, GEP, LD, ST, TERMINATOR, PHI} TInstr;

class Node {
public:
  int id;
  TInstr typeInstr;
  int bbid;
  std::string name;
  int lat;
  std::set<Node*> dependents;
  std::set<Node*> parents;
  std::set<Node*> external_dependents;
  std::set<Node*> external_parents;
  std::set<Node*> phi_dependents;
  std::set<Node*> phi_parents;
  std::set<Node*> store_addr_dependents;
  bool isSupp() {
    return (name.find("Supply")!=string::npos);
  }
  
  bool isComp() {
    return (name.find("Compute")!=string::npos);
  }

  Node(int id, TInstr typeInstr, int bbid, std::string node_name, int lat): 
    id(id), typeInstr(typeInstr), bbid(bbid), name(node_name), lat (lat) {    
    if (typeInstr==I_ADDSUB && node_name.find("197")!=string::npos)
      name="Compute";    
    if (typeInstr==I_ADDSUB && node_name.find("199")!=string::npos)
      name="Supply";
  }
  
  void addDependent(Node *dest, TEdge type);
  void eraseDependent(Node *dest, TEdge type);
  friend std::ostream &operator<<(std::ostream &os, Node &n);
};
class BasicBlock {
public:
  std::vector<Node*> inst;
  int id;
  unsigned int inst_count;
  unsigned int ld_count;
  unsigned int st_count;
  BasicBlock(int id): id(id), inst_count(0), ld_count(0), st_count(0) {}
  void addInst(Node* n);

};

class Graph {
public:
  std::map<int, Node *> nodes;
  std::map<int, BasicBlock*> bbs;
  ~Graph() { eraseAllNodes(); } 
  void addBasicBlock(int id);
  void addNode(int id, TInstr type, int bbid, std::string name, int lat);
  Node *getNode(int id);
  void eraseNode(Node *n);
  void eraseAllNodes();
  void addDependent(Node *src, Node *dest, TEdge type) ;
  void eraseDependent(Node *src, Node *dest, TEdge type);
  friend std::ostream &operator<<(std::ostream &os, Graph &g);
};


#endif
