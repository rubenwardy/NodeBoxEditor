#include "FileParser.h"
#include <iostream>
#include <fstream>
#include <string>

void LUAFileParser::save(Project* project,irr::core::stringc file){
	std::ofstream myfile (file.c_str());
	if (myfile.is_open()){
		myfile << "-- GENERATED CODE\n";
		myfile << "-- Node Box Editor, version ";
		myfile << EDITOR_TEXT_VERSION;
		myfile << "\n";
		myfile << "-- Namespace: ";
		char* name = "";
		
		if (project->name == "")
			name = "test";
		else
			name = (char*) project->name.c_str();

		myfile << name;
		myfile << "\n\n";

		int a = 1;
		list<Node*>* nodes = project->GetList();
		for (irr::core::list<Node*>::Iterator it=nodes->begin();it!=nodes->end();it++){
			Node* node = *it;
			myfile << "minetest.register_node(\"";
			if (node->name == ""){
				myfile << name;
				myfile << ":node_";
				myfile << a;
			}else{
				myfile << name;
				myfile << ":";
				myfile << node->name.c_str();
			}
			myfile << "\",{\n";
			myfile << "\tdrawtype=\"nodebox\",\n"
				"\tparamtype = \"light\",\n"
				"\tnode_box = {\n"
				"\t\ttype = \"fixed\",\n"
				"\t\tfixed = {\n";

			std::vector<NodeBox*> boxes = node->GetBoxes();
			for (std::vector<NodeBox*>::const_iterator it = boxes.begin();
					it != boxes.end();
					++it) {
				myfile << "\t\t\t{";
				myfile << (*it)->one.X;
				myfile << ",";
				myfile << (*it)->one.Y;
				myfile << ",";
				myfile << (*it)->one.Z;
				myfile << ",";
				myfile << (*it)->two.X;
				myfile << ",";
				myfile << (*it)->two.Y;
				myfile << ",";
				myfile << (*it)->two.Z;
				myfile << "}, --";
				myfile << (*it)->name.c_str();
				myfile << "\n";
			}

			myfile << "\t\t}\n\t}\n";
			myfile << "})\n\n";
			a++;
		}

		myfile.close();
	} else {
		printf("Unable to write to file\n");
	}
}
