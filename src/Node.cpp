#include "util/string.hpp"
#include "Node.hpp"

Node::Node(IrrlichtDevice* device, EditorState* state, unsigned int id) :
	device(device),
	state(state),
	_selected(-1),
	_nid(id),
	_box_count(0)
{
	for (int i = 0; i < 6; i++) {
		images[i] = state->project->media.get("default");
	}
}

Node::~Node() {
	for (int i = 0; i < 6; i++) {
		images[i]->drop();
		images[i] = NULL;
	}
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
	_box_count++;
	// Name it
	std::string name = "NodeBox" + num_to_str(_box_count);

	// Set up structure
	NodeBox* tmp = new NodeBox(name,
		vector3df(-0.5, -0.5, -0.5),
		vector3df(0.5, 0.5, 0.5));

	boxes.push_back(tmp);
	select(boxes.size() - 1);
	tmp->buildNode(state, position, device, images);

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
		(*it)->buildNode(state, position, device, images);
	}
}

