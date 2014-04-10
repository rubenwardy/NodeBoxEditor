#include "Project.h"

Project::Project()
:snode(-1), _node_count(0)
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
	for(list<Node*>::ConstIterator Iterator = nodes->begin(); Iterator != nodes->end(); ++Iterator)
	{
		Node* n = *Iterator;
		if (n && n->NodeId() == id)
			return n;
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

void Project::AddNode(EditorState* state,bool select){
	Node* node = new Node(state->GetDevice(), state, _node_count);
	AddNode(node,select);
}

void Project::AddNode(Node* node,bool select){
	_node_count++;
	if (node->name == ""){
		core::stringc nd = "Node";
		nd += _node_count;
		node->name = nd;
	}
	node->setPosition(vector3di((_node_count - 1), 0, 0));
	nodes->push_back(node);
	if (select)
		snode = _node_count - 1;
}

void Project::DeleteNode(int id){
	if (snode == id)
		snode = -1;

	for (list<Node*>::Iterator Iterator = nodes->begin(); Iterator != nodes->end(); ++Iterator)
	{
		Node* n = *Iterator;
		if (n && n->NodeId() == id){
			nodes->erase(Iterator);
			delete n;
			return;
		}
	}
}
