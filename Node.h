#ifndef _NODE_H_INCLUDED_
#define _NODE_H_INCLUDED_
#include "common.h"
#include "EditorState.h"
#include "Nodebox.h"

class EditorState;
class NodeBox;
class Node
{
public:
	// Construction / Destruction
	Node(IrrlichtDevice* mdevice,EditorState* state);
	~Node();

	// Node box manager
	int GetId(){return _id;}
	NodeBox* GetCurrentNodeBox();
	NodeBox* GetNodeBox(int id);
	NodeBox* addNodeBox();
	void deleteNodebox(int id);
	void select(int id){_id = id;}
	
	// Node properties	
	vector3di getPosition() const{return nd_position;}
	void setPosition(vector3di in){nd_position = in;}
	stringc name;
	
	// Node bulk updaters
	void remesh(); // creates the node mesh
	void defrag(); // Defragments node array
private:
	// Data
	NodeBox* boxes[NODEB_MAX];
	int _id;
	int number;	
	vector3di nd_position;

	// Irrlicht
	IrrlichtDevice* _device;
	EditorState* _state;
};

#endif