#include "util/string.hpp"
#include "Node.h"

Node::Node(IrrlichtDevice* device, EditorState* state, unsigned int id) :
	device(device),
	state(state),
	_selected(-1),
	_nid(id)
{
}

Node::~Node() {
	for (std::vector<NodeBox*>::iterator it = boxes.begin();
			it != boxes.end();
			++it) {
		delete *it;
	}
	boxes.clear();
}

NodeBox* Node::GetCurrentNodeBox() {
	return GetNodeBox(GetId());
}

NodeBox* Node::GetNodeBox(int id) {
	if (id < 0 || id > boxes.size()) {
		return NULL;
	}

	return boxes[id];
}

// Operation functions
NodeBox* Node::addNodeBox(){
	// Name it
	std::string name = "NodeBox" + num_to_str(boxes.size() + 1);

	// Set up structure
	NodeBox* tmp = new NodeBox(name,
		vector3df(0, -0.5, -0.5),
		vector3df(0.5, 0.5, 0.5));

	boxes.push_back(tmp);
	select(boxes.size() - 1);
	tmp->buildNode(position, device);

	return tmp;
}

void Node::deleteNodebox(int id){
	if (!GetNodeBox(id)) {
		return;
	}

	delete boxes[id];
	boxes.erase(boxes.begin() + id);
	if (GetId() >= boxes.size())
		_selected = boxes.size() - 1;
}

// Build node models
void Node::remesh() {
	for (std::vector<NodeBox*>::iterator it = boxes.begin();
			it != boxes.end();
			++it) {
		(*it)->buildNode(position, device);
	}
}

