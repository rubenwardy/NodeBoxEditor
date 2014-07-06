#ifndef UTIL_FILESYS_HPP_INCLUDED
#define UTIL_FILESYS_HPP_INCLUDED
#include "string.hpp"

// Get the path of the directory to save projects to
std::string getSaveLoadDirectory(std::string save_dir_setting, bool editor_is_installed);

// Determine whether a path exists
bool PathExists(const char* path);

#endif
