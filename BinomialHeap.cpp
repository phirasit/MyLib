
#ifndef BINOMIAL_HEAP

#define BINOMIAL_HEAP

#include <vector>

using namespace std;

template<class Data> class BinomialHeap {
	
	public :

		class iterator {

			public :
	
				int id, deg;
				Data key;
				iterator *parent, *left, *right, *left_child;
	
				iterator(Data k) : key(k), deg(0), parent(NULL), 
					left(NULL), left_child(NULL), right(NULL) {}
				~iterator(void) {}
		};

		int total_size;
		vector<iterator*> root;
		int min_idx;

		bool empty(void) {
			return min_idx == -1;
		}

		int size(void) {
			return total_size;
		}

		void insert_root(iterator *new_node) {

			if(new_node->deg >= root.size()) {
				root.resize(new_node->deg+1, NULL);
			}

			while(root[new_node->deg] != NULL) {
				
				iterator *v = root[new_node->deg];
				if(v->key < new_node->key) {
					swap(new_node, v);	
				}
				v->parent = new_node;
				v->left = NULL;
				v->right = new_node->left_child;
				if(v->right != NULL) v->right->left = v;
				new_node->left_child = v;
				root[new_node->deg++] = NULL;

				if(new_node->deg >= root.size()) {
					root.resize(new_node->deg+1, NULL);
				}
			}
			root[new_node->deg] = new_node;
			if(min_idx == -1 or root[min_idx] == NULL or root[new_node->deg]->key < root[min_idx]->key) {
				min_idx = new_node->deg;
			}

		}

		iterator* push(Data new_data) {
			iterator *new_node = new iterator(new_data);
			insert_root(new_node);
			total_size++;
			return new_node;
		}

		void decreasekey(iterator *current, Data new_key) {
			current->key = new_key;
			iterator *ancestor = current->parent;
			while(ancestor != NULL and current->key < ancestor->key) {
				swap(current->key, ancestor->key);
				current = current->parent;
				ancestor = ancestor->parent;
			}
		}

		void pop(void) {
			
			if(empty()) {
				return;
			}

			iterator *v = root[min_idx]->left_child;

			root[min_idx] = NULL;
			delete root[min_idx];
			root[min_idx] = NULL;

			min_idx = -1;

			while(v != NULL) {
				iterator *nxt = v->right;
				v->parent = v->left = v->right = NULL;
				insert_root(v);
				v = nxt;
			}
			
			int last = 0;
			min_idx = -1;
			for(int i = 0;i < root.size();i++) {
				if(root[i] != NULL) {
					if(min_idx == -1 or root[i]->key < root[min_idx]->key) {
						min_idx = i;
					}
					last = i;
				}
			}

			root.resize(last+1, NULL);
			total_size--;
		}

		Data top(void) {
			if(not empty()) {
				return root[min_idx]->key;
			}
		}

		void meld(BinomialHeap<Data> H2) {
			if(empty()) return H2;
			if(H2.empty()) return *this;
			for(int i = 0;i < H2->root.size();i++) {
				if(H2->root[i] != NULL) {
					insert_root(H2->root[i]);
				}
			}
		}

		BinomialHeap(void) : min_idx(-1), total_size(0) {}
};

#endif