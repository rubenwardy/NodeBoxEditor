#ifndef UTIL_FILESYS_HPP_INCLUDED
#define UTIL_FILESYS_HPP_INCLUDED
#include "string.hpp"

std::string getSaveLoadDirectory(std::string save_dir_setting, bool editor_is_installed);

std::string getTmpDirectory(bool editor_is_installed);

bool PathExists(const char* path);

bool CreateDir(std::string path);

#endif
