#ifndef _PROJECT_H_INCLUDED_
#define _PROJECT_H_INCLUDED_
#include "common.h"
#include "Node.h"

class Node;
class Project
{
public:
	Project();
	~Project();

	// Properties
	stringc name;
	stringc file;

	void AddNode(Node* node) const;	
	void SelectNode(int id){snode = id;}
	Node* GetNode(int id) const;
	Node* GetNode(vector3di pos) const;	
	Node* GetCurrentNode() const{
		if (snode>-1){
			return GetNode(snode);
		}else{
			return NULL;
		}
	};
	list<Node*>* GetList() const{return nodes;}
private:
	int snode;
	list<Node*>* nodes;
	unsigned int number_of_nodes;
};

#endif
