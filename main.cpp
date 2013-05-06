// Copyright (c) to Andrew "rubenwardy" Ward


#include <irrlicht.h>
#include "define.h"
#include "cEditor.h"

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#endif

#ifndef _DEBUG
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

using namespace irr;
using namespace gui;
using namespace scene;
using namespace core;
using namespace video;


int main()
{  
	IrrlichtDevice *device = createDevice( video::EDT_OPENGL, dimension2d<u32>(800, 640), 16,false, false, false, 0);
	if (!device)
		return 0;
	device->setWindowCaption(L"Loading Engine...");

	cEditor* editor=new cEditor();
	device->setEventReceiver(editor);
	editor->run(device);

	return 1;
}