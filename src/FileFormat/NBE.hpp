#ifndef NBEFILEFORMAT_HPP_INCLUDED
#define NBEFILEFORMAT_HPP_INCLUDED

#include "FileFormat.hpp"
#include "../Node.hpp"

class NBEFileFormat : public FileFormat
{
public:
	NBEFileFormat(EditorState *st) :
		state(st),
		node(NULL),
		stage(READ_STAGE_ROOT)
	{}
	virtual Project *read(const std::string &filename, Project *project=NULL);
	virtual bool write(Project *project, const std::string &filename);
	virtual const char *getExtension() const {
		return "nbe";
	}
	enum readstage
	{
		READ_STAGE_ROOT,
		READ_STAGE_NODE
	};
private:
	readstage stage;
	Node *node;
	EditorState *state;
	bool merging;
	bool readProjectFile(Project *project, const std::string &filename);
	bool writeProjectFile(Project *project, const std::string &filename);
	void parseLine(Project *project, std::string &line);
};

#endif
