/*
Van embe boas tree
Implemented by : Phirasit Charoenchitseriwong

This tree performs better than normal BSTs
with running time of O(log log U) instead of
O(log N). This result leads to a new approach
for dealing with dictionary.

VEB consists of exploiting bit and also resursive tree.
This tree is not an improved one so the memory require
is O(U log log U) however this can improve easily.

This code is written with C++

Everyone is free to use this code.
The author personally believe that the code is bug-free.
*/

#ifndef __VAN_EMBE_BOAS__

#define __VAN_EMBE_BOAS__

#include <vector>

using namespace std;

class VEB {
  private :
    const int none = -1;
    class VEB_NODE {
      vector<VEB_NODE*> next_tree;
      VEB_NODE *aux;
      int min_val, max_val, bit_len, half_len;
      VEB_NODE(int _len) : min_val(none), max_val(none), aux(NULL), bit_len(_len), half_len(_len/2) {
        next_tree.resize(1 << (bit_len - half_len), NULL);
      }
    };
    VEB_NODE *head;
    int bit_len;
    int high(int val, int p) {
      return val >> p;
    }
    int low(int val, int p) {
      return val & ((1 << p) - 1);
    }
  public :
    int error(void) {
      return none;
    }
    int empty(node *p = head) {
      return p->min == none;
    }
    int max(node *p = head) {
      return p->max_val;
    }
    int min(node *p = head) {
      return p->min_val;
    }
    int find(VEB_NODE *p = head, int val) {
      if(val > (1 << p->bit_len)) return 0;
      if(val == p->min_val) return 1;
      if(val == p->max_val) return 1;
      if(p->bit_len == 1) {
        return 0;
      }
      int hi = high(val, p->half_len), lo = low(val, p->half_len);
      if(p->next_tree[hi] == NULL) return 0;
      return find(p->next_tree[hi], lo);
    }
    void erase(VEB_NODE *p = head, int val) {
      if(val > (1 << p->bit_len)) return;
      if(val == p->min_val) {
        int hi = this->min(p->aux);
        if(hi == none) {
          p->min_val = p->max_val = none;
          return;
        }
        p->min_val = (hi << p->half_len) | this->min(p->next_tree[hi]);
        val = p->min_val;
      }
      int hi = high(val), lo = low(val);
      if(p->next_tree[hi] != NULL and p->next_tree[hi]->min_val != none) {
        erase(p->next_tree[hi], lo);
        if(empty(p->next_tree[hi])) {
          erase(p->aux, hi);
        }
      }
    }
    int predessor(node *p = head, int val) {
    }
    int successor(node *p = head, int val) {
    }
    void insert(VEB_NODE *p = head, int val) {
      val &= (1 << p->bit_len) - 1; // this should be ineffective
      if(p->min_val == none) { // tree is empty
        p->min_val = val;
        return;
      }
      if(p->min_val == val) { // key is already existed
        return;
      }
      if(val < p->min_val) {
        swap(val, p->min_val);
      }
      if(val > p->max_val or p->max_val == none) {
        p->max_val = val;
      }
      if(p->bit_len == 1) {
        return;
      }
      int hi = high(val, p->half_len), lo = low(val, p->half_len);
      if(p->aux == NULL) p->aux = new VEB_NODE(p->bit_len - p->half_len);
      if(p->next_tree[hi] == NULL) p->next_tree[hi] = new VEB_NODE(p->half_len);
      if(p->next_tree[hi]->min_val == none) insert(p->aux, hi);
      insert(p->next_tree[hi], lo);
    }
    void VEB(int _len = 20) : bit_len(_len), head(new VEB_NODE(_len)) {}
};

#endif
