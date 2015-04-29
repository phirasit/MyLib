
#ifndef __SEGMENT_TREE__

#define __SEGMENT_TREE__

#include <stdlib.h>

template<class Data> class SegmentTree {
  private :
    struct node {
      unsigned l, r;
      Data data, lazy;
      node *lf, *rg;
      node(unsigned _l, unsigned _r) : l(_l), r(_r), lf(NULL), rg(NULL) {}
      ~node(void) {
        if(lf != NULL) {
          delete lf;
        }
        if(rg != NULL) {
          delete rg;
        }
      }
    };

    node* init(unsigned, unsigned);
    void propagate(node*);
    Data range_query(node*, unsigned, unsigned);
    void update_range(node*, unsigned, unsigned, Data);

  public :
    node *head;
    unsigned sz;
    Data flag;

    SegmentTree(unsigned _sz, Data fg) : sz(_sz), flag(fg) {
      head = init(1, sz);
    }
    ~SegmentTree(void) {
      delete head;
    }
    Data query(unsigned l) {
      return range_query(head, l, l);
    }
    Data query(unsigned l, unsigned r) {
      return range_query(head, l, r);
    }
    void update(unsigned l, Data val) {
      return update_range(head, l, l, val);
    }
    void update(unsigned l, unsigned r, Data val) {
      return update_range(head, l, r, val);
    }
};

SegmentTree::node* SegmentTree::init(unsigned l, unsigned r) {
  SegmentTree::node* p = new SegmentTree::node(l, r);
  if(l < r) {
    unsigned mid = (l + r) >> 1;
    p->lf = init(l, mid);
    p->rg = init(mid+1, r);
  }
  p->data = Segmentree::Data();
  p->lazy = SegmentTree::flag;
  return p;
}
void SegmentTree<Data>::propagate(SegmentTree::node* p) {
  if(p->lazy != flag) {
    SegmentTree::update(p->lf, p->l, p->r, p->lazy);
    SegmentTree::update(p->rg, p->l, p->r, p->lazy);
    p->data = p->lf->data + p->rg->data;
    p->lazy = flag;
  }
}
void SegmentTree::update_range(SegmentTree::node* p, unsigned l, unsigned r, SegmentTree::Data D) {
  if(l <= p->l && p->r <= r) {
    p->data += (p->r - p->l + 1) * D;
    p->lazy += D;
    return;
  }
  if(r < p->l || p->r < l) {
    return;
  }
  SegmentTree::propagate(p);
  SegmentTree::update(p->lf, l, r, D);
  SegmentTree::update(p->rg, l, r, D);
  p->data = p->lf->data + p->rg->data;
}
SegmentTree::Data SegmentTree::range_query(SegmentTree::node* p, unsigned l, unsigned r) {
  if(l <= p->l && p->r <= r) {
    return p->data;
  }
  if(p->r < l || r < p->l) {
    return SegmentTree::flag;
  }
  SegmentTree::propagate(p);
  return range_query(p->lf, l, r) + range_query(p->rg, l, r);
}

#endif
