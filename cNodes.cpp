#include "cNodes.h"

cNode::cNode(IrrlichtDevice* mdevice, ed_data* n_ed){
	id=0;
	device = mdevice;
	driver = device->getVideoDriver();
	smgr = device->getSceneManager();
	editor=n_ed;
	number=0;

	for (int i=0;i<NODEB_MAX;i++){
		boxes[i]=NULL;
	}	
}

const sBox* cNode::addNodeBox(){
	// Set up structure
	boxes[number]=new sBox();

	// Add node
	boxes[number]->model=smgr->addCubeSceneNode(1,0,-1,vector3df(0,0,0));
	boxes[number]->model->setMaterialTexture(0, driver->getTexture("texture_box.png"));
	boxes[number]->model->setMaterialFlag(video::EMF_BILINEAR_FILTER, false);

	// Name it
	core::stringw nb=L"NodeBox ";
	nb+=(number+1);
	boxes[number]->model->setName(nb);
	boxes[number]->size=vector3df(1,1,1);
	boxes[number]->position=vector3df(0,0,0);

	// Switch the selected nodebox
	changeID(number);

	// Increment and print message
	number++;
	printf("Nodebox added\n");

	return boxes[number-1];
}

bool cNode::switchFocus(ISceneNode* hit){
	for (int a=0;a<number;a++){
		if (boxes[a] && boxes[a]->model)
			if (boxes[a]->model==hit)
				changeID(a);
				return true;

	}
	return false;
}

void cNode::changeID(int n_id){
	id=n_id;

	updateTexts();	
}

void cNode::update(){
	for (int a=0;a<number;a++){
		if (boxes[a] && boxes[a]->model)
			checkScaling(boxes[a]);
	}

	updateTexts();
}

void cNode::updateTexts(){
	if (boxes[id] && number>id){
		core::stringw nb=L"NodeBox: ";
		nb+=boxes[id]->model->getName();
		editor->d_nb->setText(nb.c_str());

		core::stringw ps=L"Position: ";
		ps+=boxes[id]->model->getPosition().X;

		ps+=" , ";
		ps+=boxes[id]->model->getPosition().Y;
		ps+=" , ";
		ps+=boxes[id]->model->getPosition().Z;
		editor->d_pos->setText(ps.c_str());

		irr::core::vector3df extent = boxes[id]->size;

		core::stringw rt=L"Size: ";
		rt+=extent.X;
		rt+=" , ";
		rt+=extent.Y;
		rt+=" , ";
		rt+=extent.Z;

		editor->d_rot->setText(rt.c_str());
	}else{
		editor->d_nb->setText(L"NodeBox: -");
		editor->d_pos->setText(L"Position: - , - , -");
		editor->d_rot->setText(L"Size: - , - , -");
	}
}

void cNode::resize(int side,f32 dir){
	if (boxes[id] && number > id){
		switch (side)
		{
		case 0: //X
			setsizeObject(boxes[id], boxes[id]->size.X+dir, boxes[id]->size.Y, boxes[id]->size.Z);
			break;
		case 1: //Y
			setsizeObject(boxes[id], boxes[id]->size.X, boxes[id]->size.Y+dir, boxes[id]->size.Z);
			break;
		case 2: //Z
			setsizeObject(boxes[id], boxes[id]->size.X, boxes[id]->size.Y, boxes[id]->size.Z+dir);
			break;
		}
	}else{
		printf("[ERROR] No nodebox selected!\n");
	}
	updateTexts();
}

void cNode::checkScaling(sBox* input){
	// Load / Prerequistes
	irr::core::vector3df extent = input->size;
	bool tmp_change=false;

	// Check X Axis scale
	if (extent.X>1){
		std::cout << "--auto correct: x" << std::endl;
		input->size.X = 1;
		tmp_change=true;
	}else if(extent.X < NODE_THIN){
		std::cout << "--auto correct: x" << std::endl;
		input->size.X = NODE_THIN;
		tmp_change=true;
	}

	// Check Y Axis scale
	if (extent.Y > 1){
		std::cout << "--auto correct: y" << std::endl;
		input->size.Y = 1;
		tmp_change=true;
	}else if(extent.Y < NODE_THIN){
		std::cout << "--auto correct: y" << std::endl;
		input->size.Y = NODE_THIN;
		tmp_change=true;
	}

	// Check Z Axis scale
	if (extent.Z>1){
		std::cout << "--auto correct: z" << std::endl;
		input->size.Z = 1;
		tmp_change=true;
	}else if(extent.Z < NODE_THIN){
		std::cout << "--auto correct: z" << std::endl;
		input->size.Z = NODE_THIN;
		tmp_change=true;
	}

	// Resize if required
	if (tmp_change==true)
		setsizeObject(input,input->size.X,input->size.Y,input->size.Z);
}

void cNode::setsizeObject(sBox* input,f32 px,f32 py,f32 pz){
	// Check limits
	if (px > 1 || px < NODE_THIN || py > 1 || py <  NODE_THIN || pz > 1 || pz <  NODE_THIN){
		return;
	}

	// Hold the name temporary
	core::stringw nb=input->model->getName();

	// Remove the node
	input->model->remove();
	input->model=NULL;

	// Read the node
	input->model=smgr->addCubeSceneNode(1,0,-1,vector3df(0,0,0));
	input->model->setMaterialTexture(0, driver->getTexture("texture_box.png"));
	input->model->setMaterialFlag(video::EMF_BILINEAR_FILTER, false);	
	input->model->setName(nb);
 
	// Get scale factor
	f32 sx = px/1;
	f32 sy = py/1;
	f32 sz = pz/1;

	// Print stuff
	printf("Resizing to (%f,",px);
	printf(" %f,",py);
	printf(" %f)\n",pz);

	// Do resize
	input->size=vector3df(px,py,pz);
	input->model->setScale(core::vector3df(sx, sy, sz));
}

const sBox* cNode::getCurrentNodeBox(){
	return boxes[id];
}

void cNode::resizeNodeBoxFace(const sBox* nodebox,CDR_TYPE face, vector3df target){
	// stuff to do here

	// Print stuff
	printf("Resizing to (%f,",target.X);
	printf(" %f,",target.Y);
	printf(" %f)\n",target.Z);

	return;
}
