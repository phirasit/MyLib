
#ifndef __SPLAY_TREE__

#define __SPLAY_TREE__

template<class Data> class SplayTree {
	
	public :

		class iterator {

			public :
				iterator *left, *right, *parent;
				int subtree_size;
				Data key;
				iterator(Data _key) : key(_key), subtree_size(1), left(NULL), right(NULL), parent(NULL) {}
		};

		iterator *root;
		int total_size;

		int get_subtree_size(iterator *it) {
			return it == NULL ? 0 : it->subtree_size;
		}

		void fix(iterator *it) {
			it->subtree_size = get_subtree_size(it->left) + 1 + get_subtree_size(it->right);
		}

		void rotate(iterator *it) {
			iterator *par = it->parent;
			iterator *par2 = par->parent;
			if(par2 != NULL) {
				(par2->left == par ? par2->left : par2->right) = it;
			}
			it->parent = par2;
			if(par->left == it) {
				par->left = it->right;
				if(par->left != NULL) par->left->parent = par;
				it->right = par;
				it->right->parent = it;
			}else {
				par->right = it->left;
				if(par->right != NULL) par->right->parent = par;
				it->left = par;
				it->left->parent = it;
			}

			fix(par);
			fix(it);
		}

		void splay(iterator *it) {
			fix(it);
			while(it->parent != NULL) {
				iterator *par = it->parent;
				iterator *par2 = it->parent;
				if(par2 == NULL) {
					rotate(it);
				}else if((par2->left != NULL and par2->left->right == it) or (par2->right != NULL and par2->right->left == it)) {
					rotate(it);
					rotate(it);
				}else {
					rotate(par);
					rotate(it);
				}
			}
			root = it;
		}

		iterator* merge(iterator *it1, iterator *it2) {
			if(it1 == NULL) return it2;
			if(it2 == NULL) return it1;
			while(it1->right != NULL) {
				it1 = it1->right;
			}
			it1->right = it2;
			it2->parent = it1;
			splay(it2);
			return it2;
		}

		void erase(iterator *it) {
			splay(it);
			iterator *it2 = it->left;
			iterator *it3 = it->right;
			if(it2 != NULL) it2->parent = NULL;
			if(it3 != NULL) it3->parent = NULL;
			root = merge(it2, it3);
			delete it;
			total_size--;
		}

		iterator* insert(Data new_data) {
			iterator *new_node = new iterator(new_data);
			if(root == NULL) {
				root = new_node;
				return new_node;
			}
			iterator *it = root;
			while(true) {
				if(new_node->key <= it->key) {
					if(it->left == NULL) {
						it->left = new_node;
						splay(new_node);
						break;
					}else {
						it = it->left;
					}
				}else {
					if(it->right == NULL) {
						it->right = new_node;
						splay(new_node);
						break;
					}else {
						it = it->right;
					}
				}
			}
			total_size++;
			return new_node;
		}

		Data Kth(int k) {
			if(1 <= k and k <= total_size) {
				iterator *it = root;
				while(k != get_subtree_size(it->left) + 1) {
					if(k <= get_subtree_size(it->left)) {
						it = it->left;
					}else {
						k -= get_subtree_size(it->left) + 1;
						it = it->right;
					}
				}
				splay(it);
				return it->key;
			}
		}

		SplayTree(void) : total_size(0), root(NULL) {}	
};

#endif