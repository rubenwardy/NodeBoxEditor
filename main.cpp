// Copyright (c) to Andrew "rubenwardy" Ward


#include <irrlicht.h>
#include "define.h"
#include "cEditor.h"

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#endif

using namespace irr;
using namespace gui;
using namespace scene;
using namespace core;
using namespace video;


int main()
{  
	dimension2d<u32> resolution = dimension2d<u32>(860, 640);
	bool fullscreen = false;
	IrrlichtDevice *device = createDevice( video::EDT_OPENGL,  resolution, 16, fullscreen, false, fullscreen, 0);
	if (!device)
		return 0;
	device->setWindowCaption(L"Loading Engine...");

	cEditor* editor=new cEditor();
	device->setEventReceiver(editor);
	editor->run(device, resolution);

	return 1;
}