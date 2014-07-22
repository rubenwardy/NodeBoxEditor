#ifndef FILEFORMAT_HPP_INCLUDED
#define FILEFORMAT_HPP_INCLUDED
#include <string>
#include "../Project.hpp"
#include "../EditorState.hpp"

enum FileFormatError
{
	EFFE_NONE,
	EFFE_READ_OLD_VERSION,
	EFFE_READ_NEW_VERSION,
	EFFE_READ_IO_ERROR,
	
};

enum FileFormatType
{
	FILE_FORMAT_NBE,
	FILE_FORMAT_LUA,
	FILE_FORMAT_MTC
};

class Project;
class EditorState;
class FileFormat
{
public:	
	FileFormat():
		error_code(EFFE_NONE)
	{}

	virtual Project * read(const std::string & filename) = 0; // Open from file	
	virtual bool write(Project *project, const std::string & filename) = 0; // Save to file
	virtual std::string getAsString(Project *project) { return ""; }
	virtual const char * getExtension() const = 0; // Get the main file extension
	FileFormatError error_code;
};

// FileFormat factory
extern FileFormat *getFromType(FileFormatType id, EditorState *st);

#endif

