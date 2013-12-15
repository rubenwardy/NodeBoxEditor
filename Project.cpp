#include "Project.h"

Project::Project()
:number_of_nodes(0),snode(-1)
{
	nodes = new list<Node*>();
}

Project::~Project(){
	for(list<Node*>::ConstIterator Iterator = nodes->begin(); Iterator != nodes->end(); ++Iterator)
	{
		Node* n = *Iterator;
		if (n)
			delete n;
	}
	delete nodes;
}

Node* Project::GetNode(int id) const{
	int a = 0;
	for(list<Node*>::ConstIterator Iterator = nodes->begin(); Iterator != nodes->end(); ++Iterator)
	{
		Node* n = *Iterator;
		if (n)
			if (a == id){
				return n;
			}else{
				a++;
			}
	}
	return NULL;
}

Node* Project::GetNode(vector3di pos) const{
	for(list<Node*>::ConstIterator Iterator = nodes->begin(); Iterator != nodes->end(); ++Iterator)
	{
		Node* n = *Iterator;
		if (n && n->getPosition() == pos)
			return n;
	}
	return NULL;
}

void Project::AddNode(Node* node) const{
	nodes->push_back(node);
}