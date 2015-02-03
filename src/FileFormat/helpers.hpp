#ifndef FILEFORMAT_HELPERS_HPP_INCLUDED
#define FILEFORMAT_HELPERS_HPP_INCLUDED
#include "../common.hpp"
#include "FileFormat.hpp"

void save_file(FileFormat *writer, EditorState *state, std::string file);

void export_textures(std::string dir, EditorState *state);

#endif
