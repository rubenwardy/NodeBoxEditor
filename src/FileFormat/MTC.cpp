#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <stdexcept>
#include "../common.hpp"
#include "../Project.hpp"
#include "../Node.hpp"
#include "MTC.hpp"

bool MTCFileFormat::write(Project * project, const std::string & filename){
	std::ofstream file(filename.c_str());
	if (!file) {		
		error_code = EFFE_IO_ERROR;
		return false;
	}
	file << "// GENERATED CODE\n";
	file << "// Node Box Editor, version " << EDITOR_TEXT_VERSION << '\n';
	file << "// Project name: " << project->name << "\n";
	file << "// Copy and paste the following code snippets\n";
	file << "//   into the correct place in src/content_nodebox.cpp\n\n";

	std::list<Node*> & nodes = project->nodes;
	unsigned int i = 0;
	for (std::list<Node*>::const_iterator it = nodes.begin();
			it != nodes.end();
			++it, ++i) {
		Node* node = *it;
		file << "void content_nodebox_";
		if (node->name == "") {
			file << "node_" << 1;
		} else {
			file << node->name;
		}
		file << "(ContentFeatures *f)\n";
		file << "{\n";
		bool is_first = true;
		std::vector<NodeBox*> & boxes = node->boxes;
		for (std::vector<NodeBox*>::const_iterator it = boxes.begin();
				it != boxes.end();
				++it) {
			
			file << "\t// " << (*it)->name << "\n";
			if (is_first) {
				file << "\tf->setNodeBox(core::aabbox3d<f32>(\n";
			} else {
				file << "\tf->addNodeBox(core::aabbox3d<f32>(\n";
			}
			file << "\t\t" << (*it)->one.X << "*BS,\n\t\t" << (*it)->one.Y << "*BS,\n\t\t" << (*it)->one.Z << "*BS,\n";
			file << "\t\t" << (*it)->two.X << "*BS,\n\t\t" << (*it)->two.Y << "*BS,\n\t\t" << (*it)->two.Z << "*BS\n";
			file << "\t));\n\n";
			is_first = false;
		}
		file << "}\n\n";
	}

	file.close();

	return true;
}


Project * MTCFileFormat::read(const std::string & file)
{
	throw std::runtime_error("MTCFileFormat::read() is not implemented!");
	return NULL;
}

