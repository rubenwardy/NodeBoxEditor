#ifndef CPPFILEFORMAT_HPP_INCLUDED
#define CPPFILEFORMAT_HPP_INCLUDED

#include "FileFormat.hpp"

class FileFormat;
class CPPFileFormat : public FileFormat
{
public:
	CPPFileFormat(EditorState* st) : state(st) {}
	virtual bool write(Project* project, const std::string & filename);
	virtual Project * read(const std::string & file, Project *project=NULL);

	virtual const char * getExtension() const {
		return "cpp";
	}
private:
	EditorState* state;
};

#endif
