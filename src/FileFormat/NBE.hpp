#ifndef NBEFILEFORMAT_HPP_INCLUDED
#define NBEFILEFORMAT_HPP_INCLUDED

#include "FileFormat.hpp"
#include "../Node.hpp"

class NBEFileFormat : public FileFormat
{
public:
	NBEFileFormat(EditorState * st) :
		state(st),
		node(NULL),
		stage(NBEFileFormat::READ_STAGE_ROOT)
	{}
	virtual Project * read(const std::string & filename);
	virtual bool write(Project * project, const std::string & filename);

	enum readstage
	{
		READ_STAGE_ROOT,
		READ_STAGE_NODE
	};

	const char * getExtension() const {
		return "nbe";
	}

private:
	readstage stage;
	Node * node;
	EditorState* state;

	void parseLine(Project * project, std::string & line);
};

#endif
