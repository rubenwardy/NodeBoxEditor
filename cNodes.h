#ifndef _cNodes_h_included_
#define _cNodes_h_included_

#include <irrlicht.h>
#include "define.h"
#include <iostream>

using namespace irr;
using namespace gui;
using namespace scene;
using namespace core;
using namespace video;

class cNode
{
public:
	// Construct
	cNode(IrrlichtDevice* mdevice,ed_data* n_ed);

	// File Operations
	bool load(wchar_t* input);
	bool save(wchar_t* output);

	// NodeBox management functions
	sBox* addNodeBox();
	void deleteNodebox(sBox* nodebox);
	void defrag();

	// Nodebox resizing functions
	void resize(int side, f32 dir); //0:Up 1:Left 2:Right 3:Down
	void resizeNodeBoxFace(sBox* nodebox,CDR_TYPE face, vector3df target);

	// API Functions
	sBox* getCurrentNodeBox();
	IGUIContextMenu* menu;

	// Generation overload functions
	stringc* build(BUILD_TYPE type); // Build full node
	stringc* build(sBox* nodebox); // Build node box
	
	// Switch 
	void switchFocus(ISceneNode* hit);
	void changeID(int n_id);

	// Update functions
	void updateTexts();
	void update();	
private:
	ed_data* editor;
	int id;
	int number;
	sBox* boxes[NODEB_MAX];

	IVideoDriver* driver;
	IrrlichtDevice* device;
	ISceneManager* smgr;

	bool setsizeObject(sBox* input,irr::f32 px,irr::f32 py,irr::f32 pz);
};

#endif