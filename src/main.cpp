#include <stdlib.h>
#include <iostream>
#include <irrlicht.h>
#include "util/string.hpp"
#include "util/filesys.hpp"
#include "common.hpp"
#include "Editor.hpp"

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#ifndef _DEBUG
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif
#endif

#ifndef _WIN32
#include <stdlib.h>
#include <unistd.h>

void findWorkingDirectory(bool &editor_is_installed)
{
	std::cerr << "Looking for the working directory..." << std::endl;
	if (FileExists("media/sky.jpg"))
		return;

	chdir("../");
	if (FileExists("media/sky.jpg"))
		return;

	chdir("share/nodeboxeditor");
	if (FileExists("media/sky.jpg")) {
		std::cerr << "Is installed!";
		editor_is_installed = true;
		return;
	}

	chdir("/usr/local/share/nodeboxeditor");
	if (FileExists("media/sky.jpg")) {
		std::cerr << "Is installed!";
		editor_is_installed = true;
		return;
	}

	char buff[PATH_MAX];
	ssize_t len = ::readlink("/proc/self/exe", buff, sizeof(buff) - 1);
	if (len != -1) {
		buff[len] = '\0';
		std::string path(buff);
		path = pathWithoutFilename(path);
		std::cerr << path << std::endl;
		chdir(path.c_str());
		if (FileExists("media/sky.jpg"))
			return;

		chdir("../");
		if (FileExists("media/sky.jpg"))
			return;
	}

	std::cerr << "Can't find the working directory!" << std::endl;
}
#endif // ifndef _WIN32

int main(int argc, char *argv[]) {
	std::cerr <<
		" _   _           _        ____              _____    _ _ _             \n"
		"| \\ | | ___   __| | ___  | __ )  _____  __ | ____|__| (_) |_ ___  _ __ \n"
		"|  \\| |/ _ \\ / _` |/ _ \\ |  _ \\ / _ \\ \\/ / |  _| / _` | | __/ _ \\| '__|\n"
		"| |\\  | (_) | (_| |  __/ | |_) | (_) >  <  | |__| (_| | | || (_) | |   \n"
		"|_| \\_|\\___/ \\__,_|\\___| |____/ \\___/_/\\_\\ |_____\\__,_|_|\\__\\___/|_|   \n\n"
		<< std::endl;

#ifdef _DEBUG
	std::cerr << "Debug mode enabled!" << std::endl;
#endif


	// Find the working directory
	bool editor_is_installed = false;
#ifndef _WIN32
	findWorkingDirectory(editor_is_installed);
#endif

	// Settings
	Configuration* conf = new Configuration();
	if (conf == NULL) {
		return EXIT_FAILURE;
	}
	conf->set("snapping", "true");
	conf->set("limiting", "true");
	conf->set("driver", "opengl");
	conf->set("hide_sidebar", "false");
	conf->set("save_directory", "");
	conf->set("minetest_root", "");
	conf->set("always_show_position_handle", "false");
#ifdef _WIN32
	conf->set("vsync", "false");
	conf->set("use_sleep", "true");
#else
	conf->set("vsync", "true");
	conf->set("use_sleep", "false");
#endif
	conf->set("viewport_top_left", "pers");
	conf->set("viewport_top_right", "top");
	conf->set("viewport_bottom_left", "front");
	conf->set("viewport_bottom_right", "right");
	conf->set("lighting", "2");
	conf->set("hide_other_nodes", "true");
	conf->set("no_negative_node_y", "true");
	conf->set("fullscreen", "false");
	conf->set("width", "896");
	conf->set("height", "520");
	if (!editor_is_installed)
		conf->load("editor.conf");
	else
		if (!conf->load(std::string(getSaveLoadDirectory("", true)) + ".config/nodeboxeditor.conf"))
			conf->load("editor.conf");

	// Set up irrlicht device
	E_DRIVER_TYPE driv = irr::video::EDT_OPENGL;

	const std::string confDriver = str_to_lower(conf->get("driver"));
#ifdef _IRR_COMPILE_WITH_DIRECT3D_8_
	if (confDriver == "directx8") {
		driv = EDT_DIRECT3D8;
	} else
#endif
#ifdef _IRR_COMPILE_WITH_DIRECT3D_9_
	if (confDriver == "directx9") {
		driv = EDT_DIRECT3D9;
	} else
#endif
	if (confDriver == "software") {
		driv = EDT_SOFTWARE;
	}

	// Start Irrlicht
	int w = conf->getInt("width");
	int h = conf->getInt("height");
	if (w < 1) w = 896;
	if (h < 1) h = 520;

	if (!conf->getBool("vsync")) {
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

#if IRRLICHT_VERSION_MAJOR == 1 && IRRLICHT_VERSION_MINOR < 8
	std::cerr << "Warning! Your Irrlicht version is outdated, so some NBE features will not be available." << std::endl;
#endif

	// Editor
	Editor* editor = new Editor();
	editor->run(device, conf, editor_is_installed);

	if (!editor_is_installed)
		conf->save("editor.conf");
	else
		if (!conf->save(std::string(getSaveLoadDirectory("", true)) + ".config/nodeboxeditor.conf"))
			conf->save("editor.conf");

	return 1;
}

#ifndef _WIN32
// Fix for Inconsistency detected by ld.so
#include <pthread.h>
void junk() {
	int i;
	i = pthread_getconcurrency();
};
#endif
