#include "../util/string.hpp"
#include "node.hpp"

Node::Node(IrrlichtDevice* device, EditorState* state, unsigned int id) :
	device(device),
	state(state),
	_selected(-1),
	_nid(id),
	_box_count(0),
	snap_res(-1)
{
	for (int i = 0; i < 6; i++) {
		images[i] = NULL;
	}
}


void Node::setAllTextures(Media::Image *def)
{
	if (!def) {
		std::cerr << "[Node::setAllTextures] def is NULL" << std::endl;
		return;
	}
	for (int i = 0; i < 6; i++) {
		if (images[i])
			images[i]->drop();
		def->grab();
		images[i] = def;
	}
}

Node::~Node()
{
	for (int i = 0; i < 6; i++) {
		if (images[i])
			images[i]->drop();
		images[i] = NULL;
	}
	for (std::vector<NodeBox*>::iterator it = boxes.begin();
			it != boxes.end();
			++it) {
		(*it)->removeMesh(state->device->getVideoDriver());
		delete *it;
	}
	boxes.clear();
}

NodeBox* Node::GetCurrentNodeBox()
{
	return GetNodeBox(GetId());
}

NodeBox* Node::GetNodeBox(int id)
{
	if (id < 0 || id > (int)boxes.size()) {
		return NULL;
	}

	return boxes[id];
}

// Operation functions
NodeBox* Node::addNodeBox(vector3df one, vector3df two)
{
	_box_count++;

	// Set up structure
	std::string name = "NodeBox" + num_to_str(_box_count);
	NodeBox *tmp = new NodeBox(name, one, two);
	boxes.push_back(tmp);

	// Select
	select(boxes.size() - 1);

	tmp->buildMesh(state, position, device, images);

	return tmp;
}

void Node::deleteNodebox(int id)
{
	if (!GetNodeBox(id)) {
		return;
	}

	boxes[id]->removeMesh(state->device->getVideoDriver());
	delete boxes[id];
	boxes.erase(boxes.begin() + id);
	if (GetId() >= (int)boxes.size())
		_selected = boxes.size() - 1;
}

void Node::cloneNodebox(int id)
{
	NodeBox *nb = GetNodeBox(id);
	if (!nb)
		return;

	NodeBox *new_nb = addNodeBox();
	new_nb->one = nb->one;
	new_nb->two = nb->two;
	new_nb->buildMesh(state, position, device, images);
}

void Node::setTexture(ECUBE_SIDE face, Media::Image *image)
{
	if (image) {
		if (images[face])
			images[face]->drop();
		image->grab();
		images[face] = image;
	}
}

// Build node models
void Node::remesh(bool force)
{
	for (std::vector<NodeBox*>::iterator it = boxes.begin();
			it != boxes.end();
			++it) {
		(*it)->buildMesh(state, position, device, images, force);
	}
}

void Node::remesh(NodeBox *box)
{
	box->buildMesh(state, position, device, images);
}

void Node::hide()
{
	for (std::vector<NodeBox*>::iterator it = boxes.begin();
			it != boxes.end();
			++it) {
		NodeBox *box = *it;
		if (box->model) {
			box->model->remove();
			box->model = NULL;
		}
	}
}

void Node::rotate(EAxis axis)
{
	for (std::vector<NodeBox*>::iterator it = boxes.begin();
			it != boxes.end();
			++it) {
		(*it)->rotate(axis);
	}
	remesh();
}

void Node::flip(EAxis axis)
{
	for (std::vector<NodeBox*>::iterator it = boxes.begin();
			it != boxes.end();
			++it) {
		(*it)->flip(axis);
	}
	remesh();
}
