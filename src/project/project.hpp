#ifndef PROJECT_HPP_INCLUDED
#define PROJECT_HPP_INCLUDED

#include <string>
#include <list>
#include "../common.hpp"
#include "../EditorState.hpp"
#include "media.hpp"
#include "node.hpp"

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

	// Media
	Media media;

	// Nodes
	void AddNode(EditorState* state, bool select = true, bool add_initial_box = true);
	void AddNode(Node* node, bool select = true);
	void DeleteNode(int id);
	void SelectNode(int id) { snode = id; }
	void hideAllButCurrentNode();
	void remesh();
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
