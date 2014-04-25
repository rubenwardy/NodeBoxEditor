#include <fstream>
#include <string>
#include <stdlib.h>
#include "Configuration.hpp"
#include "util/string.hpp"


bool Configuration::load(const std::string & filename){
	std::string line;
	std::ifstream file(filename.c_str());
	if (!file) {
		return false;
	}
	while (std::getline(file, line)) {
		readLine(line);
	}
	file.close();
	return true;
}


void Configuration::readLine(std::string & line){
	// Skip comments
	if (line[0] == '#')
		return;
	line = trim(line);

	if (line.empty()) {
		return;
	}

	// Split string into hash and value per
	size_t eqPos = line.find('=');
	if (eqPos == std::string::npos) {
		return;
	}
	const std::string key = trim(line.substr(0, eqPos));
	const std::string value = trim(line.substr(eqPos + 1));

	// Create setting
	settings[key] = value;
}


bool Configuration::save(const std::string & filename){
	std::ofstream file(filename.c_str());
	if (!file) {
		return false;
	}
	for (std::map<std::string, std::string>::const_iterator it = settings.begin();
			it != settings.end();
			++it) {
		file << it->first << " = " << it->second << "\n";
	}
	file.close();
	return true;
}


const std::string & Configuration::get(const std::string & key) const
{
	return settings.find(key)->second;
}


void Configuration::set(const std::string & key, const std::string & value)
{
	settings[key] = value;
}


static inline bool to_bool(const std::string & str)
{
	std::string s = str_to_lower(str);
	return s == "true" || s == "yes" || s == "1" || s == "on";
}


bool Configuration::getBool(const std::string & key) const
{
	return to_bool(settings.find(key)->second);
}


int Configuration::getInt(const std::string & key) const
{
	return atoi(settings.find(key)->second.c_str());
}

