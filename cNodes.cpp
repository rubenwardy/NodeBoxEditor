#include "cNodes.h"

cNode::cNode(IrrlichtDevice* mdevice, ed_data* n_ed){
	id=0;
	device = mdevice;
	driver = device->getVideoDriver();
	smgr = device->getSceneManager();
	editor=n_ed;
	number=0;
}

void cNode::addNodeBox(){
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

	

	std::cout << "--Added" << std::endl;
}

bool cNode::switchFocus(ISceneNode* hit){
	int a;
	for (a=0;a<number;a++){
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