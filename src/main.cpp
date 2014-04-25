#include <stdlib.h>
#include <iostream>
#include <irrlicht.h>
#include "util/string.hpp"
#include "common.h"
#include "Editor.h"

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#ifndef _DEBUG
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif
#endif

int main(int argc, char *argv[]) {
	std::cerr <<
		" _   _           _        ____              _____    _ _ _             \n"
		"| \\ | | ___   __| | ___  | __ )  _____  __ | ____|__| (_) |_ ___  _ __ \n"
		"|  \\| |/ _ \\ / _` |/ _ \\ |  _ \\ / _ \\ \\/ / |  _| / _` | | __/ _ \\| '__|\n"
		"| |\\  | (_) | (_| |  __/ | |_) | (_) >  <  | |__| (_| | | || (_) | |   \n"
		"|_| \\_|\\___/ \\__,_|\\___| |____/ \\___/_/\\_\\ |_____\\__,_|_|\\__\\___/|_|   \n\n"
		<< std::endl;

	// Settings
	Configuration* conf = new Configuration();
	if (conf == NULL) {
		return EXIT_FAILURE;
	}

	// Init Settings
	conf->set("snapping", "true");
	conf->set("limiting", "true");
	conf->set("driver", "opengl");
	conf->set("hide_sidebar", "false");
	conf->set("always_show_position_handle", "false");
	conf->set("vsync", "true");
	conf->set("use_sleep", "false");
	conf->set("fullscreen", "false");
	conf->set("width", "896");
	conf->set("height", "520");
	conf->load("editor.conf");

	E_DRIVER_TYPE driv = irr::video::EDT_OPENGL;

	const std::string confDriver = str_to_lower(conf->get("driver"));
	if (confDriver == "directx8") {
		driv = EDT_DIRECT3D8;
	} else if (confDriver == "directx9") {
		driv = EDT_DIRECT3D9;
	} else if (confDriver == "software") {
		driv = EDT_SOFTWARE;
	}

	// Start Irrlicht
	int w = conf->getInt("width");
	int h = conf->getInt("height");

	if (w < 1) w = 896;
	if (h < 1) h = 520;

	if (conf->getBool("vsync")) {
		std::cerr << "[WARNING] You have disabled vsync. Expect major CPU usage!" << std::endl;
	}
	irr::IrrlichtDevice* device = irr::createDevice(
		driv,
		irr::core::dimension2d<irr::u32>(w,h),
		16U,
		conf->getBool("fullscreen"),
		false,
		conf->getBool("vsync")
	);
	if (device == NULL) {
		return EXIT_FAILURE; // could not create selected driver.
	}

	// Editor
	Editor* editor = new Editor();
	editor->run(device, conf);

	conf->save("editor.conf");

	return 1;
}

// Define class independant functions
const wchar_t* convert(const char* input){
        #define MAXSIZE 1024
        size_t origsize = strlen(input) + 1;
        static wchar_t wcstring[MAXSIZE];
        mbstowcs(wcstring, input, origsize);
        wcscat(wcstring, L"");
        return wcstring;
        #undef MAXSIZE
}

void fillTB(IGUIElement* sidebar,int parentId,int id,float value){
	IGUIElement* e = sidebar->getElementFromId(parentId)->getElementFromId(id);

	if (e){
		IGUIEditBox* b = static_cast<IGUIEditBox*>(e);

		if (!b)
			return;

		b->setText(stringw(value).c_str());
	}
}
void fillTB(IGUIElement* sidebar,int parentId,int id,int value){
	IGUIElement* e = sidebar->getElementFromId(parentId)->getElementFromId(id);

	if (e){
		IGUIEditBox* b = static_cast<IGUIEditBox*>(e);

		if (!b)
			return;

		b->setText(stringw(value).c_str());
	}
}

void addBox(IGUIElement* parent,IGUIEnvironment* guienv, vector2di pos,int index,const wchar_t* label){
	guienv->addStaticText(label,rect<s32>(pos.X,pos.Y,pos.X+20,pos.Y+20),false,true,parent)->setNotClipped(true);
	guienv->addEditBox(L"",rect<s32>(pos.X+15,pos.Y,pos.X+200,pos.Y+20),true,parent,index)->setNotClipped(true);
}
void addXYZ(IGUIElement* parent,IGUIEnvironment* guienv, vector2di pos,int startIndex){
	addBox(parent,guienv,vector2di(pos.X,pos.Y),startIndex,L"X");   // 0,0
	addBox(parent,guienv,vector2di(pos.X,pos.Y+30),startIndex+1,L"Y");   // 80, 0
	addBox(parent,guienv,vector2di(pos.X,pos.Y+60),startIndex+2,L"Z");  // 160, 0
}
