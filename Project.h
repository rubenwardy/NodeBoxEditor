#ifndef _PROJECT_H_INCLUDED_
#define _PROJECT_H_INCLUDED_
#include "common.h"
#include "Node.h"

class Project
{
public:
	// Constructors
	Project(io::path file)
	:Project()
	{
		// Encode reader here
		filepath = file;
	}

	Project()
	:filepath(NULL),mod("untitled"),_id(NULL)
	{
		for (int i=0;i<5;i++){
			nodes[i]=NULL;
		}
	}

	~Project(){
		for (int i=0;i<5;i++){
			if (nodes[i])
				delete nodes[i];
		}
	}

	// Data
	stringc mod;
	io::path filepath;

	// Nodes
	Node* nodes[5];
	Node* GetNode(int id){
		if (id>=5)
			return NULL;

		return nodes[id];
	}
	Node* GetCurNode(){
		return GetNode(_id);		
	}	
private:
	int _id;
};

#endif