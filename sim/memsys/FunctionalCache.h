#include <map>
#include <vector>
#include <set>
#include <bitset>
#include <math.h>
#include <iostream>
using namespace std;

struct CacheLine
{
  uint64_t addr;
  CacheLine* prev;
  CacheLine* next;
  bool dirty=false;
};

class CacheSet
{
public:
  CacheLine *entries;
  CacheLine *head;
  CacheLine *tail;
  std::vector<CacheLine*> freeEntries;
  std::map<uint64_t, CacheLine*> addr_map;
  CacheSet(int size)
  {
    head = new CacheLine;
    tail = new CacheLine;
    entries = new CacheLine[size];
    for(int i=0; i<size;i++)
    {
      freeEntries.push_back(&entries[i]);
    }
    head->prev = NULL;
    head->next = tail;
    tail->next = NULL;
    tail->prev = head;
  }
  ~CacheSet()
  {
    delete head;
    delete tail;
    delete [] entries;
  }
  bool access(uint64_t address, bool isLoad)
  {
    CacheLine *c = addr_map[address];
    if(c)
    {
      // Hit
      deleteNode(c);
      insertFront(c);
      if(!isLoad)
        c->dirty = true;
      return true;
    }
    else
    {
      return false;
    }
  }
  void insert(uint64_t address, int64_t *evict = NULL)
  {
    CacheLine *c = addr_map[address];    
    if(freeEntries.size() == 0)
    {
      // Evict
      c = tail->prev;
      deleteNode(c);
      addr_map.erase(c->addr);
    
      if(evict && c->dirty) {
        *evict = c->addr;
      }  
    }
    else
    {
      // Get from Free Entries
      c = freeEntries.front();
      freeEntries.erase(freeEntries.begin());
    }
    c->addr = address;
    c->dirty = false;
    addr_map[address] = c;
    insertFront(c);
  }
  
  void insertFront(CacheLine *c)
  {
    c->next = head->next;
    c->prev = head;
    head->next = c;
    c->next->prev = c;
  }
  void deleteNode(CacheLine *c)
  {
    c->prev->next = c->next;
    c->next->prev = c->prev;
  }
};


class FunctionalCache
{
public:
  int line_count;
  int set_count;
  int log_set_count;
  std::vector<CacheSet*> sets;
  FunctionalCache(int size, int assoc)
  {
    line_count = size * 1024 / 64;
    set_count = line_count / assoc;
    log_set_count = log2(set_count);
    for(int i=0; i<set_count; i++)
    {
      sets.push_back(new CacheSet(assoc));
    }
  } 
  uint64_t extract(int max, int min, uint64_t address) // inclusive
  {
      uint64_t maxmask = ((uint64_t)1 << (max+1))-1;
      uint64_t minmask = ((uint64_t)1 << (min))-1;
      uint64_t mask = maxmask - minmask;
      uint64_t val = address & mask;
      if(min > 0)
        val = val >> (min-1);
      return val;
  }

  bool access(uint64_t address, bool isLoad)
  {
    uint64_t setid = extract(log_set_count-1, 0, address);
    uint64_t tag = extract(58, log_set_count, address);
    CacheSet *c = sets.at(setid);
    bool res = c->access(tag, isLoad);
    return res;
  }
  void insert(uint64_t address, int64_t *evicted = NULL)
  {
    uint64_t setid = extract(log_set_count-1, 0, address);
    uint64_t tag = extract(58, log_set_count, address);
    CacheSet *c = sets.at(setid);
    int64_t evictedTag = -1;
    c->insert(tag, &evictedTag);
    if(evicted && evictedTag != -1)
      *evicted = evictedTag * set_count + setid;
  }
};
