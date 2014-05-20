#ifndef NODE_HPP_INCLUDED
#define NODE_HPP_INCLUDED

#include <vector>
#include "common.hpp"
#include "EditorState.hpp"
#include "NodeBox.hpp"

class EditorState;
class NodeBox;
class Node
{
public:
	// Construction / Destruction
	Node(IrrlichtDevice* device, EditorState* state, unsigned int id);
	~Node();

	// Node box manager
	int GetId() const { return _selected; }
	unsigned int NodeId() const { return _nid; }
	NodeBox* GetCurrentNodeBox();
	NodeBox* GetNodeBox(int id);
	NodeBox* addNodeBox();
	void deleteNodebox(int id);
	void select(int id) { _selected = id; }

	// Node bulk updaters
	void remesh(); // creates the node mesh

	vector3di position;
	std::string name;
	std::vector<NodeBox*> boxes;
private:
	// Data
	int _selected;
	unsigned int _nid; // the node's id.

	// Irrlicht
	IrrlichtDevice* device;
	EditorState* state;
};

#endif
