#ifndef MAP_CLASS_HPP
#define MAP_CLASS_HPP
#include <iostream> // FOR STD::ALLOCATOR
#include "../../iterators/map_iterator.hpp"
#include "../../utils_containers/less.hpp"
#include "../../utils_containers/pair.hpp"
#include "../../utils_containers/make_pair.hpp"

namespace ft {
	template<class _Key, class T, class Compare = ft::less<_Key>, class _Allocator = std::allocator <ft::pair<const _Key, T> > >
	class map {
	public:
		typedef _Key key_type;
		typedef T mapped_type;
		typedef ft::pair<const key_type, mapped_type> value_type;
		typedef Compare key_compare;
		typedef _Allocator allocator_type;
		typedef value_type &reference;
		typedef const value_type &const_reference;
		typedef value_type *pointer;
		typedef const value_type *const_pointer;
		typedef size_t size_type;
	private:
		struct Node {
			value_type value;
			Node *parrent;
			Node *left;
			Node *right;
		};
		size_type _size;
		allocator_type _type_allocator;
		std::allocator<Node> _node_allocator;
		Node *_root;
	public:
		typedef ft::map_iterator<key_type, mapped_type, key_compare, Node> iterator;
		// PRINT - FUNCTIONS FOR TESTS
		void print() { printBT("" ,_root, false); }

		// DEFAULT_CONSTRUCTOR
		explicit map(const allocator_type &alloc = allocator_type())
			: _size(0), _type_allocator(alloc) {
			_root = createNode(value_type(), nullptr);
		}

		// COPY CONSTRUCTOR
		map (const map& x) : _size(0) {
			_root = createNode(value_type(), nullptr);
			this->operator=(x);
		}
		// DESTRUCTOR
		~map() {
			deleteNode(&_root);
		}

		// OPERATOR =
		map& operator=(const map& x) {
			this->clear();
			copyNode(x._root);
			this->_size = x._size;
			this->_type_allocator = x._type_allocator;
			this->_node_allocator = x._node_allocator;
			return *this;
		}

		/*---------Iterators:---------*/
		//BEGIN
		iterator begin() { return iterator(_root).getNode(); }
//		const_iterator begin() const;
		//END
		iterator end() { return iterator(_root).getLastNode(); }
//		const_iterator end() const;

		/*---------CAPACITY---------*/
		// EMPTY
		bool empty() const { return _size == 0; }
		// SIZE
		size_type size() const { return _size; }

		//MAX_SIZE
		size_type max_size() const { return _node_allocator.max_size(); }

		/*---------Element access:---------*/
		// OPERATOR[]
		mapped_type &operator[](const key_type &k) {
			value_type pair(k, 0);
			if (this->count(k) == 0)
				this->addNode(pair);
			return this->searchNode(_root, pair)->value.second;
		}
		/*---------MODIFIERS---------*/
		// ERASE
		size_type erase (const key_type& k) {
			map tmp(*this);
			if (this->count(k) == 0)
				return 0;
			this->clear();
			this->addTmpBesidesK(tmp, k);
			return 1;
		}
		// SWAP
		void swap(map &x) {
			map tmp(*this);
			*this = x;
			x = tmp;
		}
		//CLEAR
		void clear() {
			deleteNode(&_root);
			_root = createNode(value_type(), nullptr);
		}

		/*---------Operations:---------*/
		//COUNT
		size_type count (const key_type& k) const {
			value_type pair(k, 0);
			Node *tmp = searchNode(_root, pair);
			return tmp == nullptr ? 0 : 1;
		}
		/*---------Allocator:---------*/
		// get_allocator()
		allocator_type get_allocator() const { return _type_allocator; }
	private:
		void copyNode(Node *root) {
			value_type tmp(root->value.first, root->value.second);
			this->addNode(tmp);
			if (root != nullptr) {
				if (root->left != nullptr)
					copyNode(root->left);
				if (root->right != nullptr)
					copyNode(root->right);
			}
		}
		Node *createNode(const value_type &value, Node *root) {
			Node *newNode;
			newNode = _node_allocator.allocate(1);
			_type_allocator.construct(&newNode->value, value_type(value));
			newNode->parrent = root;
			newNode->left = nullptr;
			newNode->right = nullptr;
			return newNode;
		}
		Node *searchNode(Node *root, const value_type &value) const {
			if (root == nullptr)
				return nullptr; // Не нашли
			if (root->value.first == value.first)
				return root; // Нашли
			if (value > root->value) {
				if (root->right != nullptr) {
					root = root->right; // Ищем справа
					return searchNode(root, value);
				}
			} else if (value.first < root->value.first) {
				if (root->left != nullptr) {
					root = root->left; // Ищем слева
					return searchNode(root, value);
				}
			}
			return nullptr; // Не нашли
		}
		void printBT(const std::string &prefix, const Node *node, bool isLeft) {
			if (node != nullptr) {
				std::cout << prefix;
				std::cout << (isLeft ? "├──" : "└──");
				std::cout << node->value.first << node->value.second << std::endl;
				printBT(prefix + (isLeft ? "│   " : "    "), node->left, true);
				printBT(prefix + (isLeft ? "│   " : "    "), node->right, false);
			}
		}
		void deleteNode(Node **root) {
			if (*root) {
				deleteNode(&(*root)->left);
				deleteNode(&(*root)->right);
				_type_allocator.destroy(&(*root)->value);
				_node_allocator.deallocate(*root, 1);
				*root = nullptr;
				if (_size)
					_size--;
			}
		}
		void addNode(const value_type &value) {
			if (_size == 0) {
				deleteNode(&_root);
				_root = createNode(value, nullptr);
				_size++;
				return;
			}
			if (value.first < _root->value.first) {
				if (_root->left) {
					_root = _root->left;
					return addNode(value);
				} else {
					_root->left = createNode(value, _root);
				}
			} else if (value.first > _root->value.first) {
				if (_root->right) {
					_root = _root->right;
					return addNode(value);
				} else {
					_root->right = createNode(value, _root);
				}
			}
			returnPointerRoot();
			_size++;
		}
		void addTmpBesidesK(map tmp, const key_type &k) {
			value_type q(tmp._root->value.first, tmp._root->value.second);
			if (q.first != k)
				addNode(q);
			if (tmp._root != nullptr) {
				if (tmp._root->left != nullptr) {
					tmp._root = tmp._root->left;
					addTmpBesidesK(tmp, k);
				}
				tmp.returnPointerRoot();
				if (tmp._root->right != nullptr) {
					tmp._root = tmp._root->right;
					addTmpBesidesK(tmp, k);
				}
			}
			tmp.returnPointerRoot();
		}
		void returnPointerRoot() {
			while (_root->parrent != nullptr) {
				_root = _root->parrent;
			}
		}
	};
}

#endif