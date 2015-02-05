#ifndef MINETEST_HPP_INCLUDED
#define MINETEST_HPP_INCLUDED

#include "common.hpp"
#include "EditorState.hpp"
#include "Configuration.hpp"

class Minetest
{
public:
	Minetest(Configuration *conf);
	bool findMinetest(bool editor_is_installed);
	bool runMod(EditorState *state, const std::string &world = "nbe_test");
private:
	bool findMinetestDir(std::string path);

	Configuration *_conf;
	std::string minetest_dir;
	std::string minetest_exe;
};

#endif
