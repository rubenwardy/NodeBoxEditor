#include "SimpleFileCombiner.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

std::vector<char> ReadAllBytes(char const* filename)
{
	std::ifstream ifs(filename, std::ios::binary|std::ios::ate);

	if (!ifs) {
		std::cerr << "Error! Unable to open file '" << filename << std::endl << "' (util/SimpleFileCombiner.cpp::ReadAllBytes)";
		return std::vector<char>(0);
	}

	std::ifstream::pos_type pos = ifs.tellg();

	std::vector<char> result(pos);

	ifs.seekg(0, std::ios::beg);
	ifs.read(&result[0], pos);

	return result;
}

bool SimpleFileCombiner::write(std::string filename) {
	std::cerr << std::endl << "===saving===" << std::endl;
	std::ofstream output(filename.c_str(), std::ios::binary|std::ios::out);
	output << (char)files.size();
	unsigned int start = files.size() * sizeofdef + 1;
	std::cerr << "Adding " << files.size() << " files" << std::endl;
	for (std::list<SimpleFileCombiner::File>::const_iterator it = files.begin();
			it != files.end();
			++it) {
		SimpleFileCombiner::File file = *it;
		std::string name = file.name;
		while (name.size() < 50) {
			name += " ";
		}
		output << name.c_str();
		unsigned int size = file.bytes.size();
		output.write(static_cast<char*>(static_cast<void*>(&start)), sizeof(unsigned int));	
		output.write(static_cast<char*>(static_cast<void*>(&size)), sizeof(unsigned int));
		std::cerr << trim(name) << " - start: " << start << ", size: " << size << std::endl;
		start += file.bytes.size();		
	}		
	for (std::list<SimpleFileCombiner::File>::const_iterator it = files.begin();
			it != files.end();
			++it) {
		SimpleFileCombiner::File file = *it;
		output.write(&file.bytes[0], file.bytes.size());
	}		
	output.close();
	std::cerr << "==saved==" << std::endl;
	return true;
}
bool SimpleFileCombiner::add(const char* readfrom, std::string file)
{
	files.push_back(File(file, ReadAllBytes(readfrom)));
	return true;
}
std::list<std::string> SimpleFileCombiner::read(const char* file, std::string dir)
{
	std::cerr << "reading..." << std::endl;
	// Start reading
	std::ifstream ifs(file, std::ios::binary|std::ios::ate);
	if (!ifs)
		return std::list<std::string>();

	// Read header
	char amount = 0;
	ifs.seekg(0, std::ios::beg);
	ifs.read(&amount, 1);
	std::list<std::string> result;

	// Loop through files
	for (int f = 0; f < (int)amount; f++) {
		std::string name(50, '\0');
		ifs.seekg(f * sizeofdef + 1, std::ios::beg);
		ifs.read(static_cast<char*>(static_cast<void*>(&name[0])), 50);
		name = trim(name);
		result.push_back(name);

		// Get start location
		unsigned int start = 0;
		ifs.seekg(f * sizeofdef + 51, std::ios::beg);
		ifs.read(static_cast<char*>(static_cast<void*>(&start)), sizeof(unsigned int));

		// Get size
		unsigned int size = 0;
		ifs.seekg(f * sizeofdef + 55, std::ios::beg);
		ifs.read(static_cast<char*>(static_cast<void*>(&size)), sizeof(unsigned int));

		// Read and save data
		ifs.seekg(start, std::ios::beg);
		char data[size];
		ifs.read(static_cast<char*>(static_cast<void*>(&data)), size);
		std::ofstream output((dir + "/" + name).c_str(), std::ios::binary|std::ios::out);
		output.write(data, size);
		output.close();
	}
	return result;
}
