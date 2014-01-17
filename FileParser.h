#ifndef _FILEPARSER_H_INCLUDED_
#define _FILEPARSER_H_INCLUDED_
#include "common.h"
#include "Project.h"
#include "EditorState.h"

class FileParser
{
public:
	// Save to file
	virtual void save(Project* project,irr::core::stringc file) = 0;

	// Open from file
	virtual Project* open(irr::core::stringc file) = 0;

	// Get the main file extension
	virtual const char* getEXT() const = 0;
};

class EditorState;
class NBEFileParser:public FileParser
{
public:
	NBEFileParser(EditorState* sta):state(sta),proj(NULL),node(NULL),stage(NBEFileParser::ERS_ROOT){}
	virtual void save(Project* project,irr::core::stringc file);
	virtual Project* open(irr::core::stringc file);

	enum readstage
	{
		ERS_ROOT = 1,
		ERS_NODE = 2
	};

	const char* getEXT() const{
		return ".nbe";
	}
private:
	readstage stage;
	Node* node;
	Project* proj;
	EditorState* state;
	void parseLine(stringc line);
};

class LUAFileParser:public FileParser
{
public:
	LUAFileParser(EditorState* sta):state(sta){}
	virtual void save(Project* project,irr::core::stringc file);
	virtual Project* open(irr::core::stringc file){
		printf("[WARNING] The lua importer has not been written yet!\n");
		return NULL;
	}

	const char* getEXT() const{
		return ".lua";
	}
private:
	EditorState* state;
};

// Parser factory
static FileParser* getFromType(unsigned int id,EditorState* sta){
	switch (id){
	case(0):
		return new NBEFileParser(sta);
	case(1):
		return new LUAFileParser(sta);
	}
	return NULL;
}	

#endif