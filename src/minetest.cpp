#include "minetest.hpp"
#include "util/string.hpp"
#include "util/filesys.hpp"
#include <stdlib.h>
#include <fstream>

#ifdef _WIN32
#define DIR_DELIM "\\"
#else
#define DIR_DELIM "/"
#endif

Minetest::Minetest(Configuration *conf):
	_conf(conf), minetest_dir(""), minetest_exe("")
{}

bool Minetest::findMinetest()
{
	std::cerr << "Searching for Minetest using minetest_root setting" << std::endl;
	std::string path = _conf->get("minetest_root");
	if (path != "") {
		if (PathExists(path.c_str())) {
			std::cerr << "Minetest found at " << path.c_str() << std::endl;
			minetest_dir = path;
			minetest_exe = minetest_dir + "bin/minetest";
	#ifdef _WIN32
			minetest_exe += ".exe";
	#endif
			if (!PathExists(minetest_exe.c_str()))
				std::cerr << "Error! exe missing from Minetest/bin" << std::endl;

			return true;
		}
	}

#ifndef _WIN32
	std::cerr << "Searching for Minetest in system-wide" << std::endl;
	path = getenv("HOME");
	path += "/.minetest/";

	if (PathExists(path.c_str())) {
		std::cerr << "Minetest found at " << path.c_str() << std::endl;
		minetest_dir = path;
		if (PathExists("/usr/local/bin/minetest"))
			minetest_exe = "/usr/local/bin/minetest";
		else
			minetest_exe = "/usr/bin/minetest";
		return true;
	}
#endif

	std::cerr << "Searching for Minetest relative to NBE save directory" << std::endl;
	path = getSaveLoadDirectory(_conf->get("save_directory"), _conf->getBool("installed"));
	if (PathExists((path + "../minetest/").c_str())) {
		std::cerr << "Minetest found at " << (path + "../minetest/").c_str() << std::endl;
		minetest_dir = path + "../minetest/";
		minetest_exe = minetest_dir + "bin/minetest";
#ifdef _WIN32
		minetest_exe += ".exe";
#endif
		if (!PathExists(minetest_exe.c_str()))
			std::cerr << "Error! exe missing from Minetest/bin" << std::endl;

		return true;
	}

	std::cerr << "Minetest not found!" << std::endl;
	return false;
}

bool Minetest::runMod(const std::string &modname, const std::string &modpath,
		const std::string &world, bool new_world)
{
	if (new_world) {
		// Delete old world folder
		std::string param = minetest_dir + DIR_DELIM "worlds" DIR_DELIM + world;
		remove(param.c_str());

		// Create new world folder
		CreateDir(param.c_str());

		// world.mt
		param = minetest_dir + DIR_DELIM "worlds" DIR_DELIM + world + DIR_DELIM "world.mt";
		std::ofstream file(param.c_str());
		file << "gameid = minetest\n";
		file << "backend = sqlite3\n";
		file << "load_mod_" << modname.c_str() << " = true\n";
		file.close();
	}

	// Create mod folder in world
	std::string cmd = minetest_dir + DIR_DELIM  "worlds" DIR_DELIM + world + DIR_DELIM "worldmods";
	CreateDir(cmd);

	// Install mod
	cmd = minetest_dir + DIR_DELIM "worlds" DIR_DELIM + world + DIR_DELIM "worldmods" DIR_DELIM + modname;
	rename(modpath.c_str(), cmd.c_str());

	// Run minetest
	std::string exec = minetest_exe;
	exec = " --worldname " + world + " --go";
	system(exec.c_str());
	return true;
}
