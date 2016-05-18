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

				// constructors

					// with key 
					node ( DataType );

					// without key
					node ( void );
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

		// fix function 
		void fix ( std :: shared_ptr < node > );

		// rotate function (use with splay) 
		void rotate ( std :: shared_ptr < node > );

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
			void insert ( SplayTree < DataType > );

			// insert a tree before the given iterator
			void insert ( iterator , SplayTree < DataType > );

			// insert a tree at the given position
			void insert ( size_t , SplayTree < DataType > );

		// remove a key

			// remove the matched key
			// perform like multi-set
			size_t erase ( DataType );

			// remove at the position
			void erase ( size_t );

			// with a given iterator
			void erase ( iterator );

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

		// shift functions
		iterator operator + ( iterator , int );
		iterator operator - ( iterator , int );

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
bool SplayTree < DataType > :: iterator :: operator == ( iterator iter ) { return ptr == iter.ptr; }
template < class DataType >
bool SplayTree < DataType > :: iterator :: operator != ( iterator iter ) { return ptr != iter.ptr; }

// increment function
template < class DataType >
void SplayTree < DataType > :: iterator :: operator ++ ( void ) {
	assert ( ( ptr != end().ptr , " cannot increase " ) );
	ptr = ptr->succ;
}

// decrement function
template < class DataType >
void SplayTree < DataType > :: iterator :: operator -- ( void ) {
	assert ( ( ptr != begin().ptr , " cannot decrease " ) );
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
void SplayTree < DataType > :: fix ( std :: shared_ptr < node > ptr ) {

	// fix subtree's size
	ptr->size = get_size ( ptr->left ) + get_size ( ptr->right ) + 1u;

}

// rotate operation ( helper function of splay )
template < class DataType > 
void SplayTree < DataType > :: rotate ( std :: shared_ptr < node > ptr ) {

	assert ( ( ptr->parent , " error rotate root " ) );

	std :: shared_ptr < node > par ( ptr->parent );

	ptr->parent = par->parent;

	if( par->parent != nullptr ) {
		if ( par->parent->left == par ) {
			par->parent->left = ptr;
		} else {
			par->parent->right = ptr;
		}
	}

	if( par->left == ptr ) {

		par->left = ptr->right;

		if( par->left != nullptr ) {
			par->left->parent = par;
		}

		ptr->right = par;
		ptr->right->parent = ptr;

	} else {

		par->right = ptr->left;

		if( par->right != nullptr ) {
			par->right->parent = par;
		}

		ptr->left = par;
		ptr->left->parent = ptr;

	}

	fix ( par );
	fix ( ptr );

}

// splay operation ( the heart of this data structure )
template < class DataType > 
void SplayTree < DataType > :: splay ( iterator it ) {
	
	while ( it.ptr->parent != nullptr ) { // not root

		std :: shared_ptr < node > par = it.ptr->parent;

		if( par->parent == nullptr ) {

			rotate ( it.ptr );

		} else if( ( par->parent->left == par and par->right == it.ptr ) or ( par->parent->right == par and par->left == it.ptr ) ) {
			
			rotate ( it.ptr );
			rotate ( it.ptr );
		
		} else {
			
			rotate ( par );
			rotate ( it.ptr );
		
		}

	}

	fix ( it.ptr );

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

	assert ( ( 0u <= index and index <= size() , " index out of bound " ) );

	iterator it = _head;

	while ( index != get_size ( it.ptr->left ) ) {

		if ( index < get_size ( it.ptr->left ) ) {
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

			if ( comparator ( *it , key ) ) {

				if ( it.ptr->left == nullptr ) {
					splay ( it );
					return end();
				}
				it = it.ptr->left;

			} else {
				
				if ( comparator ( key , *it ) ) {
					
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
			if ( (not comparator ( *it , key ) ) and (not comparator ( key , *it ) ) ) {
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
	return get_size ( it.ptr->left ); 
}

// lower bound function
template < class DataType >
typename SplayTree < DataType > :: iterator SplayTree < DataType > :: lower_bound ( DataType key ) {

	assert ( ( _type == __SPLAY_TREE_BST__ or _type == __SPLAY_TREE_BST_LAZY_PROPAGATION__ , " This tree is not a BST ") );

	std :: shared_ptr < node > ptr ( _head );

	while ( true ) {

		if ( ptr != end().ptr and comparator ( ptr->key , key ) ) {

			if ( ptr->right == nullptr ) {
				splay ( ptr );
				return iterator ( ptr->succ );
			}
			
			ptr = ptr->right;

		} else { 

			if ( ptr->lett == nullptr ) {
				splay ( ptr );
				if ( ptr != begin().ptr and comparator ( key , ptr->pred->key )) {
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
		ptr->parent = it.ptr;

		// set new begin 
		_begin = ptr;

		// fix pred / succ
		ptr->succ = it.ptr;
		it.ptr->pred = ptr;

	} else { 

		it.ptr = it.ptr->left;
		while ( it.ptr->right != nullptr ) {
			it.ptr = it.ptr->right;
		}

		it.ptr->right = ptr;
		ptr->parent = it.ptr;

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

// insert a tree before the given iterator
template < class DataType >
void SplayTree < DataType > :: insert ( iterator it , SplayTree < DataType > tree ) {

	if ( tree.size() == 0u ) {
		return;
	}

	if ( size() == 0u ) {
		_head = tree.begin();
		splay ( _head );
		return; 
	}

	if ( it == end() ) {

		auto it = nth ( size() - 1u );

		erase ( it->right );

		tree.splay ( tree.begin() );

		it->right = tree.begin();

		splay ( it );
	}
}

// insert at th given position
template < class DataType > 
void SplayTree < DataType > :: insert ( size_t pos , SplayTree < DataType > tree ) {
	insert ( nth ( pos ) , tree );
}

// insert before the end() iterator
template < class DataType > 
void SplayTree < DataType > :: insert ( SplayTree < DataType > tree ) {
	insert ( size() , tree );
}


// -- end insert function

// -- begin erase function

// erase iterator 
template < class DataType > 
void SplayTree < DataType > :: erase ( iterator it ) {
	
	assert ( ( it != end() , " try to erase end() " ) );

	splay ( it );

	if ( it == begin() ) { 
		
		_begin = it.ptr->succ; 

		it.ptr->succ->pred = nullptr;
		it.ptr->succ = nullptr;
		it.ptr->right->parent = nullptr;
		it.ptr->right = nullptr;
	
		splay ( _begin );

	} else {

		std :: shared_ptr < node > left ( it.ptr->pred );
		std :: shared_ptr < node > right ( it.ptr->succ );

		it.ptr->pred->succ = it.ptr->succ;
		it.ptr->succ->pred = it.ptr->pred;

		it.ptr->pred = it.ptr->succ = nullptr;
		it.ptr->left->parent = it.ptr->right->parent = nullptr;
		it.ptr->left = it.ptr->right = nullptr;

		splay ( left );

		left->right = right;
		right->parent = left;

		splay ( right );
	}

}

// erase every candidate with the same key
template < class DataType > 
size_t SplayTree < DataType > :: erase ( DataType key ) {

	if ( _type == __SPLAY_TREE_BST__ or _type == __SPLAY_TREE_BST_LAZY_PROPAGATION__ ) {

		iterator it = find ( key );

		if ( it == end() ) {
			return;
		}

		while ( it != begin() and ( not comparator ( *prev ( it ) , key ) ) and ( not comparator ( key , *prev ( it ) ) ) ) {
			erase ( prev ( it ) );
		}

		while ( it != end() and ( not comparator ( *next ( it ) , key ) ) and ( not comparator ( key , *next ( it ) ) ) ) {
			erase ( next ( it ) );
		}

	} else {

		for ( iterator it = begin() + 1u ; it != end() ; it++ ) {
			if ( ( not comparator ( *prev( it ) , key ) ) and ( not comparator ( key , *prev( it ) ) ) ) {
				erase ( prev ( it ) );
			}
		}

	}
}

template < class DataType > 
void SplayTree < DataType > :: erase ( size_t index ) {
	erase ( nth ( index ) );
}

// -- end erase function 

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

// ## functions with namespace std

// get next iterator
template < class DataType > 
typename SplayTree < DataType > :: iterator std::next ( typename SplayTree < DataType > :: iterator it ) {
	it++;
	return it;
}

// get prev iterator
template < class DataType >
typename SplayTree < DataType > :: iterator std::prev ( typename SplayTree < DataType > :: iterator it ) {
	it--;
	return it;
}

// distance function
template < class DataType >
size_t std :: distance ( typename SplayTree < DataType > :: iterator it1 , typename SplayTree < DataType > :: iterator it2 ) {
	return abs ( SplayTree < DataType > :: position ( it2 ) - SplayTree < DataType > :: position ( it1 ) );
}

// shift functions
template < class DataType >
typename SplayTree < DataType > :: iterator operator + ( typename SplayTree < DataType > :: iterator it , int shift) {
	return nth ( position ( it ) + shift );
}
template < class DataType >
typename SplayTree < DataType > :: iterator operator - ( typename SplayTree < DataType > :: iterator it , int shift ) {
	return nth ( position ( it ) - shift );
}

// -- end helper functions of splay tree

#endif

// ## testing part

#include <iostream>

int main () {

	SplayTree<int> tree = SplayTree<int>( std :: greater<int>() );

	const int N = 10;
	for(int i = 0;i < N;i++) {
		int val = rand();
		std :: cout << tree.size() << " " << val << std :: endl;
		tree.insert( val );
	}

	for(int i = 0;i < N;i++) {
		std::cout << *tree[0] << "\n";
		tree.erase ( tree[0] );
	}

	std :: cout << "end " << std :: endl;

	return 0;
}