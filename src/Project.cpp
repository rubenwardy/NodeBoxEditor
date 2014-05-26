#include "Project.hpp"
#include "Node.hpp"
#include "util/string.hpp"

Project::Project() :
	name("test"),
	snode(-1),
	_node_count(0)
{
}

Project::~Project()
{
	for (std::list<Node*>::const_iterator it = nodes.begin();
			it != nodes.end();
			++it) {
		if (*it) {
			delete *it;
		}
	}
}

Node* Project::GetNode(int id) const
{
	int curid = 0;
	for (std::list<Node*>::const_iterator it = nodes.begin();
			it != nodes.end();
			++it, ++curid) {
		if (*it && curid == id) {
			return *it;
		}
	}
	return NULL;
}

Node* Project::GetNode(vector3di pos) const
{
	for (std::list<Node*>::const_iterator it = nodes.begin();
			it != nodes.end();
			++it) {
		if (*it && (*it)->position == pos) {
			return *it;
		}
	}
	return NULL;
}

void Project::remesh()
{
	for (std::list<Node*>::const_iterator it = nodes.begin();
			it != nodes.end();
			++it) {
		if (*it) {
			(*it)->remesh();
		}
	}
}

void Project::AddNode(EditorState* state, bool select)
{
	Node* node = new Node(state->device, state, _node_count);
	AddNode(node, select);
}

void Project::AddNode(Node* node, bool select)
{
	_node_count++;
	if (node->name == "") {
		node->name = "Node" + num_to_str(_node_count);
	}
	node->position = vector3di((_node_count - 1), 0, 0);
	nodes.push_back(node);
	if (select) {
		snode = _node_count - 1;
	}
}

void Project::DeleteNode(int id)
{
	if (snode == id) {
		snode = -1;
	}

	int curid = 0;
	for (std::list<Node*>::iterator it = nodes.begin();
			it != nodes.end();
			++it, ++curid) {
		if (*it && curid == id){
			delete *it;
			it = nodes.erase(it);
			return;
		}
	}
}

Node* Project::GetCurrentNode() const
{
	if (snode >= 0) {
		return GetNode(snode);
	} else {
		return NULL;
	}
}

