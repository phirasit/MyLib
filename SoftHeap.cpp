/*
Soft Heap
implemented by Phirasit Charoenchitseriwong
Idea from: The Soft Heap, Bernard Chazelle [CHA'99]

This heap performs as good as fibonacci heap
with some better advantages. The inventor of this heap claimed that
this heap can be easily implemented and also attached some source codes
in the paper result to this implementation.

Overall, the running time of each operations is O(1) except 
extract-min which can be perform in O(log 1/eps).

However, Having a better running time has to exchange
with losing some properties. Some numbers (no more than eps * n) 
are called "corrupted", which their values are changed(increased).
According to the paper, it proves the optimality of this heap.
Therefore, there won't be any better heap.

If eps = 2 * n, there is no corrupted number, and
the performance is as same as fibonacci heap's. 

There are lots of applications. For instance, It can be use to 
achieve new running time of finding MST, O(m alpha(n, m)).
This heap stimulate running time of other problems.

The author, personally, believe that this code is bug-free.
*/

#ifndef SOFT_HEAP

#define SOFT_HEAP

class SoftHeap {
  private : 
    const int inf = -1u/2;
    struct data {
      int key;
      data *next;
      data(int _key) : key(_key), next(NULL) {}
    };
    struct node {
      int ckey, rank;
      node *next, *child;
      data *first, *last;
    };
    struct head {
      head *queue;
      head *next, *prev, *suffix_min;
      int rank;
    }
    double eps;
    head *header, *tail;
  public :
    int extract_min(void) {
      node *tmp;
    }
    int pop(void) {
      return extract_min();
    }
    void fix_minlist(head *h) {
      head *tmpmin = (h->next == tail ? h : h->next->suffix_min);
      while(h != header) {
        if(h->queue->ckey < tmpmin->queue->ckey) {
          tmpmin = h;
        }
        h->suffix_min = tmpmin;
        h = h->prev;
      }
    }
    void meld(node *q) {
      head *h, *prevhead, *tohead = header->next;
      node *top, *bottom;
      while(q->rank > tohead->rank) {
        tohead = tohead->next;
      }
      prevhead = tohead->prev;
      while(q->rank == tohead->rank) {
        if(tohead->queue->ckey > q->ckey) {
          top = q;
          bottom = tohead->queue;
        }else { 
          top = tohead->queue;
          bottom = q;
        }
        q = new node();
        q->ckey = top->ckey;
        q->rank = top->rank + 1;
        q->child = bottom;
        q->next = top;
        q->first = top->first;
        q->last = top->last;
        tohead = tohead->next;
      }
      if(prevhead == tohead->prev) {
        h = new_head();
      }else {
        h = prevhead->next;
      }
      h->prev = prevhead;
      h->next = tohead;
      prevhead->next = tohead->prev = h;
      fix_minlist(h);
    }
    void insert(int new_key) { 
      node *p = new node();
      p->rank = 0, p->ckey = new_key;
      p->first = p->last = new data(new_key);
      meld(p);
    }
    
    SoftHeap(double _eps = 0.5) : eps(_eps) {
      header = new head();
      tail = new head();
      tail->rank = inf;
      header->next = tail;
      tail->prev = header;
    }
};

#endif
