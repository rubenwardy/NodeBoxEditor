#ifndef FILEFORMAT_HPP_INCLUDED
#define FILEFORMAT_HPP_INCLUDED

#include <string>

class Project;
class EditorState;

class FileFormat
{
public:
	// Open from file
	virtual Project * read(const std::string & filename) = 0;
	// Save to file
	virtual bool write(Project * project, const std::string & filename) = 0;

	// Get the main file extension
	virtual const char * getExtension() const = 0;
};

enum FileFormatType {
	FILE_FORMAT_NBE,
	FILE_FORMAT_LUA,
	FILE_FORMAT_MTC
};

// FileFormat factory
extern FileFormat *getFromType(FileFormatType id, EditorState *st);

#endif
