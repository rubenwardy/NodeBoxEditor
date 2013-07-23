#ifndef _NODE_H_INCLUDED_
#define _NODE_H_INCLUDED_
#include "common.h"

class Node
{
public:
	// Construction / Destruction
	Node(IrrlichtDevice* mdevice,ed_data* n_ed);
	~Node();

	// Get functions
	int GetId();
	NodeBox* GetCurrentNodeBox();
	NodeBox* GetNodeBox(int id);

	// Operation functions
	NodeBox* addNodeBox();
	void select(int id);
	void update();
	void resizeNodeBoxFace(CDR_TYPE type,vector3df position,bool both);
	void moveNodeBox(CDR_TYPE type,vector3df position);
	void deleteNodebox(int id);
	vector3df getPosition() const;
	void setPosition(vector3df in);	

	// Build node models
	void update(int id);
	void remesh();

	// Build code 
	stringc* build(BUILD_TYPE type); // Build full node
	stringc* build(NodeBox* nodebox); // Build node box

	void defrag();
private:
	// Data
	NodeBox* boxes[NODEB_MAX];
	ed_data* editor;
	int _id;
	int number;	
	f32 snappers[NODE_RES+1];
	vector3df nd_position;

	// Irrlicht
	video::IVideoDriver* driver;
	IrrlichtDevice* device;
	ISceneManager* smgr;
};

#endif