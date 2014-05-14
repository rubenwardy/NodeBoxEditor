#include "FileParser.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>

void NBEFileParser::save(Project* project,irr::core::stringc file){
	std::ofstream myfile (file.c_str());
	if (myfile.is_open()){
		myfile << "MINETEST NODEBOX EDITOR\n";
		myfile << "PARSER 1\n";
		myfile << "NAME ";
		if (project->name == "")
			myfile << "test";
		else
			myfile << project->name.c_str();
		myfile << "\n\n";

		int a = 1;
		list<Node*>* nodes = project->GetList();
		for (irr::core::list<Node*>::Iterator it=nodes->begin();it!=nodes->end();it++){
			printf("Looping...\n");
			Node* node = *it;
			myfile << "NODE ";
			if (node->name == ""){
				myfile << "Node";
				myfile << a;
			}else
				myfile << node->name.c_str();
			myfile << "\n";
			myfile << "POSITION ";
			myfile << node->getPosition().X;
			myfile << " ";
			myfile << node->getPosition().Y;
			myfile << " ";
			myfile << node->getPosition().Z;
			myfile << "\n";

			std::vector<NodeBox*> boxes = node->GetBoxes();
			for (std::vector<NodeBox*>::const_iterator it = boxes.begin();
					it != boxes.end();
					++it) {
				NodeBox* box = *it;
				myfile << "NODEBOX ";
				myfile << box->name.c_str();
				myfile << " ";
				myfile << box->one.X;
				myfile << " ";
				myfile << box->one.Y;
				myfile << " ";
				myfile << box->one.Z;
				myfile << " ";
				myfile << box->two.X;
				myfile << " ";
				myfile << box->two.Y;
				myfile << " ";
				myfile << box->two.Z;
				myfile << "\n";
			}

			myfile << "END NODE\n\n";
			a++;
		}

		myfile.close();
	}else
		printf("Unable to write to file\n");
}

Project* NBEFileParser::open(irr::core::stringc file){
	std::string line;
	std::ifstream myfile(file.c_str());
	if (myfile.is_open()){	
		// Read parser header
		if (!std::getline (myfile,line) || !std::getline (myfile,line))
			return NULL;
		if (line!="PARSER 1"){
			printf("Not parser 1!\n");
			return NULL;
		}else{
			printf("Is currect parser version\n");
		}

		// Parse file
		proj = new Project();
		stage = ERS_ROOT;
		while (std::getline(myfile, line))
		{
			parseLine(irr::core::stringc(line.c_str()));
		}
		myfile.close();
		return proj;
	}else{
		printf("Unable to open file\n"); 
		return NULL;
	}
}

void NBEFileParser::parseLine(stringc line){
	stringc l = line.trim(" \t\n\r");

	if (l == "")
		return;

	#if IRRLICHT_VERSION_MAJOR == 1 && IRRLICHT_VERSION_MINOR < 8
		stringc lw = irr::core::stringc(l);
		lw.make_lower();		
	#else
		stringc lw = l.make_lower();
	#endif
	

	if (stage == ERS_ROOT){
		if (lw.find("name ") == 0){
			stringc name = l.subString(4, l.size());
			proj->name = name.trim();
		}else if (lw.find("node ") == 0){
			stage = ERS_NODE;
			node = new Node(state->GetDevice(),state,proj->GetNodeCount());
			stringc name = l.subString(4,l.size());
			node->name = name.trim();
		}
	}else if (stage == ERS_NODE){
		if (lw.find("position ") == 0){
			printf("-- position parser not complete!\n");
		}else if (lw.find("nodebox ") == 0){
			stringc n = l.subString(7,l.size());
			n = n.trim();
			stringc ls[7];
			int i = 0;
			while (n!=""){
				int nid = n.find(" ");

				if (nid==-1){
					nid = n.size();
				}
				if (i>=7){
					printf("-- too many arguments to nodebox tag!\n%s\n",n.c_str());
					break;
				}
				ls[i] = n.subString(0,nid).trim();
				n = n.subString(nid,n.size()).trim();
				printf(">> %i ",i);
				printf(" is '%s'\n",ls[i].c_str());
				i++;				
			}
			node->addNodeBox();
			node->GetCurrentNodeBox()->name = ls[0];
			node->GetCurrentNodeBox()->one = vector3df(atof(ls[1].c_str()),atof(ls[2].c_str()),atof(ls[3].c_str()));
			node->GetCurrentNodeBox()->two = vector3df(atof(ls[4].c_str()),atof(ls[5].c_str()),atof(ls[6].c_str()));
			node->remesh();
		}else if (lw.find("end node") == 0){
			proj->AddNode(node);
			node = NULL;
			stage = ERS_ROOT;
		}		
	}
}

