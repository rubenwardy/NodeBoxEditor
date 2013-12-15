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
			printf("Looping...\n");
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

			for (int i = 0;i<NODEB_MAX;i++){
				NodeBox* box = node->GetNodeBox(i);

				if (box){
					myfile << "\t\t\t{";
					myfile << box->one.X;
					myfile << ",";
					myfile << box->one.Y;
					myfile << ",";
					myfile << box->one.Z;
					myfile << ",";
					myfile << box->two.X;
					myfile << ",";
					myfile << box->two.Y;
					myfile << ",";
					myfile << box->two.Z;
					myfile << "}, --";
					myfile << box->name.c_str();
					myfile << "\n";
				}
			}

			myfile << "\t\t}\n\t}\n";
			myfile << "})\n\n";
			a++;
		}

		myfile.close();
	}else
		printf("Unable to write to file\n");
}