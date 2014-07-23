#ifndef UTIL_FILESYS_HPP_INCLUDED
#define UTIL_FILESYS_HPP_INCLUDED
#include "string.hpp"

#ifdef _WIN32
#define DIR_DELIM "\\"
#else
#define DIR_DELIM "/"
#endif

std::string getSaveLoadDirectory(std::string save_dir_setting, bool editor_is_installed);

std::string getTmpDirectory(bool editor_is_installed);

bool PathExists(const char* path);

bool CreateDir(std::string path);

std::string filenameWithExt(std::string path);
std::string filenameWithoutExt(std::string path);

#endif
