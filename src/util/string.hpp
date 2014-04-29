#ifndef UTIL_STRING_HPP_INCLUDED
#define UTIL_STRING_HPP_INCLUDED

#include <string>
#include <sstream>

extern std::string trim(const std::string & str);
extern std::string str_to_lower(const std::string & str);
extern std::wstring narrow_to_wide(const std::string & input);
extern std::string str_replace(const std::string & str, char f, char r);


template<class T>
const std::string num_to_str(T num)
{
	std::ostringstream os;
	os << num;
	return os.str();
}

#endif // UTIL_STRING_HPP_INCLUDED
