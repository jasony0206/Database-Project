#include "MultiMap.h"
using namespace std;


MultiMap::MultiMap(){
	m_root = NULL;
}

void MultiMap::insert(std::string key, unsigned int value){
	//empty tree
	if(m_root == NULL){		
		m_root = new Node(key, value, NULL);
		return;	}
	//find the right place
	Node *cur = m_root;
	for(;;){
		if(key < cur->m_key){	//less than or equal 
			if(cur->m_left != NULL)
				cur = cur->m_left;
			else{
				cur->m_left = new Node(key, value, cur);
				return;
			}
		}
		else if(key >= cur->m_key){
			if(cur->m_right != NULL)
				cur = cur->m_right;
			else{
				cur->m_right = new Node(key, value, cur);
				return;
			}
		}
	}
}

void MultiMap::clearHelper(Node *cur){
	//stack overflow when large
	if(cur == NULL) return;
	clearHelper(cur->m_left);
	clearHelper(cur->m_right);
	delete cur;	
}


void MultiMap::clear(){
	clearHelper(m_root);
	m_root = NULL;
}

MultiMap::~MultiMap(){
	clear();
}

//////////////////////////
//ITERATOR IMPLEMENTATION
//////////////////////////

MultiMap::Iterator::Iterator(){
	m_cur = NULL;
}

MultiMap::Iterator::Iterator(const MultiMap *p){
	m_cur = p->m_root;
}

MultiMap::Iterator::Iterator(Node *p){
	m_cur = p;
}



bool MultiMap::Iterator::valid() const{
	return (m_cur != NULL);
}

string MultiMap::Iterator::getKey() const{
	if(valid())
		return m_cur->m_key;
	//if invalid, do whatever
	else{
		cerr<< "invalid!" <<endl;
	}
}

unsigned int MultiMap::Iterator::getValue() const{
	if(valid())
		return m_cur->m_value;
	//if invalid, do whatever
	cerr<< "invalid!" <<endl;
}

bool MultiMap::Iterator::next(){
	if(!valid())	return false;
	//if I'm the root node or a node w/ a right child, 
	//find the left-most of the right subtree
	if(m_cur->m_parent == NULL || m_cur->m_right != NULL){
		m_cur = m_cur->m_right;
		if(m_cur == NULL)
			return false;
		while(m_cur->m_left != NULL)
			m_cur = m_cur->m_left;
		return true;
	}

	//if I'm a left child node with no right child, 
	//my immediate parent is my next
	if(m_cur == m_cur->m_parent->m_left && m_cur->m_right == NULL){
		m_cur = m_cur->m_parent;
		return true;
	}

	//if I'm a right child node with no right child, 
	//my first "left-child ancestor"'s immediate parent is my next
	else if(m_cur == m_cur->m_parent->m_right && m_cur->m_right == NULL){
		Node *trace = m_cur;
		//find the first left-child ancestor
		while(trace != NULL && trace->m_parent !=NULL){
			if(trace == trace->m_parent->m_left){
				trace = trace->m_parent;
				m_cur = trace;
				return true;
			}
			else
				trace = trace->m_parent;
		}
		//trace reached the root node, which means there's no next node.
		m_cur = NULL;	//invalid now
		return false;
	}
}

bool MultiMap::Iterator::prev(){
	if(!valid())	return false;
	//if I'm the root node or a node w/ a left child, 
	//find the right-most of the left subtree
	if(m_cur->m_parent == NULL || m_cur->m_left != NULL){
		m_cur = m_cur->m_left;
		if(m_cur == NULL)
			return false;
		while(m_cur->m_right != NULL)
			m_cur = m_cur->m_right;
		return true;
	}

	//if I'm a right child node with no left child, 
	//my immediate parent is my prev
	if(m_cur == m_cur->m_parent->m_right && m_cur->m_left == NULL){
		m_cur = m_cur->m_parent;
		return true;
	}
	//if I'm a left child with no left child,
	//my first "right-child ancestor"'s immediate parent is my prev
	else if(m_cur == m_cur->m_parent->m_left && m_cur->m_left == NULL){
		Node *trace = m_cur;
		//find the first right-child ancestor
		while(trace != NULL && trace->m_parent !=NULL){
			if(trace == trace->m_parent->m_right){
				trace = trace->m_parent;
				m_cur = trace;
				return true;
			}
			else
				trace = trace->m_parent;
		}
		//trace reached the root node, which means there's no prev node.
		m_cur = NULL;	//invalid now
		return false;
	}
}


MultiMap::Iterator MultiMap::findEqualHelper(Node *cur, std::string key) const{
	if(cur == NULL){
		Iterator invalidIt;
		return invalidIt;
	}
	if(key < cur->m_key)
		return findEqualHelper(cur->m_left, key);
	else if(key == cur->m_key){
		Iterator it1(cur);
		return it1;
	}
	else if(key > cur->m_key)
		return findEqualHelper(cur->m_right, key);
}

MultiMap::Iterator MultiMap::findEqual(std::string key) const{
	return findEqualHelper(m_root, key);
}

MultiMap::Iterator MultiMap::findEqualOrSuccessorHelper(Node *cur, std::string key) const{
	if(cur == NULL){
		Iterator invalidIt;
		return invalidIt;
	}
	Iterator it1(cur);
	//first look for it in left subtree
	it1 = findEqualOrSuccessorHelper(cur->m_left, key);
	if(it1.valid())
		return it1;
	Iterator it2(cur);
	Iterator it3 = it2;
	if(!it3.prev() && it2.getKey() >= key)		//earliest
		return it2;
	if(it2.getKey() >= key && it3.getKey() < key){
		return it2;
	}
	return findEqualOrSuccessorHelper(cur->m_right, key);
	
}

MultiMap::Iterator MultiMap::findEqualOrPredecessorHelper(Node *cur, std::string key) const{
	if(cur == NULL){
		Iterator invalidIt;
		return invalidIt;
	}
	Iterator it1(cur);
	it1 = findEqualOrPredecessorHelper(cur->m_right, key);
	if(it1.valid())
		return it1;
	Iterator it2(cur);
	Iterator it3 = it2;
	if(!it3.next() && it2.getKey() <= key)		//last
		return it2;
	if(it2.getKey() <= key && it3.getKey() > key){
		return it2;
	}
	return findEqualOrPredecessorHelper(cur->m_left, key);
}

MultiMap::Iterator MultiMap::findEqualOrSuccessor(std::string key) const{
	return findEqualOrSuccessorHelper(m_root, key);
}

MultiMap::Iterator MultiMap::findEqualOrPredecessor(std::string key) const{
	return findEqualOrPredecessorHelper(m_root, key);
}





/*
int main()
{
	MultiMap myMultiMap; 
	myMultiMap.insert("Andrea", 6); 
	myMultiMap.insert("Bill", 2); 
	myMultiMap.insert("Carey", 5); 
	myMultiMap.insert("Bill", 8); 
	myMultiMap.insert("Batia", 4); 
	myMultiMap.insert("Larry", 7); 
	myMultiMap.insert("Larry", 9); 
	myMultiMap.insert("Bill", 3); 

	MultiMap::Iterator it1 = myMultiMap.findEqualOrSuccessor("Bil");
	while(it1.valid())
	{ 
	cout << it1.getKey() << " " << it1.getValue() << endl; 
	it1.next(); // advance to the next associaton 
	} 
	 
	 

}
*/
