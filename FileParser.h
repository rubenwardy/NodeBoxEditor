#ifndef _FILEPARSER_H_INCLUDED_
#define _FILEPARSER_H_INCLUDED_
#include "common.h"
#include "Project.h"
#include "EditorState.h"

class FileParser
{
public:
	virtual void save(Project* project,irr::core::stringc file) = 0;
	virtual Project* open(irr::core::stringc file) = 0;
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
private:
	EditorState* state;
};

#endif