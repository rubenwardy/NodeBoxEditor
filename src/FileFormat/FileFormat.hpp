#ifndef FILEFORMAT_HPP_INCLUDED
#define FILEFORMAT_HPP_INCLUDED
#include <string>
#include "../Project.hpp"
#include "../EditorState.hpp"

enum FileFormatError
{
	EFFE_NONE, // No error reported
 	EFFE_IO_ERROR, // Problem opening file
	EFFE_READ_OLD_VERSION, // File is too old for this version of NBE
	EFFE_READ_NEW_VERSION, // File is newer than this version of NBE
	EFFE_READ_PARSE_ERROR, // Corruption or issue in reading file
	EFFE_READ_WRONG_TYPE // Wrong type of file
};

enum FileFormatType
{
	FILE_FORMAT_NBE,
	FILE_FORMAT_LUA,
	FILE_FORMAT_CPP
};

class Project;
class EditorState;
class FileFormat
{
public:
	FileFormat():
		error_code(EFFE_NONE)
	{}

	virtual Project * read(const std::string &filename, Project *project=NULL) = 0; // Open from file
	virtual bool write(Project *project, const std::string & filename) = 0; // Save to file
	virtual std::string getAsString(Project *project) { return ""; }
	virtual const char * getExtension() const = 0; // Get the main file extension
	FileFormatError error_code;
};

// FileFormat factory
extern FileFormat *getFromType(FileFormatType id, EditorState *st);
extern FileFormat *getFromExt(std::string path, EditorState *st);

#endif
