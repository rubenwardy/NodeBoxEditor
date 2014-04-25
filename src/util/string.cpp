#include <algorithm>
#include <cctype>
#include "string.hpp"

std::string trim(const std::string & str)
{
	std::string s(str);
	s.erase(s.find_last_not_of(" \t\r\n") + 1);
	s.erase(0, s.find_first_not_of(" \t\r\n"));
	return s;
}

std::string str_to_lower(const std::string & str)
{
	std::string s(str);
	std::transform(s.begin(), s.end(), s.begin(), tolower);
	return s;
}

