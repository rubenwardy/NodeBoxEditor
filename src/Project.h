#ifndef _PROJECT_H_INCLUDED_
#define _PROJECT_H_INCLUDED_

#include <string>
#include <list>
#include "common.h"

class Node;
class EditorState;

class Project
{
public:
	Project();
	~Project();

	// Properties
	std::string name;
	std::string file;

	// Nodes
	void AddNode(EditorState* state, bool select=true);
	void AddNode(Node* node, bool select = true);
	void DeleteNode(int id);
	void SelectNode(int id) { snode = id; }
	Node* GetNode(int id) const;
	Node* GetNode(vector3di pos) const;	
	Node* GetCurrentNode() const;
	int GetSelectedNodeId() const { return snode; }
	unsigned int GetNodeCount() const { return _node_count; }

	std::list<Node*> nodes;
private:
	int snode;
	unsigned int _node_count;
};

#endif
