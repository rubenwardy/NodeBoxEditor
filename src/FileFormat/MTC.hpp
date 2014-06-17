#ifndef MTCFILEFORMAT_HPP_INCLUDED
#define MTCFILEFORMAT_HPP_INCLUDED

#include "FileFormat.hpp"

class FileFormat;
class MTCFileFormat : public FileFormat
{
public:
	MTCFileFormat(EditorState* st) : state(st) {}
	virtual bool write(Project* project, const std::string & filename);
	virtual Project * read(const std::string & file);

	virtual const char * getExtension() const {
		return "cpp";
	}
private:
	EditorState* state;
};

#endif
