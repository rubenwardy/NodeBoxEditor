#include "cNodes.h"

cNode::cNode(IrrlichtDevice* mdevice, ed_data* n_ed){
	id=0;
	device = mdevice;
	driver = device->getVideoDriver();
	smgr = device->getSceneManager();
	editor=n_ed;
	number=0;
}

const sBox* cNode::addNodeBox(){
	std::cout << "NodeBox Adder" << std::endl;

	boxes[number]=new sBox();
	boxes[number]->model=smgr->addCubeSceneNode(1,0,-1,vector3df(0,0,0));
	boxes[number]->model->setMaterialTexture(0, driver->getTexture("texture_box.png"));
    boxes[number]->model->setMaterialFlag(video::EMF_BILINEAR_FILTER, false);

	core::stringw nb=L"NodeBox ";
	nb+=(number+1);
	boxes[number]->model->setName(nb);
	irr::core::aabbox3d<f32> box = boxes[number]->model->getTransformedBoundingBox();
	irr::core::vector3df extent = box.getExtent();
	boxes[number]->size=extent;

	std::cout << "--Inc" << std::endl;
	changeID(number);
	number++;

	return boxes[number-1];

	std::cout << "--Added" << std::endl;
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

		irr::core::vector3df extent2 = boxes[id]->model->getTransformedBoundingBox().getExtent();
		
		rt+="\nSize: ";
		rt+=extent2.X;
		rt+=" , ";
		rt+=extent2.Y;
		rt+=" , ";
		rt+=extent2.Z;

		editor->d_rot->setText(rt.c_str());
	}else{
		editor->d_nb->setText(L"NodeBox: -");
		editor->d_pos->setText(L"Position: - , - , -");
		editor->d_rot->setText(L"Size: - , - , -");
	}
}

void cNode::resize(int side,f32 dir){
	std::cout << "Resizing..." << std::endl;
	if (boxes[id] && number > id){
		switch (side)
		{
		case 0: //X
			resizeObject(boxes[id],dir,0,0);
			break;
		case 1: //Y
			resizeObject(boxes[id],0,dir,0);
			break;
		case 2: //Z
			resizeObject(boxes[id],0,0,dir);
			break;
		}
	}else{
		std::cout << "Nodebox is not selected" << std::endl;
	}
	updateTexts();
}

void cNode::checkScaling(sBox* input){
	irr::core::vector3df extent = input->size;
 
	f32 sx=extent.X;
	f32 sy=extent.Y;
	f32 sz=extent.Z;
	bool tmp_change=false;

	if (extent.X>1){
		std::cout << "--auto correct: x" << std::endl;
		input->size.X = 1;
		tmp_change=true;
	}else if(extent.X < NODE_THIN){
		std::cout << "--auto correct: x" << std::endl;
		input->size.X = NODE_THIN;
		tmp_change=true;
	}

	if (extent.Y > 1){
		std::cout << "--auto correct: y" << std::endl;
		input->size.Y = 1;
		tmp_change=true;
	}else if(extent.Y < NODE_THIN){
		std::cout << "--auto correct: y" << std::endl;
		input->size.Y = NODE_THIN;
		tmp_change=true;
	}

	if (extent.Z>1){
		std::cout << "--auto correct: z" << std::endl;
		input->size.Z = 1;
		tmp_change=true;
	}else if(extent.Z < NODE_THIN){
		std::cout << "--auto correct: z" << std::endl;
		input->size.Z = NODE_THIN;
		tmp_change=true;
	}

	setsizeObject(input,input->size.X,input->size.Y,input->size.Z);
}

void cNode::resizeObject(sBox* input,f32 px,f32 py,f32 pz){
	std::cout << std::endl << "-----------------------" << std::endl << "Performing resize." << std::endl;

	std::cout << std::endl << "Increase by: " << std::endl << px << " - " << py << " - " << pz << std::endl << std::endl;

	irr::core::vector3df extent = input->size;

	if ((px+extent.X)>1 || (px+extent.X) < NODE_THIN){
		std::cout << "--error! target out of bounds" << std::endl;
		return;
	}

	if ((py+extent.Y) > 1 || (py+extent.Y) <  NODE_THIN){
		std::cout << "--error! target out of bounds" << std::endl;
		return;
	}

	if ((pz+extent.Z)>1 || (pz+extent.Z)<  NODE_THIN){
		std::cout << "--error! target out of bounds" << std::endl;
		return;
	}

 
	f32 sx = px+extent.X;
	f32 sy = py+extent.Y;
	f32 sz = pz+extent.Z;

	setsizeObject(input,sx,sy,sz);
}

void cNode::setsizeObject(sBox* input,f32 px,f32 py,f32 pz){

	std::cout << std::endl << "-----------------------" << std::endl << "Performing resize." << std::endl;

	core::stringw nb=input->model->getName();

	input->model->remove();
	input->model=NULL;

	input->model=smgr->addCubeSceneNode(1,0,-1,vector3df(0,0,0));
	input->model->setMaterialTexture(0, driver->getTexture("texture_box.png"));
    input->model->setMaterialFlag(video::EMF_BILINEAR_FILTER, false);	
	input->model->setName(nb);
 
	f32 sx = px/1;
	f32 sy = py/1;
	f32 sz = pz/1;

	std::cout << std::endl << "Before: " << std::endl << input->size.X << " - " << input->size.Y << " - " << input->size.Z << std::endl;

	std::cout << std::endl << "After: " << std::endl << px << " - " << py << " - " << pz << std::endl;

	std::cout << std::endl << "Scaled by: " << std::endl << sx << " - " << sy << " - " << sz << std::endl;

	input->size=vector3df(px,py,pz);
	input->model->setScale(core::vector3df(sx, sy, sz));
}
