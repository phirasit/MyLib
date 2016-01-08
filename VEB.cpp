/*
Van Embe Boas Tree
Implemented by : Phirasit Charoenchitseriwong

This tree performs better than standard BSTs
with running time of O(log log U) instead of
O(log N). This result leads to a new approach
of dealing with dictionary.

VEB consists of exploiting bitwise number and resursive tree.
This tree requires O(N log log U) memory.

This code is written in C++11. 
Everyone is free to use this code.
The author personally believe that the code is bug-free.
*/

#ifndef __VAN_EMBE_BOAS__

#define __VAN_EMBE_BOAS__

#include <algorithm>
#include <vector>

using namespace std;

class VEB {

  // protected :
  public :

    static const int error_code = -1;

    class VEB_NODE {

      private :

        VEB_NODE *aux;
        vector<VEB_NODE*> cluster;
        int len, half_len;
        int min, max;

      public :

        static const int none = -1;

        // list of functions in each node
        bool empty(void);
        int high(int);
        int low(int);
        int find_min(void);
        int find_max(void);

        VEB_NODE(int);
        bool find(int);
        int successor(int);
        int predecessor(int);
        void insert(int);
        void erase(int);
    };

    VEB_NODE *head;
    int bit_len;

  public :

    // list of functions that are accessible
    VEB(int);
    ~VEB(void);
    
    int size(void);
    bool empty(void);
    int error(void);
    int min(void);
    int max(void);

    bool find(int);
    int successor(int);
    int predecessor(int);
    void insert(int);
    void erase(int);

};

VEB::VEB_NODE::VEB_NODE(int _len) {
  len = _len;
  half_len = (len >> 1);
  if(len > 1) {
    aux = new VEB_NODE(len - half_len);
    cluster.resize(1 << (len - half_len), NULL);
  }
  min = max = none;
}

// these are accessory functions
bool VEB::VEB_NODE::empty(void) {
  return min == none;
}
int VEB::VEB_NODE::high(int val) {
  return val >> half_len;
}
int VEB::VEB_NODE::low(int val) {
  return val & ((1 << half_len)-1);
}
int VEB::VEB_NODE::find_min(void) {
  return min;
}
int VEB::VEB_NODE::find_max(void) {
  if(max == none) return min;
  return max;
}

// these are the most important parts of this tree 
// and also the most complicated ones

bool VEB::VEB_NODE::find(int val) {
  if(val < 0 or val >= (1 << len)) return false;
  if(min == val or (max != none and val == max)) return true;
  if(len == 1 or aux == NULL or max == none) return false;
  int cluster_id = high(val);
  if(cluster[cluster_id] == NULL) return false;
  return cluster[cluster_id]->find(low(val));
}

int VEB::VEB_NODE::successor(int val) {
  if(empty() or (max != none and val >= max)) return none;
  if(val < min) return min;
  if(len == 1) return max;
  int cluster_id = high(val);
  if(cluster[cluster_id] == NULL) {
    int new_cluster_id = aux->successor(cluster_id);
    if(new_cluster_id == none) return none;
    return (new_cluster_id << half_len) ^ cluster[new_cluster_id]->find_min();
  }else {
    int ans = cluster[cluster_id]->successor(low(val));
    if(ans == none) {
      int new_cluster_id = aux->successor(cluster_id);
      if(new_cluster_id == none) return none;
      return (new_cluster_id << half_len) ^ cluster[new_cluster_id]->find_min();    
    }else {
      return (cluster_id << half_len) ^ ans;
    }
  }
}

int VEB::VEB_NODE::predecessor(int val) {
  if(empty() or val <= min) return none;
  if(aux == NULL or max == none) return min;
  if(len == 1) return min;
  int cluster_id = high(val);
  if(cluster[cluster_id] == NULL) {
    int new_cluster_id = aux->predecessor(cluster_id);
    if(new_cluster_id == none) return min;
    return (new_cluster_id << half_len) ^ cluster[new_cluster_id]->find_max();
  }else {
    int ans = cluster[cluster_id]->predecessor(low(val));
    if(ans == none) {
      int new_cluster_id = aux->predecessor(cluster_id);
      if(new_cluster_id == none) return min;
      return (new_cluster_id << half_len) ^ cluster[new_cluster_id]->find_max();
    }else {
      return (cluster_id << half_len) ^ ans;
    }
  }
}

void VEB::VEB_NODE::insert(int val) {

  // check for out of bound numbers
  if(val < 0 or val >= (1 << len)) return;

  if(min == none) {
    min = val;
    return;
  }

  if(val == min) return;

  if(val < min) {
    swap(min, val);
  }

  if(val > max or max == none) {
    max = val;
  }

  if(len == 1) {
    return;
  }

  if(aux == NULL) {
    aux = new VEB_NODE(len - half_len);
  }

  int cluster_id = high(val);
  if(cluster[cluster_id] == NULL) {
    cluster[cluster_id] = new VEB_NODE(half_len);
  }
  if(cluster[cluster_id]->empty()) {
    aux->insert(cluster_id);
  }
  cluster[cluster_id]->insert(low(val));

}

void VEB::VEB_NODE::erase(int val) {
  
  // check for out of bound numbers
  if(val < 0 or val >= (1 << len)) return;

  // check wheather tree is empty
  if(empty()) return;

  if(len == 1) {
    if(val == max) {
      max = none;
    }else if(val == min) {
      min = max;
      max = none;
    }
    return;
  }

  if(val == min) {
    if(aux == NULL or aux->empty() or max == none) {
      min = none;
    }else {
      int cluster_id = aux->find_min();
      min = (cluster_id << half_len) ^ cluster[cluster_id]->find_min();
      cluster[high(min)]->erase(low(min));
      if(cluster[cluster_id]->empty()) {
        aux->erase(cluster_id);
      }
      if(min == max) max = none;
    }
    return;
  }

  if(aux == NULL or aux->empty() or max == none) return;

  int cluster_id = high(val);
  if(cluster[cluster_id] == NULL) return;
  cluster[cluster_id]->erase(low(val));
  if(cluster[cluster_id]->empty()) {
    aux->erase(cluster_id);
  }  
  if(val == max and max != none) {
    cluster_id = aux->find_max();
    if(cluster_id == none) {
      max = none;
    }else {
      max = (cluster_id << half_len) ^ cluster[cluster_id]->find_max();
    }
  }
}


// these functions are publicly accessable
VEB::VEB(int _len) {
  bit_len = _len;
  head = new VEB_NODE(bit_len);
}
VEB::~VEB(void) {
  delete head;
}
int VEB::size(void) {
  return 1 << bit_len;
}
bool VEB::empty(void) {
  return head->empty();
}
int VEB::error(void) {
  return error_code;
}
int VEB::min(void) {
  if(head->find_min() == head->none) return error();
  return head->find_min();
}
int VEB::max(void) {
  if(head->find_max() == head->none) return error();
  return head->find_max();
}
bool VEB::find(int val) {
  return head->find(val);
}
int VEB::successor(int val) {
  if(empty()) return error();
  int ans = head->successor(val);
  if(ans == VEB_NODE::none) return error();
  return ans;
}
int VEB::predecessor(int val) {
  if(empty()) return error();
  int ans = head->predecessor(val);
  if(ans == VEB_NODE::none) return error();
  return ans;
}
void VEB::insert(int val) {
  head->insert(val);
}
void VEB::erase(int val) {
  head->erase(val);
}

#endif
