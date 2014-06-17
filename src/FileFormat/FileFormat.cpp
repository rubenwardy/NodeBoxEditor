#include "FileFormat.hpp"
#include "NBE.hpp"
#include "Lua.hpp"
#include "MTC.hpp"

FileFormat *getFromType(FileFormatType id, EditorState *st)
{
	switch (id) {
	case FILE_FORMAT_NBE:
		return new NBEFileFormat(st);
	case FILE_FORMAT_LUA:
		return new LuaFileFormat(st);
	case FILE_FORMAT_MTC:
		return new MTCFileFormat(st);
	}
	return NULL;
}
	
