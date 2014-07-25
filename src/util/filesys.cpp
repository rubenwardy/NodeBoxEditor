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
	return dir;
}

std::string getTmpDirectory(bool editor_is_installed)
{
#ifndef _WIN32
	if (editor_is_installed) {
		return std::string(getenv("HOME")) + "/.nbetmp/";	
	}
#endif
	return "tmp/";
}

// This code was nicked from Minetest, subject to LGPLv2
// See http://minetest.net
#ifdef _WIN32
#include <windows.h>

bool PathExists(const char* path)
{
	return (GetFileAttributes(path) != INVALID_FILE_ATTRIBUTES);
}

bool CreateDir(std::string path)
{
	bool r = CreateDirectory(path.c_str(), NULL);
	if(r == true)
		return true;
	if(GetLastError() == ERROR_ALREADY_EXISTS)
		return true;
	return false;
}
#else
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

bool PathExists(const char* path)
{
	struct stat st;
	return (stat(path, &st) == 0);
}

bool CreateDir(std::string path)
{
	int r = mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	if(r == 0) {
		return true;
	} else {
		// If already exists, return true
		if(errno == EEXIST)
			return true;
		return false;
	}	
}

#endif


std::string filenameWithExt(std::string path)
{
	size_t pos = path.find_last_of(DIR_DELIM);
	if (pos >= path.size() || pos < 0)
		return path;
	
	return path.substr(pos + 1);	
}

std::string filenameWithoutExt(std::string path)
{
	std::string res = filenameWithExt(path);
	size_t pos = res.find_last_of(".");	
	if (pos > res.size() || pos < 0)
		return res;
	return res.substr(0, pos);
}
