#include <irrlicht.h>
#include "common.h"
#include "Editor.h"

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#ifndef _DEBUG
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif
#endif

int main(){
	// create device
    irr::IrrlichtDevice* device = irr::createDevice(irr::video::EDT_OPENGL, irr::core::dimension2d<irr::u32>(640, 480));

    if (device == 0)
        return 0; // could not create selected driver.

    irr::video::IVideoDriver* driver = device->getVideoDriver();
    irr::scene::ISceneManager* smgr = device->getSceneManager();
    irr::gui::IGUIEnvironment* gui = device->getGUIEnvironment();

	Editor* editor = new Editor();
	editor->run(device);

    return 1;
}