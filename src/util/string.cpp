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


std::wstring narrow_to_wide(const std::string & input)
{
	wchar_t * wide = new wchar_t[input.size() * 2];
	size_t newlen = mbstowcs(wide, input.data(), input.size());
	return std::wstring(wide, newlen);
}


std::string str_replace(const std::string & str, char f, char r)
{
	std::string s(str);
	std::replace(s.begin(), s.end(), f, r);
	return s;
}
