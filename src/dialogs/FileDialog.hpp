#ifndef FILEDIALOG_HPP_INCLUDED
#define FILEDIALOG_HPP_INCLUDED
#include "../common.hpp"
#include "../EditorState.hpp"

extern void FileDialog_show(EditorState *state, FileParserType type,
		const wchar_t* title, const wchar_t* button);

#endif
