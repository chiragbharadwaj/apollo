#ifndef LSQ_H
#define LSQ_H
#include "../common.h"
using namespace std;

class DynamicNode;
class DynamicNodePointerCompare;

class LoadStoreQ {
public:
  deque<DynamicNode*> lq;
  deque<DynamicNode*> sq;
  unordered_map<uint64_t, set<DynamicNode*, DynamicNodePointerCompare>> lm;
  unordered_map<uint64_t, set<DynamicNode*, DynamicNodePointerCompare>> sm;
  set<DynamicNode*, DynamicNodePointerCompare> us;
  set<DynamicNode*, DynamicNodePointerCompare> ul;
  set<DynamicNode*,DynamicNodePointerCompare> unresolved_st_set;
  set<DynamicNode*,DynamicNodePointerCompare> unresolved_ld_set;

  int size;
  bool set_processed = false;

  LoadStoreQ();
  void resolveAddress(DynamicNode *d);
  void insert(DynamicNode *d);
  bool checkSize(int num_ld, int num_st);
  bool check_unresolved_load(DynamicNode *in);
  bool check_unresolved_store(DynamicNode *in);
  int check_load_issue(DynamicNode *in, bool speculation_enabled);
  bool check_store_issue(DynamicNode *in);
  int check_forwarding (DynamicNode* in);

  std::vector<DynamicNode*> check_speculation (DynamicNode* in);
};
#endif
