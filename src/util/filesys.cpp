#include "filesys.hpp"
#include <iostream>
#include "string.hpp"
#include <stdlib.h>

std::string getSaveLoadDirectory(std::string save_dir_setting, bool editor_is_installed)
{
	std::string dir = save_dir_setting;

#ifndef _WIN32
	if (dir == "" && editor_is_installed) {
		dir = getenv("HOME");
	}
#endif

	if (dir.length() != 0) {
		size_t pos = dir.find_last_of("/");
		if(pos != dir.length() - 1) {
			dir += "/";
		}
	}
	std::cerr << "Returning " << dir.c_str() << std::endl;
	return dir;
}

// This code was nicked from Minetest, subject to LGPLv2
// See http://minetest.net
#ifdef _WIN32
#include <windows.h>

bool PathExists(const char* path)
{
	return (GetFileAttributes(path) != INVALID_FILE_ATTRIBUTES);
}

#else
#include <sys/stat.h>

bool PathExists(const char* path)
{
	struct stat st;
	return (stat(path, &st) == 0);
}

#endif
