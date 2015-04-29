
#ifndef __FenwickTree__

#define __FenwickTree__

template<class Data> class FenwickTree {
  private :
    unsigned sz;
    Data *tree;
  public :
    unsigned size(void) {
      return sz;
    }
    void update(unsigned idx, Data val) {
      if(idx == 0) {
        return;
      }
      while(idx <= sz) {
        tree[idx] += val;
        idx += idx & (-idx);
      }
    }
    Data query(unsigned idx) {
      Data Result = Data();
      if(idx > sz) {
        idx = sz;
      }
      while(idx > 0) {
        Result += tree[idx];
        idx -= idx & (-idx);
      }
      return Result;
    }
    FenwickTree(unsigned _sz) : sz(_sz) {
      tree = new Data[sz + 1];
      for(int i = 1;i <= sz;i++) {
        tree[i] = Data();
      }
    }
};

#endif
