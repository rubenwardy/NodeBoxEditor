#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <stdexcept>
#include "../common.hpp"
#include "../Project.hpp"
#include "../Node.hpp"
#include "Lua.hpp"
#include <sstream>
#include "../util/filesys.hpp"

bool LuaFileFormat::write(Project * project, const std::string & filename){
	std::ofstream file(filename.c_str());
	if (!file) {
		error_code = EFFE_IO_ERROR;
		return false;
	}
	file << getAsString(project);
	file.close();

	// Save images
	std::string path = pathWithoutFilename(filename) + "textures/";
	std::cerr << path.c_str() << std::endl;
	CreateDir(path.c_str());
	Media *media = &project->media;
	std::map<std::string, Media::Image*>& images = media->getList();
	for (std::map<std::string, Media::Image*>::const_iterator it = images.begin();
			it != images.end();
			++it) {
		Media::Image *image = it->second;
		if (!image->get()) {
			std::cerr << "Image->get() is NULL!" << std::endl;
			continue;
		}
		state->device->getVideoDriver()->writeImageToFile(image->get(), (path + image->name).c_str());
	}
	return true;
}

std::string doTileImage(Node *node, CubeSide face)
{
	Media::Image *image = node->getTexture(face);
	if (!image)
		return "default_wood.png";
	
	return image->name;
}

std::string LuaFileFormat::getAsString(Project *project)
{
	std::ostringstream file;
	file << "-- GENERATED CODE\n";
	file << "-- Node Box Editor, version " << EDITOR_TEXT_VERSION << '\n';
	file << "-- Namespace: " << project->name << "\n\n";

	std::list<Node*> & nodes = project->nodes;
	unsigned int i = 0;
	for (std::list<Node*>::const_iterator it = nodes.begin();
			it != nodes.end();
			++it, ++i) {
		Node* node = *it;
		file << "minetest.register_node(\"";
		if (node->name == "") {
			file << project->name << ":node_" << 1;
		} else {
			file << project->name << ":" << node->name;
		}
		file << "\", {\n";
		file << "\ttiles = {\n";
		file << "\t\t\"" << doTileImage(node, ECS_TOP).c_str() << "\",\n";
		file << "\t\t\"" << doTileImage(node, ECS_BOTTOM).c_str() << "\",\n";
		file << "\t\t\"" << doTileImage(node, ECS_RIGHT).c_str() << "\",\n";
		file << "\t\t\"" << doTileImage(node, ECS_LEFT).c_str() << "\",\n";
		file << "\t\t\"" << doTileImage(node, ECS_BACK).c_str() << "\",\n";
		file << "\t\t\"" << doTileImage(node, ECS_FRONT).c_str() << "\"\n";
		file << "\t},\n";
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

	return file.str();
}


Project * LuaFileFormat::read(const std::string & file, Project *project)
{
	throw std::runtime_error("LuaFileFormat::read() is not implemented!");
	return NULL;
}

