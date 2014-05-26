#include "FileFormat.hpp"
#include "NBE.hpp"
#include "Lua.hpp"

FileFormat *getFromType(FileFormatType id, EditorState *st)
{
	switch (id) {
	case FILE_FORMAT_NBE:
		return new NBEFileFormat(st);
	case FILE_FORMAT_LUA:
		return new LuaFileFormat(st);
	}
	return NULL;
}
	
