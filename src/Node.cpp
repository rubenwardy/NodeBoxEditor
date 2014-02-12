#include "Node.h"

Node::Node(IrrlichtDevice* mdevice,EditorState* state)
	:_device(mdevice),_state(state),number(0),_id(-1)
{
	for (int i=0;i<NODEB_MAX;i++){
		boxes[i] = NULL;
	}
}

Node::~Node(){
	for (int i=0;i<NODEB_MAX;i++){
		if (boxes[i])
			delete boxes[i];
	}
}

NodeBox* Node::GetCurrentNodeBox(){
	return GetNodeBox(GetId());
}

NodeBox* Node::GetNodeBox(int id){
	if (id==-1)
		return NULL;

	return boxes[id];
}

// Operation functions
NodeBox* Node::addNodeBox(){
	if (!number)
		number = 0;

	// Name it
	core::stringc nb="NodeBox";
	nb+=(number+1);

	// Set up structure
	NodeBox* tmp =new NodeBox(nb,vector3df(0,-0.5,-0.5),vector3df(0.5,0.5,0.5));

	if (!tmp)
		return NULL;

	boxes[number] = tmp;
	select(number);
	tmp->buildNode(getPosition(),_device);

	// Increment and print message
	number++;
	printf("Nodebox added\n");

	// Clean up list
	defrag();

	return tmp;
}

void Node::deleteNodebox(int id){
	if (!GetNodeBox(id))
		return;

	delete boxes[id];
	boxes[id] = NULL;

	defrag();
}

// Build node models
void Node::remesh(){
	for (int i=0;i<NODEB_MAX;i++){
		NodeBox* box = GetNodeBox(i);
		if (box)
			box->buildNode(getPosition(),_device);
	}
}

// Private functions
void Node::defrag(){
	int a=0;
	for (int i=0;i<NODEB_MAX;i++){
		if (boxes[i]!=NULL){
			boxes[a]=boxes[i];
			if (GetId()==i)
				_id=a;
			a++;
		}else{
			boxes[a]=NULL;
		}
	}

	number = a;

#ifdef _DEBUG
	for (int i=0;i<NODEB_MAX;i++){
		if (boxes[i]!=NULL && boxes[i]->model->getName()){
			printf("%i> ",i);
			printf("%s \n",boxes[i]->model->getName());
		}
	}

	printf("There are %i boxes\n",a);
#endif
}
