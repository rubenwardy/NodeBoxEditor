#ifndef UTIL_FILESYS_HPP_INCLUDED
#define UTIL_FILESYS_HPP_INCLUDED
#include "string.hpp"
#include <vector>

#ifdef _WIN32
#define DIR_DELIM '\\'
#else
#define DIR_DELIM '/'
#endif

std::string getSaveLoadDirectory(std::string save_dir_setting, bool editor_is_installed);

std::string cleanDirectoryPath(std::string &path);

std::string getTmpDirectory(bool editor_is_installed);

bool FileExists(const char* path);
bool DirExists(const char* path);

bool CreateDir(std::string path);

std::vector<std::string> filesInDirectory(std::string path);

std::string filenameWithExt(std::string path);
std::string extFromFilename(std::string path);
std::string filenameWithoutExt(std::string path);
std::string pathWithoutFilename(std::string path);


#endif
