#include "SimpleFileCombiner.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

std::vector<char> ReadAllBytes(char const* filename)
{
	std::ifstream ifs(filename, std::ios::binary|std::ios::ate);

	if (!ifs) {
		std::cerr << "Error! Unable to open file '" << filename << "' in SimpleFileCombiner/ReadAllBytes" << std::endl;
		return std::vector<char>(0);
	}

	std::ifstream::pos_type pos = ifs.tellg();

	std::vector<char> result(pos);

	ifs.seekg(0, std::ios::beg);
	ifs.read(&result[0], pos);

	return result;
}

bool SimpleFileCombiner::write(std::string filename) {
	std::ofstream output(filename.c_str(), std::ios::binary|std::ios::out);
	if (!output) {
		errcode = EERR_IO;
		return false;	
	}
	output.write("NBEFP", 5);
	output << (char)files.size();
	unsigned int start = files.size() * sizeofdef + 6;
	for (std::list<SimpleFileCombiner::File>::const_iterator it = files.begin();
			it != files.end();
			++it) {
		SimpleFileCombiner::File file = *it;
		std::string name = file.name;
		unsigned int size = file.bytes.size();
		std::cerr << "(SFC) Writing " << name.c_str() << ": " << start << " (" << size << ")" << std::endl;
		while (name.size() < 50) {
			name += " ";
		}
		output << name.c_str();
		output.write(static_cast<char*>(static_cast<void*>(&start)), sizeof(unsigned int));	
		output.write(static_cast<char*>(static_cast<void*>(&size)), sizeof(unsigned int));
		start += size;
			
	}		
	for (std::list<SimpleFileCombiner::File>::const_iterator it = files.begin();
			it != files.end();
			++it) {
		SimpleFileCombiner::File file = *it;
		output.write(&file.bytes[0], file.bytes.size());
	}		
	output.close();
	return true;
}
bool SimpleFileCombiner::add(const char* readfrom, std::string file)
{
	files.push_back(File(file, ReadAllBytes(readfrom)));
	return true;
}
std::list<std::string> SimpleFileCombiner::read(const char* file, std::string dir)
{
	// Start reading
	std::ifstream ifs(file, std::ios::binary|std::ios::ate);
	if (!ifs) {
		errcode = EERR_IO;
		return std::list<std::string>();
	}

	std::string start(5, '\0');
	ifs.seekg(0, std::ios::beg);
	ifs.read(static_cast<char*>(static_cast<void*>(&start[0])), 5);
	if (start != "NBEFP") {
		errcode = EERR_WRONG_FILE;
		return std::list<std::string>();
	}

	// Read header
	char amount = 0;
	ifs.seekg(5, std::ios::beg);
	ifs.read(&amount, 1);
	std::list<std::string> result;

	// Loop through files
	for (int f = 0; f < (int)amount; f++) {
		std::string name(50, '\0');
		ifs.seekg(f * sizeofdef + 6, std::ios::beg);
		ifs.read(static_cast<char*>(static_cast<void*>(&name[0])), 50);
		name = trim(name);
		result.push_back(name);

		// Get start location
		unsigned int start = 0;
		ifs.seekg(f * sizeofdef + 56, std::ios::beg);
		ifs.read(static_cast<char*>(static_cast<void*>(&start)), sizeof(unsigned int));

		// Get size
		unsigned int size = 0;
		ifs.seekg(f * sizeofdef + 60, std::ios::beg);
		ifs.read(static_cast<char*>(static_cast<void*>(&size)), sizeof(unsigned int));
		std::cerr << "(SFC) Reading " << name.c_str() << ": " << start << " (" << size << ")" << std::endl;

		// Read and save data
		std::string data(size, '\0');
		ifs.seekg(start, std::ios::beg);
		ifs.read(static_cast<char*>(static_cast<void*>(&data[0])), size);
		std::ofstream output((dir + "/" + name).c_str(), std::ios::binary|std::ios::out);
		output.write(static_cast<char*>(static_cast<void*>(&data[0])), size);
		output.close();
	}
	return result;
}
