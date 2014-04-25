#ifndef _CONFIGURATION_HPP_INCLUDED_
#define _CONFIGURATION_HPP_INCLUDED_

#include <map>

class Configuration
{
public:
	Configuration() {}
	bool load(const std::string & filename);
	bool save(const std::string & filename);	

	// Getters
	const std::string & get(const std::string & key) const;
	bool getBool(const std::string & key) const;
	int getInt(const std::string & key) const;

	// Setters
	void set(const std::string & key, const std::string & value);
protected:
	void readLine(std::string & line);

	std::map<std::string, std::string> settings;
};

#endif
