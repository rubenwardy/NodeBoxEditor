#ifndef NODE_HPP_INCLUDED
#define NODE_HPP_INCLUDED

#include <vector>
#include "common.hpp"
#include "EditorState.hpp"
#include "NodeBox.hpp"
#include "MediaManager.hpp"

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
	void setAllTextures(Media::Image *def);
	void hide();

	void setTexture(CubeSide face, Media::Image *image);
	Media::Image *getTexture(CubeSide face) { return images[face]; }

	vector3di position;
	std::string name;
	std::vector<NodeBox*> boxes;
private:
	// Data
	int _selected;
	unsigned int _nid; // the node's id.
	int _box_count;

	// Irrlicht
	IrrlichtDevice* device;
	EditorState* state;
	Media::Image *images[6];
};

#endif
