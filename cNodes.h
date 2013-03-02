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
	cNode(IrrlichtDevice* mdevice,ed_data* n_ed);
	bool load(wchar_t* input);
	bool save(wchar_t* output);
	bool switchFocus(ISceneNode* hit);
	const sBox* addNodeBox();
	void updateTexts();
	void changeID(int n_id);
	void resize(int side, f32 dir); //0:Up 1:Left 2:Right 3:Down
	void update();
private:
	ed_data* editor;
	int id;
	int number;
	sBox* boxes[50];

	IVideoDriver* driver;
	IrrlichtDevice* device;
	ISceneManager* smgr;

	void resizeObject(sBox* input,irr::f32 px,irr::f32 py,irr::f32 pz);
	void setsizeObject(sBox* input,irr::f32 px,irr::f32 py,irr::f32 pz);
	void checkScaling(sBox* input);
};

#endif