#include "cNodes.h"

cNode::cNode(IrrlichtDevice* mdevice, ed_data* n_ed){
	device = mdevice;
	driver = device->getVideoDriver();
	smgr = device->getSceneManager();
	editor=n_ed;
	number=0;
}

void cNode::addNodeBox(){
	std::cout << "NodeBox Adder" << std::endl;

	boxes[number]=smgr->addCubeSceneNode(1,0,-1,vector3df(0,0,0));
	boxes[number]->setMaterialTexture(0, driver->getTexture("texture_box.png"));
    boxes[number]->setMaterialFlag(video::EMF_BILINEAR_FILTER, false);

	core::stringw nb=L"NodeBox ";
	nb+=(number+1);
	boxes[number]->setName(nb);
	//boxes[number]->getMaterial(0).getTextureMatrix(0).setTextureScale(10,10);

	std::cout << "--Inc" << std::endl;
	changeID(number);
	number++;

	

	std::cout << "--Added" << std::endl;
}

bool cNode::switchFocus(ISceneNode* hit){
	int a;
	for (a=0;a<number;a++){
		if (boxes[a])
			if (boxes[a]==hit)
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
	if (boxes[id]){
		core::stringw nb=L"NodeBox: ";
		nb+=boxes[id]->getName();
		editor->d_nb->setText(nb.c_str());

		core::stringw ps=L"Position: ";
		ps+=boxes[id]->getPosition().X;
		ps+=" , ";
		ps+=boxes[id]->getPosition().Y;
		ps+=" , ";
		ps+=boxes[id]->getPosition().Z;
		editor->d_pos->setText(ps.c_str());

		irr::core::aabbox3d<f32> box = boxes[id]->getTransformedBoundingBox();
		irr::core::vector3df extent = box.getExtent();

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

void cNode::resize(int side,double dir){
	std::cout << "Resizing..." << std::endl;
	if (boxes[id]){
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