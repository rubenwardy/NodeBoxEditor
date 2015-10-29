#include "filesys.hpp"
#include <iostream>
#include "string.hpp"
#include <stdlib.h>

std::string cleanDirectoryPath(std::string &path)
{
	if (path == "")
		return "";

	std::string r(path);
	if (*r.rbegin() != '/')
		r += "/";
	return r;
}

std::string getSaveLoadDirectory(std::string save_dir_setting, bool editor_is_installed)
{
	std::string dir = save_dir_setting;

#ifndef _WIN32
	if (dir == "" && editor_is_installed) {
		dir = getenv("HOME");
	}
#endif

	return cleanDirectoryPath(dir);
}

std::string getTmpDirectory(bool editor_is_installed)
{
#ifndef _WIN32
	if (editor_is_installed) {
		std::string res = std::string(getenv("HOME")) + "/.nbetmp/";
		std::cerr << "Tmpdir requested. Gave " << res.c_str() << std::endl;
		return res;
	}
#endif
	std::cerr << "Tmpdir requested. Gave .tmp/" << std::endl;
	return ".tmp/";
}

// This code was nicked from Minetest, subject to LGPLv2
// See http://minetest.net
#ifdef _WIN32
#include <windows.h>

bool FileExists(const char* path)
{
	DWORD dwAttrib = GetFileAttributesA(path);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool DirExists(const char* path)
{
	DWORD ftyp = GetFileAttributesA(path);
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;

	return false;
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

std::vector<std::string> filesInDirectory(std::string path)
{
	std::vector<std::string> res;
	res.push_back("No file browser for Windows yet :/");
	return res;
}

#else
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>

bool FileExists(const char* path)
{
	struct stat st;
	if (stat(path, &st) == -1)
		return false;
	return S_ISREG(st.st_mode);
}

bool DirExists(const char* path)
{
	struct stat st;
	if (stat(path, &st) == -1)
		return false;
	return S_ISDIR(st.st_mode);
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

std::vector<std::string> filesInDirectory(std::string path)
{
	std::vector<std::string> res;
	if (path == "")
		path = ".";
	DIR *dirp = opendir(path.c_str());
	if (!dirp) {
		std::cerr << "Failed to open directory" << std::endl;
		return std::vector<std::string>();
	}
	while (dirent *dp = readdir(dirp)) {
		res.push_back(std::string(dp->d_name));
	}
	(void)closedir(dirp);
	return res;
}

#endif


std::string filenameWithExt(std::string path)
{
	size_t pos = str_replace(path, '\\', '/').find_last_of("/");
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


std::string extFromFilename(std::string path)
{
	std::string res = filenameWithExt(path);
	size_t pos = res.find_last_of(".");
	if (pos > res.size() || pos < 0)
		return "";
	return res.substr(pos + 1, res.size());
}

std::string pathWithoutFilename(std::string path)
{
	size_t pos = str_replace(path, '\\', '/').find_last_of("/");
	if (pos >= path.size() || pos < 0)
		return "";

	return str_replace(str_replace(path.substr(0, pos), '\\', DIR_DELIM), '/', DIR_DELIM);
}
