#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <stdexcept>
#include "../common.hpp"
#include "../Project.hpp"
#include "../Node.hpp"
#include "Lua.hpp"

bool LuaFileFormat::write(Project * project, const std::string & filename){
	std::ofstream file(filename.c_str());
	if (!file) {
		return false;
	}
	file << "-- GENERATED CODE\n";
	file << "-- Node Box Editor, version " << EDITOR_TEXT_VERSION << '\n';
	file << "-- Namespace: " << project->name << "\n\n";

	std::list<Node*> & nodes = project->nodes;
	unsigned int i = 0;
	for (std::list<Node*>::const_iterator it = nodes.begin();
			it != nodes.end();
			++it, ++i) {
		Node* node = *it;
		/*
		 * Example output:
		 * minetest.register_node("test:node_0", {
		 * 	drawtype = "nodebox",
		 * 	paramtype = "light",
		 * 	node_box = {
		 * 		type = "fixed",
		 * 		fixed = {
		 * 			{-0.5, -0.5, -0.5, 0.5, 0.5, 0.5} -- Main box
		 * 		}
		 * 	}
		 * })
		 */
		file << "minetest.register_node(\"";
		if (node->name == "") {
			file << project->name << ":node_" << 1;
		} else {
			file << project->name << ":" << node->name;
		}
		file << "\", {\n";
		file << "\tdrawtype = \"nodebox\",\n"
			"\tparamtype = \"light\",\n"
			"\tnode_box = {\n"
			"\t\ttype = \"fixed\",\n"
			"\t\tfixed = {\n";

		std::vector<NodeBox*> & boxes = node->boxes;
		for (std::vector<NodeBox*>::const_iterator it = boxes.begin();
				it != boxes.end();
				++it) {
			file << "\t\t\t{";
			file << (*it)->one.X << ", " << (*it)->one.Y << ", " << (*it)->one.Z << ", ";
			file << (*it)->two.X << ", " << (*it)->two.Y << ", " << (*it)->two.Z << "},";
			file << " -- " << (*it)->name << "\n";
		}

		file << "\t\t}\n"
			"\t}\n"
			"})\n\n";
	}

	file.close();

	return true;
}


Project * LuaFileFormat::read(const std::string & file)
{
	// TODO: Read Lua files
	throw std::runtime_error("LuaFileFormat::read() is not implemented!");
	return NULL;
}

