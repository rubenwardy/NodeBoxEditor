#ifndef LUAFILEFORMAT_HPP_INCLUDED
#define LUAFILEFORMAT_HPP_INCLUDED

#include "FileFormat.hpp"

class LuaFileFormat : public FileFormat
{
public:
	LuaFileFormat(EditorState* st) : state(st) {}
	virtual bool write(Project* project, const std::string & filename);
	virtual std::string getAsString(Project *project);
	virtual Project * read(const std::string & file);

	const char * getExtension() const {
		return "lua";
	}
private:
	EditorState* state;
};

#endif
