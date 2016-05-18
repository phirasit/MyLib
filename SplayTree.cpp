/*
==========
SPLAY TREE
==========

Implemented by Phirasit Charoenchitseriwong

This code is written in C++11.
The author believes that this code is bug-free.

Splay tree is a BST with amortized running time . However, due to caching like method, 
splay tree's performance is very efficient in practical with rarely slow running time
like a normal amoritzed data structure. 

*/

#include <memory>
#include <functional>
#include <cassert>

#ifndef __SPLAY_TREE__
#define __SPLAY_TREE__

// type of splay trees
enum SplayTreeType { 
	__SPLAY_TREE_NORMAL__ , 
	__SPLAY_TREE_BST__ , 
	__SPLAY_TREE_LAZY_PROPAGATION__ , 
	__SPLAY_TREE_BST_LAZY_PROPAGATION__ 
};

template < class DataType > class SplayTree {

	// list of structures
	private:

		// tree structure
		class node {

			public:
				
				// pointer to other iterators
				std :: shared_ptr < node > left, right, parent;

				// pointer for iteration
				std :: shared_ptr < node > succ , pred;

				// key of the iterator
				DataType key;

				// size of subtree
				size_t size;

				// constructor
					// without key
					node ( void );

					// with key 
					node ( DataType );
		};

	public:

		// structure of each node in splay tree

		class iterator {

			public:

				// pointer to node
				std :: shared_ptr < node > ptr;

				// constructor
				iterator ( std :: shared_ptr < node > _ptr );

				// access the key of the iterator
				DataType& operator * ( void );

				// compare iterator (wheather it is the same)
				bool operator == ( iterator );
				bool operator != ( iterator );

				// increment function
				void operator ++ ( void );

				// decrement function
				void operator -- ( void );

		};

	private:

		// comparator function
		std::function< bool ( DataType& , DataType& ) > comparator;

		// combinator function
		std::function < DataType ( DataType&, DataType& ) > combinator;

		// null node 
		node _null_node;

		// begin() iterator
		std :: shared_ptr < node > _begin;

		// end() iterator
		std :: shared_ptr < node > _end;

		// head of the tree
		std :: shared_ptr < node > _head;

		// type of splay tree
		size_t _type;

	// list of headers

	private:

		// get the size of subtree
		size_t get_size ( std :: shared_ptr < node > );

		// fix function (use with lazy propagation)
		void fix ( iterator );

		// rotate function (use with splay) 
		void rotate ( iterator );

	public:

		// normal Splay Tree
		SplayTree ( void );

		// Splay Tree with a comparator
		SplayTree ( std::function < bool ( DataType& , DataType& ) > );

		// Splay Tree with lazy propagation
		// this functions is given wtih a combine function and a null node
		SplayTree ( std::function < DataType ( DataType&, DataType& ) > , DataType );

		// Splay Tree with a comparator and a combinator
		SplayTree ( std::function < bool ( DataType& , DataType& ) > , std::function < DataType ( DataType&, DataType& ) > , DataType );

		// splay the current node
		// this should not be used manually , but can in some special cases
		void splay ( iterator );

		// find the nth element
		iterator nth ( size_t );
		iterator operator [] ( size_t );

		// search for a key
		iterator find ( DataType );

		// find the current position of the given iterator
		size_t position ( iterator );


		// split the tree into two trees

			// with given index
			SplayTree split ( size_t );

			// with given iterator
			SplayTree split ( iterator );

		// reverse some part of the tree
			
			// the whole tree
			void reverse ( void );

			// with given range
			void reverse ( size_t , size_t );

			// with given iterator
			void reverse ( iterator , iterator );

		// insert a key 

			// before end() iterator
			iterator insert ( DataType );

			// before a given iterator
			iterator insert ( iterator , DataType );

			// at the given position
			iterator insert ( size_t , DataType );

			// insert a tree at its end
			iterator insert ( SplayTree );

			// insert a tree with given position
			iterator insert ( iterator , SplayTree );

		// remove a key

			// remove the matched key
			// perform like multi-set
			void remove ( DataType );

			// with a given iterator
			void remove ( iterator );

		// search for a key

			// perform like std :: lower_bound
			iterator lower_bound ( DataType );

			// perform like std :: upper_bound
			iterator upper_bound ( DataType );

		// get number of nodes in the current tree
		size_t size ( void );

		// check wheather tree is empty
		bool empty ( void );

		// get begin() iterator
		iterator begin ( void );

		// get end() iterator
		iterator end ( void );

};

// -- begin node functions

template < class DataType >
SplayTree < DataType > :: node :: node ( void ) : size ( 1u ) {}

template < class DataType >
SplayTree < DataType > :: node :: node ( DataType _key ) : size ( 1u ) , key ( _key ) {}

// -- end node functions

// -- begin iterator functions

// iterator constructor
template < class DataType >
SplayTree < DataType > :: iterator :: iterator ( std :: shared_ptr < node > _ptr ) : ptr ( _ptr ) {}

// get value of iterator
template < class DataType >
DataType& SplayTree < DataType > :: iterator :: operator * ( void ) { return ptr->key; }

// compare iterator
template < class DataType >
bool SplayTree < DataType > :: iterator :: operator == ( iterator iter ) { return ptr == iter->ptr; }
template < class DataType >
bool SplayTree < DataType > :: iterator :: operator != ( iterator iter ) { return ptr != iter->ptr; }

// increment function
template < class DataType >
void SplayTree < DataType > :: iterator :: operator ++ ( void ) {
	assert ( ( ptr != end() , " cannot increase " ) );
	ptr = ptr->succ;
}

// decrement function
template < class DataType >
void SplayTree < DataType > :: iterator :: operator -- ( void ) {
	assert ( ( ptr != begin() , " cannot decrease " ) );
	ptr = ptr->pred;
}

// -- end iterator

// -- begin splay tree constructors

// with no comparator
template < class DataType > 
SplayTree < DataType > :: SplayTree ( void ) 
	: _head ( new node ) , _begin ( _head ) , _end ( _head ) , _type ( __SPLAY_TREE_NORMAL__ ) {}

// with comparator
template < class DataType > 
SplayTree < DataType > :: SplayTree ( std::function < bool ( DataType& , DataType& ) > _comparator) 
	: _head ( new node ) , _begin ( _head ) , _end ( _head ) , comparator ( _comparator ) , _type ( __SPLAY_TREE_BST__ ) {}

// with combinator
template < class DataType > 
SplayTree < DataType > :: SplayTree ( std::function < DataType ( DataType& , DataType& ) > _combinator , DataType __null_node ) 
	: _head ( new node ) , _begin ( _head ) , _end ( _head ) , combinator ( _combinator ) , _null_node ( __null_node ) , _type ( __SPLAY_TREE_LAZY_PROPAGATION__ ) {}

// with both comparator and combinator
template < class DataType > 
SplayTree < DataType > :: SplayTree ( std::function < bool ( DataType& , DataType& ) > _comparator , std::function < DataType ( DataType& , DataType& ) > _combinator , DataType __null_node)
	: _head ( new node ) , _begin ( _head ) , _end ( _head ) , comparator ( _comparator ) , combinator ( _combinator ) , _type ( __SPLAY_TREE_BST_LAZY_PROPAGATION__ ) {}

// -- end splay tree constructors

// -- begin helper functions of splay tree

// fix operation ( use only with lazy propagation tag )
template < class DataType > 
void SplayTree < DataType > :: fix ( iterator it ) {

}

// rotate operation ( helper function of splay )
template < class DataType > 
void SplayTree < DataType > :: rotate ( iterator it ) {

	assert ( ( it.ptr->parent , " error rotate root " ) );

	std :: shared_ptr < node > par ( it.ptr->parent );

	it.ptr->parent = par->parent;

	if( par->parent ) {
		( par->parent->left == par ? par->parent->left : par->parent->right ) = it.ptr;
	}

	if( par->left == it.ptr ) {

		par->left = it.ptr->right;

		if( par->left ) {
			par->left->parent = par;
		}

		it.ptr->right = par;
		it.ptr->right->parent = it.ptr;

	} else {

		par->right = it.ptr->left;

		if( par->right ) {
			par->right->parent = par;
		}

		it.ptr->left = par;
		it.ptr->left->parent = it.ptr;

	}

	if ( _type == __SPLAY_TREE_LAZY_PROPAGATION__ or _type == __SPLAY_TREE_BST_LAZY_PROPAGATION__ ) {
		fix ( iterator ( par ) );
		fix ( it.ptr );
	}

}

// splay operation ( the heart of this data structure )
template < class DataType > 
void SplayTree < DataType > :: splay ( iterator it ) {
	
	while ( it.ptr->parent ) { // not root

		std :: shared_ptr < node > par = it.ptr->parent;

		if( par->parent == nullptr ) {

			rotate ( it );

		} else if( ( par->parent->left == par and par->right == it.ptr ) or ( par->parent->right == par and par->left == it.ptr ) ) {
			
			rotate ( it );
			rotate ( it );
		
		} else {
			
			rotate ( iterator ( par ) );
			rotate ( it );
		
		}

	}

	_head = it.ptr;

}

// return the size of subtree
template < class DataType >
size_t SplayTree < DataType > :: get_size ( std :: shared_ptr < node > ptr ) {
	return ( ptr == nullptr ? 0 : ptr->size );
}

// get the nth element of splay tree
template < class DataType >
typename SplayTree < DataType > :: iterator SplayTree < DataType > :: nth ( size_t index ) {

	assert ( ( 0 <= index and index <= size() , " index out of bound " ) );

	iterator it = _head;

	while ( index != get_size ( it.ptr->left ) + 1u ) {

		if ( get_size ( it.ptr->left ) + 1u < index ) {
			it.ptr = it.ptr->left;
		} else {
			index = index - ( get_size ( it.ptr->left ) + 1u );
			it.ptr = it.ptr->right;
		}

	}

	splay( it );
	
	return it;
}

// similar to nth element
template < class DataType >
typename SplayTree < DataType > :: iterator SplayTree < DataType > :: operator [] ( size_t index ) {
	return nth ( index );
}

// find the iterator with the same key
template < class DataType >
typename SplayTree < DataType > :: iterator SplayTree < DataType > :: find ( DataType key ) {

	if ( _type == __SPLAY_TREE_BST__ or _type == __SPLAY_TREE_BST_LAZY_PROPAGATION__ ) {  // with bst tag : tree is able to search in O ( log n ) amortized

		iterator &it = *_head;

		while ( true ) {

			bool isLess = comparator ( it.ptr->key , key );

			if ( isLess ) {

				if ( it.ptr->left == nullptr ) {
					splay ( it );
					return end();
				}
				it = it.ptr->left;
			} else {
	
				bool isMore = comparator ( key , it.ptr->key );
				
				if ( isMore ) {
					if ( it.ptr->right == nullptr ) {
						splay ( it );
						return end();
					}
					it = it.ptr->right;
				} else {
					splay ( it );
					return it;
				}
			}
		}

	} else {

		for ( auto it = begin() ; it != end() ; it++ ) {
			if ( (not comparator ( it.ptr->key , key ) ) and (not comparator ( key , it.ptr->key ) ) ) {
				splay ( it );
				return it;
			}
		}

	}
}

// get position of iterator
template < class DataType > 
size_t SplayTree < DataType > :: position ( iterator it ) {
	splay ( it );
	return get_size ( it->left ) + 1u; 
}

// lower bound function
template < class DataType >
typename SplayTree < DataType > :: iterator SplayTree < DataType > :: lower_bound ( DataType key ) {

	assert ( ( _type == __SPLAY_TREE_BST__ or _type == __SPLAY_TREE_BST_LAZY_PROPAGATION__ , " SplayTree is not a BST ") );

	std :: shared_ptr < node > ptr ( _head );

	while ( true ) {

		if ( ptr != end() and comparator ( ptr->key , key ) ) {

			if ( ptr->right == nullptr ) {
				splay ( ptr );
				return iterator ( ptr->succ );
			}
			
			ptr = ptr->right;

		} else { 

			if ( ptr->lett == nullptr ) {
				splay ( ptr );
				if ( ptr != begin() and comparator ( key , ptr->pred->key )) {
					return iterator ( ptr->pred );
				} else {
					return iterator ( ptr );
				}
			}

			ptr = ptr->left;

		}
	}
}

// upper bound function
template < class DataType > 
typename SplayTree < DataType > :: iterator SplayTree < DataType > :: upper_bound ( DataType key ) {

	assert ( ( _type == __SPLAY_TREE_BST__ or _type == __SPLAY_TREE_BST_LAZY_PROPAGATION__ , " SplayTree is not a BST ") );

	std :: shared_ptr < node > ptr ( _head );

	while ( true ) {

		if ( ptr == end().ptr or comparator ( key , ptr->key ) ) {
			if ( ptr->left == nullptr ) {
				splay ( ptr );
				return iterator ( ptr );
			}
			ptr = ptr->left;
		} else {
			if ( ptr->right == nullptr ) {
				splay ( ptr );
				if ( comparator ( key , ptr->key ) ) {
					return iterator ( ptr );
				} else {
					return iterator ( ptr->succ );
				}
			}
			ptr = ptr->right;
		}
	}
}
// -- begin insert function

// insert at a given iterator
template < class DataType > 
typename SplayTree < DataType > :: iterator SplayTree < DataType > :: insert ( iterator it , DataType key ) {

	std :: shared_ptr < node > ptr ( new node ( key ) );

	splay ( it );

	if ( it.ptr->left == nullptr ) {
		
		// assign new node to the left
		it.ptr->left = ptr;
		
		// set new begin 
		_begin = ptr;

		// fix pred / succ
		ptr->succ = it.ptr;
		it.ptr->pred = ptr;

	} else { 
		it.ptr = it.ptr->left;
		while ( it.ptr->right ) {
			it.ptr = it.ptr->right;
		}

		it.ptr->right = ptr;

		// fix pred / succ
		it.ptr->succ->pred = ptr;
		ptr->succ = it.ptr->succ;

		it.ptr->succ = ptr;
		ptr->pred = it.ptr;
	}

	splay ( iterator ( ptr ) );

	return iterator ( ptr );
}

// insert before a given key
template < class DataType >
typename SplayTree < DataType > :: iterator SplayTree < DataType > :: insert ( DataType key ) {

	if ( _type == __SPLAY_TREE_BST__ or _type == __SPLAY_TREE_BST_LAZY_PROPAGATION__ ) {
		return insert ( upper_bound ( key ) , key );
	} else {
		return insert ( nth ( size() ) , key );
	}

}

// insert at a given position
template < class DataType > 
typename SplayTree < DataType > :: iterator SplayTree < DataType > :: insert ( size_t index , DataType key ) {
	return insert ( nth ( index ) , key );
}

// -- end insert function

// return number of nodes in tree
template < class DataType > 
size_t SplayTree < DataType > :: size ( void ) {
	return get_size ( _head ) - 1u; 
}

// return wheather tree is empty
template < class DataType > 
bool SplayTree < DataType > :: empty ( void ) {
	return size() == 0u;
}

// return end() iterator 
template < class DataType > 
typename SplayTree < DataType > :: iterator SplayTree < DataType > :: end ( void ) {
	return iterator( _end );
}

// return begin() iterator
template < class DataType > 
typename SplayTree < DataType > :: iterator SplayTree < DataType > :: begin ( void ) {
	return iterator( _begin	);
}

// distance function
template < class DataType >
size_t std :: distance ( typename SplayTree < DataType > :: iterator it1 , typename SplayTree < DataType > :: iterator it2 ) {
	return abs ( SplayTree < DataType > :: position ( it2 ) - SplayTree < DataType > :: position ( it1 ) );
}

// -- end helper functions of splay tree

#endif

// testing functions

#include <iostream>

int main () {

	// SplayTree < int > tree ( std :: greater<int> );

	SplayTree < int > tree = SplayTree < int > ( std :: greater < int >() );

	for(int i = 0;i < 10;i++) {
		tree.insert( rand() );
	}

	for(int i = 1;i <= 10;i++) {
		std::cout << *tree[i] << "\n";
		// tree.remove(tree[1]);
	}

	return 0;
}