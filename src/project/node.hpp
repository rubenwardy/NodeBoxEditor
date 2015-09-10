#ifndef NODE_HPP_INCLUDED
#define NODE_HPP_INCLUDED

#include <vector>
#include "../common.hpp"
#include "../EditorState.hpp"
#include "nodebox.hpp"
#include "media.hpp"

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
	NodeBox* addNodeBox(vector3df one = vector3df(-0.5, -0.5, -0.5),
		vector3df two = vector3df(0.5, 0.5, 0.5));
	void deleteNodebox(int id);
	void cloneNodebox(int id);
	void select(int id) { _selected = id; }

	// Node bulk updaters
	void remesh(bool force = false); // creates the node mesh
	void remesh(NodeBox *box);
	void setAllTextures(Media::Image *def);
	void rotate(EAxis axis);
	void flip(EAxis axis);
	void hide();

	void setTexture(ECUBE_SIDE face, Media::Image *image);
	Media::Image *getTexture(ECUBE_SIDE face) { return images[face]; }

	vector3di position;
	std::string name;
	std::vector<NodeBox*> boxes;
	int snap_res;
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
