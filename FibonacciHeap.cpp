
#ifndef __FIBONACCI_HEAP__

#define __FIBONACCI_HEAP__

#include <vector>
#include <stdio.h>
#include <assert.h>

using namespace std;

template<class Data> class FIBOheap {
	
	public :
		
		class iterator {

			public :
	
				bool mark;
				int id, deg;
				Data key;
				iterator *parent, *left, *right, *left_child;
	
				iterator(Data k) : key(k), deg(0), parent(NULL), mark(false),
					left(NULL), left_child(NULL), right(NULL) {}
				~iterator(void) {}
		};

		int total_size;
		iterator *min_node, *root;

		bool empty(void) {
			return min_node == NULL;
		}

		int size(void) {
			return total_size;
		}

		void insert_root(iterator *new_node) {

			if(root == NULL) {
				root = new_node;
				root->left = root->right = root;
			}else {
				new_node->right = root->right;
				new_node->left = root;
				root->right = new_node;
			}

			if(min_node == NULL or new_node->key < min_node->key) {
				min_node = new_node;
			}
		}

		iterator* push(Data new_data) {
			iterator *new_node = new iterator(new_data);
			insert_root(new_node);
			return new_node;
		}

		void cut(iterator *current) {
			if(current->parent == NULL) return;
			iterator *ancestor = current->parent;
			ancestor->mark = false;
			if(current->left == NULL) {
				ancestor->left_child = current->right;
			}else {
				current->left->right = current->right;
			}
			if(current->right != NULL) {
				current->right->left = current->left;
			}
			current->left = current->right = current->parent = NULL;
			if(ancestor->parent != NULL and ancestor->mark) {
				cut(ancestor);
			}else {
				ancestor->mark = true;
			}
		}

		void decreasekey(iterator *current, Data new_key) {

			current->key = new_key;
			iterator *ancestor = current->parent;
			
			if(ancestor == NULL or !(current->key < ancestor->key)) {
				// heap constraints is not violate
				return;
			}

			cut(current);
		}

		void Consolidate(void) {


			vector<iterator*> new_root;
			
			bool first = true; 
			
			for(iterator *u = root; u != root or first;) {

				iterator *nxt = u->right;

				assert(u != NULL);

				first = false;

				if(u == min_node) {
					u = nxt;
					continue;
				}

				if(new_root.size() <= u->deg) {
					new_root.resize(u->deg + 1, NULL);
				}
			
				while(new_root[u->deg] != NULL) {
			
					iterator *v = new_root[u->deg];

					if(v->key < u->key) {
						swap(v, u);
					}

					v->parent = u;
					v->left = NULL;
					v->right = u->left_child;
					if(v->right != NULL) v->right->left = v;
					u->left_child = v;
					new_root[u->deg] = NULL;
			
					u->deg++;
			
					if(new_root.size() <= u->deg) {
						new_root.resize(u->deg + 1, NULL);
					}
				} 

				new_root[u->deg] = u;
			
				u = nxt;
			}

			root = NULL;
			delete min_node;
			min_node = NULL;

			for(iterator *u : new_root) {
				if(u != NULL) {
					insert_root(u);
				}
			}

		}

		void pop(void) {
			
			if(empty()) {
				return;
			}
				
			for(iterator *v = min_node->left_child;v != NULL;) {
				iterator *nxt = v->right;
				v->parent = v->left = v->right = NULL;
				v->mark = false;
				insert_root(v);
				v = nxt;
			}
			
			min_node->deg = 0;
			min_node->left_child = NULL;

			Consolidate();
			total_size--;
		}

		Data top(void) {
			if(not empty()) {
				return min_node->key;
			}
		}

		FIBOheap(void) : min_node(NULL), root(NULL), total_size(0) {}
		~FIBOheap(void) {}
};

#endif