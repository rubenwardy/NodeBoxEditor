#include "minetest.hpp"
#include "util/string.hpp"
#include "util/filesys.hpp"
#include "FileFormat/FileFormat.hpp"
#include "FileFormat/helpers.hpp"
#include <stdlib.h>
#include <fstream>

#ifdef _WIN32
	#include <direct.h>
	#include <process.h>
	#define getcwd _getcwd
	#define EXE_FILENAME "minetest.exe"
#else
	#include <unistd.h>
	#define EXE_FILENAME "minetest"
#endif

Minetest::Minetest(Configuration *conf):
	_conf(conf), minetest_dir(""), minetest_exe("")
{}

bool Minetest::findMinetestDir(std::string path)
{
	if (DirExists(path.c_str())) {
		std::cerr << "Minetest found at " << path.c_str() << std::endl;
		minetest_dir = path;
		std::string testpath = path + "bin" + DIR_DELIM + EXE_FILENAME;
		if (FileExists(testpath.c_str())) {
			minetest_exe = testpath;
		} else {
			std::cerr << "...but no executable!" << std::endl;
		}
		return true;
	}
	return false;
}

bool Minetest::findMinetest(bool editor_is_installed)
{
	std::cerr << "Searching for Minetest using minetest_root setting.." << std::endl;
	std::string path = _conf->get("minetest_root");
	path = cleanDirectoryPath(path);
	if (path != "") {
		if (findMinetestDir(path) && minetest_exe != "")
			return true;

		std::cerr << "Minetest not found in the path given, not trying anywhere else." << std::endl;
		return false;
	}

#ifndef _WIN32
	std::cerr << "Searching for Minetest in system-wide..." << std::endl;
	path = getenv("HOME");
	path += "/.minetest/";
	if (findMinetestDir(path)) {
		if (minetest_exe == "") {
			if (FileExists("/usr/local/bin/minetest")) {
				minetest_exe = "/usr/local/bin/minetest";
				return true;
			} else if (FileExists("/usr/bin/minetest")) {
				minetest_exe = "/usr/bin/minetest";
				return true;
			} else
				std::cerr << "~/.minetest/ exists, but no exe in /usr/bin/ or /usr/local/bin/" << std::endl;
		}
	}
#endif

	std::cerr << "Searching for Minetest relative to NBE save directory..." << std::endl;
	path = getSaveLoadDirectory(_conf->get("save_directory"), editor_is_installed);
	path = cleanDirectoryPath(path);

	// minetest/
	if (findMinetestDir(path + "minetest" + DIR_DELIM) && minetest_exe != "")
		return true;

	// ../minetest
	if (findMinetestDir(path + ".." + DIR_DELIM + "minetest" + DIR_DELIM) && minetest_exe != "")
		return true;

	// Minetest/
	if (findMinetestDir(path + "Minetest" + DIR_DELIM) && minetest_exe != "")
		return true;

	// ../Minetest/
	if (findMinetestDir(path + ".." + DIR_DELIM + "Minetest" + DIR_DELIM) && minetest_exe != "")
		return true;

	// games/minetest/
	if (findMinetestDir(path + "games" + DIR_DELIM + "minetest" + DIR_DELIM) && minetest_exe != "")
		return true;

	// ../games/minetest/
	if (findMinetestDir(path + ".." + DIR_DELIM + "games" + DIR_DELIM + "minetest" + DIR_DELIM) && minetest_exe != "")
		return true;

	// apps/minetest/
	if (findMinetestDir(path + "apps" + DIR_DELIM + "minetest" + DIR_DELIM) && minetest_exe != "")
		return true;

	// ../apps/minetest/
	if (findMinetestDir(path + ".." + DIR_DELIM + "apps" + DIR_DELIM + "minetest" + DIR_DELIM) && minetest_exe != "")
		return true;

	std::cerr << "Minetest not found!" << std::endl;
	return false;
}

bool Minetest::runMod(EditorState *state, const std::string &world)
{
	std::string worlddir = minetest_dir + "worlds" + DIR_DELIM + world + DIR_DELIM;
	std::string modname = state->project->name;
	if (DirExists(worlddir.c_str())) {
		// Open file
		const char *filename = (worlddir + "world.mt").c_str();
		std::ifstream file(filename);
		if (file) {
			std::string line;
			std::string search_for = std::string("load_mod_") + modname.c_str();
			bool found = false;
			while (std::getline(file, line)) {
				if (trim(line) == search_for) {
					found = true;
					break;
				}
			}
			file.close();

			if (!found) {
				std::ofstream outfile;
				outfile.open(filename, std::ios_base::app);
				outfile << "\n" << search_for << "\n";
				outfile.close();
			}
		} else {
			std::ofstream outfile(filename);
			outfile << "gameid = minetest\n";
			outfile << "backend = sqlite3\n";
			outfile << "load_mod_" << modname.c_str() << " = true\n";
			outfile.close();
		}
	} else {
		// Create new world folder
		CreateDir(worlddir);
		std::ofstream file((worlddir + "world.mt").c_str());
		file << "gameid = minetest\n";
		file << "backend = sqlite3\n";
		file << "load_mod_" << modname.c_str() << " = true\n";
		file.close();
	}

	// Create mod folder in world
	std::string cmd = worlddir + "worldmods";
	CreateDir(cmd);

	// Export Mod
	std::string mod_to = worlddir + "worldmods" + DIR_DELIM + modname;
	CreateDir(mod_to);
	mod_to = cleanDirectoryPath(mod_to);
	export_textures(mod_to + "textures" + DIR_DELIM, state);
	FileFormat *writer = getFromType(FILE_FORMAT_LUA, state);
	save_file(writer, state, mod_to + "init.lua");

	// Build argument list
	char *const argv[] = {
	    (char*)minetest_exe.c_str(),
	    "--worldname", (char*)world.c_str(),
	    "--name", "tester",
	    "--address", "",
	    "--go",
	    NULL
	};

#ifndef _WIN32
	pid_t pid = fork();
	if (pid == 0) {
		chdir(minetest_dir.c_str());
		std::cerr << "FORK: launching mt, exe = " << argv[0] << std::endl;
		execvp(argv[0], argv);
		std::cerr << "Couldn't run minetest!" << std::endl;
		_exit(0);
	} else if (pid < 0) {
		std::cerr << "Error!" << std::endl;
	}
#else
	// Change working directory
	char buffer[300];
	char *prev_cwd = getcwd(buffer, sizeof(buffer));
	if (!prev_cwd) {
		std::cerr << "Failed to get CWD" << std::endl;
		return false;
	}

	// Run minetest
	chdir(minetest_dir.c_str());
	std::cerr << "SPAWNVP: launching mt, exe = " << argv[0] << std::endl;
	_spawnvp(P_NOWAIT, argv[0], argv);
	chdir(prev_cwd);
#endif


	return true;
}
