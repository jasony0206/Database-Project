#include <iostream>
#include <string>
#include <stack>

//struct Node;

class MultiMap
{
private:
	struct Node
	{
		Node(){}
		Node(std::string key, unsigned int value, Node *p){
			m_key = key;
			m_value = value;
			m_left = NULL;
			m_right = NULL;
			m_parent = p;
		}

		std::string m_key;
		unsigned int m_value;
		Node *m_left;
		Node *m_right;
		Node *m_parent;
	};
public: 
	class Iterator
	{
	public:
		Iterator();
		//Iterator(parameters);
		Iterator(const MultiMap *p);
		Iterator(Node *p);
		bool valid() const;
		std::string getKey() const;
		unsigned int getValue() const;
		bool next();
		bool prev();
	private:
		Node *m_cur;
	};

	MultiMap();
	~MultiMap();
	void clear();
	void insert(std::string key, unsigned int value);
	Iterator findEqual(std::string key) const;
	Iterator findEqualOrSuccessor(std::string key) const;
	Iterator findEqualOrPredecessor(std::string key) const;


private:

	Node *m_root;
	void clearHelper(Node *cur);
	Iterator findEqualHelper(Node *cur, std::string key) const;
	Iterator findEqualOrSuccessorHelper(Node *cur, std::string key) const;
	Iterator findEqualOrPredecessorHelper(Node *cur, std::string key) const;

	//To prevent Multimaps from being copied or assigned, declare these 
	//members private and do not implement them.
	MultiMap(const MultiMap& other);
	MultiMap& operator=(const MultiMap& rhs);
};
