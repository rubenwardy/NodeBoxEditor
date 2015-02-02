#ifndef SIMPLEFILECOMBINER_HPP_INCLUDED
#define SIMPLEFILECOMBINER_HPP_INCLUDED

#include "string.hpp"
#include <list>
#include <vector>



class SimpleFileCombiner
{
public:
	enum Errors
	{
		EERR_NONE = 0,
		EERR_IO,
		EERR_WRONG_FILE
	};

	SimpleFileCombiner():
		errcode(EERR_NONE)
	{}

	static const unsigned int sizeofdef = 50 + 2 * sizeof(unsigned int);
	class File
	{
	public:
		File(std::string tname, std::vector<char> tbytes):
			name(tname),
			bytes(tbytes){}
		std::string name;
		std::vector<char> bytes;
	};
	std::list<SimpleFileCombiner::File> files;
	bool write(std::string filename);
	bool add(const char* readfrom, std::string file);
	std::list<std::string> read(const char* file, std::string dir);
	SimpleFileCombiner::Errors errcode;
};

#endif
