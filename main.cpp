#include <irrlicht.h>
#include "common.h"
#include "Editor.h"

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#ifndef _DEBUG
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif
#endif

const wchar_t* convert(const char* input){
        #define MAXSIZE 1024
        size_t origsize = strlen(input) + 1;
        static wchar_t wcstring[MAXSIZE];
        mbstowcs(wcstring, input, origsize);
        wcscat(wcstring, L"");
        return wcstring;
        #undef MAXSIZE
}

int main(){
	printf(
		" _   _           _        ____              _____    _ _ _             \n"
		"| \\ | | ___   __| | ___  | __ )  _____  __ | ____|__| (_) |_ ___  _ __ \n"
		"|  \\| |/ _ \\ / _` |/ _ \\ |  _ \\ / _ \\ \\/ / |  _| / _` | | __/ _ \\| '__|\n"
		"| |\\  | (_) | (_| |  __/ | |_) | (_) >  <  | |__| (_| | | || (_) | |   \n"
		"|_| \\_|\\___/ \\__,_|\\___| |____/ \\___/_/\\_\\ |_____\\__,_|_|\\__\\___/|_|   \n\n"		
	);

	// Settings
	Configuration* conf = new Configuration();
	if (!conf)
		return 0;

	// Init Settings
	conf->setStringSetting("snapping","true");
	conf->setStringSetting("limiting","true");
	conf->setStringSetting("driver","opengl");
	conf->setStringSetting("hide_sidebar","false");
	conf->setStringSetting("always_show_position_handle","false");
	conf->setStringSetting("dont_use_vsync","false");
	conf->setStringSetting("fullscreen","false");
	conf->setStringSetting("width","896");
	conf->setStringSetting("height","520");
	conf->load("editor.conf");

	E_DRIVER_TYPE driv = irr::video::EDT_OPENGL;

	if (conf->getSettingAsString("driver")->equals_ignore_case("directx8")){
		driv = EDT_DIRECT3D8;	
	}else if (conf->getSettingAsString("driver")->equals_ignore_case("directx9")){
		driv = EDT_DIRECT3D9;
	}else if (conf->getSettingAsString("driver")->equals_ignore_case("software")){
		driv = EDT_SOFTWARE;	
	}
	
	// Start Irrlicht
	int w = conf->getSettingAsInt("width");
	int h = conf->getSettingAsInt("height");

	if (w==0)
		w=896;
	if (h==0)
		h=520;

	if (conf->getSettingAsBool("dont_use_vsync"))
		printf("[WARNING] You have disabled vsync, so expect major CPU usage!\n");

    irr::IrrlichtDevice* device = irr::createDevice(
		driv,
		irr::core::dimension2d<irr::u32>(w,h),
		16U,
		conf->getSettingAsBool("fullscreen"),
		false,
		!conf->getSettingAsBool("dont_use_vsync")
	);
    if (device == 0)
        return 0; // could not create selected driver.

	// Editor
	Editor* editor = new Editor();
	editor->run(device,conf);

	conf->save("editor.conf");

    return 1;
}
