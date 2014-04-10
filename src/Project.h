#ifndef _PROJECT_H_INCLUDED_
#define _PROJECT_H_INCLUDED_
#include "common.h"
#include "Node.h"
#include "EditorState.h"

class Node;
class EditorState;
class Project
{
public:
	Project();
	~Project();

	// Properties
	stringc name;
	stringc file;

	// Nodes
	void AddNode(EditorState* state, bool select=true);
	void AddNode(Node* node, bool select = true);
	void DeleteNode(int id);
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
	int GetSelectedNodeId() const{return snode;}
	unsigned int GetNodeCount() const{ return _node_count; }
private:
	int snode;
	list<Node*>* nodes;
	unsigned int _node_count;
};

#endif
