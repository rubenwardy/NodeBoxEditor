#ifndef _NODE_H_INCLUDED_
#define _NODE_H_INCLUDED_

#include <vector>
#include "common.h"
#include "EditorState.h"
#include "NodeBox.h"

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
