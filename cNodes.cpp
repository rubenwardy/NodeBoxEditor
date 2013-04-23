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

sBox* cNode::addNodeBox(){
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

bool cNode::setsizeObject(sBox* input,f32 px,f32 py,f32 pz){
	// Check limits
	if (px > 1 || px < NODE_THIN || py > 1 || py <  NODE_THIN || pz > 1 || pz <  NODE_THIN){
		return false;
	}

	// Hold the name temporary
	core::stringw nb=input->model->getName();

	// Remove the node
	input->model->remove();
	input->model=NULL;

	// Read the node
	input->model=smgr->addCubeSceneNode(1,0,-1,input->position);
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

	return true;
}

sBox* cNode::getCurrentNodeBox(){
	return boxes[id];
}

void cNode::resizeNodeBoxFace(sBox* nodebox,CDR_TYPE face, vector3df target){
	// Print stuff
	printf("Ray hit (%f,", target.X);
	printf(" %f,",target.Y);
	printf(" %f)\n",target.Z);

	f32 opp,change;
	vector3df tsize;

	switch (face){
	case CDR_X_P:
		// Resize
		tsize=nodebox->size; // Save the size		
		opp = nodebox->position.X + ((float)nodebox->size.X / (float)2); // Get the corner before		
		tsize.X -= opp - target.X; // Adjust the size to now
		if (tsize.X <= 0) // Validate
			tsize.X=NODE_THIN;
		if (tsize.X > 1) // Validate
			tsize.X=1;
		setsizeObject(nodebox,tsize.X,tsize.Y,tsize.Z); // Resize	

		// Change position
		change = (nodebox->position.X + ((float)nodebox->size.X / (float)2))-opp; 
		nodebox->position.X += change;
		break;
	case CDR_X_N:
		// Resize
		tsize=nodebox->size; // Save the size		
		opp = nodebox->position.X - ((float)nodebox->size.X / (float)2); // Get the corner before			
		tsize.X += opp - target.X; // Adjust the size to now
		if (tsize.X <= 0) // Validate
			tsize.X=NODE_THIN;
		if (tsize.X > 1) // Validate
			tsize.X=1;
		setsizeObject(nodebox,tsize.X,tsize.Y,tsize.Z); // Resize	

		// Change position
		change = (nodebox->position.X - ((float)nodebox->size.X / (float)2))-opp; 
		nodebox->position.X += change;
		break;
	case CDR_Y_P:
		// Resize
		tsize=nodebox->size; // Save the size		
		opp = nodebox->position.Y + ((float)nodebox->size.Y / (float)2); // Get the corner before		
		tsize.Y -= opp - target.Y; // Adjust the size to now
		if (tsize.Y <= 0) // Validate
			tsize.Y=NODE_THIN;
		if (tsize.Y > 1) // Validate
			tsize.Y=1;
		setsizeObject(nodebox,tsize.X,tsize.Y,tsize.Z); // Resize	

		// Change position
		change = (nodebox->position.Y + ((float)nodebox->size.Y / (float)2))-opp; 
		nodebox->position.Y += change;
		break;
	case CDR_Y_N:
		// Resize
		tsize=nodebox->size; // Save the size		
		opp = nodebox->position.Y - ((float)nodebox->size.Y / (float)2); // Get the corner before			
		tsize.Y += opp - target.Y; // Adjust the size to now
		if (tsize.Y <= 0) // Validate
			tsize.Y=NODE_THIN;
		if (tsize.Y > 1) // Validate
			tsize.Y=1;
		setsizeObject(nodebox,tsize.X,tsize.Y,tsize.Z); // Resize	

		// Change position
		change = (nodebox->position.Y - ((float)nodebox->size.Y / (float)2))-opp; 
		nodebox->position.Y += change;
		break;
	case CDR_Z_P:
		// Resize
		tsize=nodebox->size; // Save the size		
		opp = nodebox->position.Z + ((float)nodebox->size.Z / (float)2); // Get the corner before		
		tsize.Z -= opp - target.Z; // Adjust the size to now
		if (tsize.Z <= 0) // Validate
			tsize.Z=NODE_THIN;
		if (tsize.Z > 1) // Validate
			tsize.Z=1;
		setsizeObject(nodebox,tsize.X,tsize.Y,tsize.Z); // Resize	

		// Change position
		change = (nodebox->position.Z + ((float)nodebox->size.Z / (float)2))-opp; 
		nodebox->position.Z += change;
		break;
	case CDR_Z_N:
		// Resize
		tsize=nodebox->size; // Save the size		
		opp = nodebox->position.Z - ((float)nodebox->size.Z / (float)2); // Get the corner before			
		tsize.Z += opp - target.Z; // Adjust the size to now
		if (tsize.Z <= 0) // Validate
			tsize.Z=NODE_THIN;
		if (tsize.Z > 1) // Validate
			tsize.Z=1;
		setsizeObject(nodebox,tsize.X,tsize.Y,tsize.Z); // Resize	

		// Change position
		change = (nodebox->position.Z - ((float)nodebox->size.Z / (float)2))-opp; 
		nodebox->position.Z += change;
		break;
	}

	return;
}

// This build individual nodebox tables
stringc* cNode::build(sBox* nodebox){
	if (!nodebox)
		return NULL;

	stringc* res = new stringc();
	res->append("{");

	// Bounds one
	res->append(stringc(nodebox->position.X - ((float)nodebox->size.X / (float)2)));
	res->append(",");
	res->append(stringc(nodebox->position.Y - ((float)nodebox->size.Y / (float)2)));
	res->append(",");
	res->append(stringc(nodebox->position.Z - ((float)nodebox->size.Z / (float)2)));
	res->append(",");

	// Bounds two
	res->append(stringc(nodebox->position.X + ((float)nodebox->size.X / (float)2)));
	res->append(",");
	res->append(stringc(nodebox->position.Y + ((float)nodebox->size.Y / (float)2)));
	res->append(",");
	res->append(stringc(nodebox->position.Z + ((float)nodebox->size.Z / (float)2)));

	res->append("}");
	return res;
}

// This puts all the nodeboxes together
stringc* cNode::build(BUILD_TYPE type){
	
	stringc* res = new stringc();

	if (type >= NBT_NBS)
		res->append("minetest.register_node(\"test:node\",{\n");

	if (type >= NBT_NBS)
		if (type == NBT_FULL)
			res->append("\tdrawtype=\"nodebox\",\n\tnodebox = {\n");
		else
			res->append("drawtype=\"nodebox\",\nnodebox = {\n");

	int a;
	for (a=0;a<number;a++){
		stringc tmp = *build(boxes[a]);

		if (tmp!=NULL){
			for (int i=0;i<type-1;i++){
				res->append("\t");
				printf("tab");
			}
			res->append(tmp);
			res->append("\n");
		}
	}

	if (type >= NBT_NBS)
		if (type == NBT_FULL)
			res->append("\t}\n");
		else
			res->append("}\n");

	if (type == NBT_FULL)
		res->append("})");

	// Print result
	printf("\n===== res =====\n");
	printf("%s\n",res->c_str());
	printf("===============\n");
	return res;
}
