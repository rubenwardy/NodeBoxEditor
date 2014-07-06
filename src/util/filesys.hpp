#ifndef UTIL_FILESYS_HPP_INCLUDED
#define UTIL_FILESYS_HPP_INCLUDED

// Get the path of the directory to save projects to
const char* getSaveLoadDirectory(const char* save_dir_setting, bool editor_is_installed);

// Determine whether a path exists
bool PathExists(const char* path);

#endif
