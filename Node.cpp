#include "Node.h"

// Construction / Destruction
Node::Node(IrrlichtDevice* mdevice, ed_data* n_ed)
:device(mdevice),editor(n_ed),_id(0),number(0)
{
	driver = device->getVideoDriver();
	smgr = device->getSceneManager();

	nd_position = vector3df(0,0,0);

	for (int i=0;i<NODEB_MAX;i++){
		boxes[i]=NULL;
	}

	for (int i=0;i<17;i++){
		snappers[i]=NULL;
	}

	printf("Making 16 pixel snap grid: \n");

	for (int a=-(NODE_RES/2);a<(NODE_RES/2)+1;a++){
		snappers[a+(NODE_RES/2)] = a*((float)1/(float)NODE_RES);
		printf(">> %f\n",snappers[a+8]);
	}

	defrag();
}

Node::~Node(){
	for (int i=0;i<NODEB_MAX;i++){
		if (boxes[i])
			delete boxes[i];
	}
}


// Get functions
int Node::GetId(){
	return _id;
}

NodeBox* Node::GetCurrentNodeBox(){
	return GetNodeBox(GetId());
}

NodeBox* Node::GetNodeBox(int id){
	return boxes[id];
}

// Operation functions
NodeBox* Node::addNodeBox(){
	if (!number)
		number = 0;

	// Name it
	core::stringw nb=L"NodeBox ";
	nb+=(number+1);

	// Set up structure
	NodeBox* tmp =new NodeBox(nb,vector3df(0,-0.5,-0.5),vector3df(0.5,0.5,0.5));

	if (!tmp)
		return NULL;

	boxes[number] = tmp;
	select(number);
	update(number);

	// Increment and print message
	number++;
	printf("Nodebox added\n");

	// Clean up list	
	defrag();	

	return tmp;
}

void Node::select(int id){
	_id = id;
}

void Node::update(){
	if (editor->type==TOOL_NODEB){
		if (GetCurrentNodeBox() && number>GetId()){
			NodeBox* tmp = GetCurrentNodeBox();

			// Name label
			core::stringw nb=L"NodeBox: ";
			nb+=tmp->name;
			editor->d_nb->setText(nb.c_str());

			// One label
			core::stringw ps=L"One: ";
			ps+=tmp->one.X;
			ps+=" , ";
			ps+=tmp->one.Y;
			ps+=" , ";
			ps+=tmp->one.Z;
			editor->d_pos->setText(ps.c_str());

			// Two label
			core::stringw rt=L"Two: ";
			rt+=tmp->two.X;
			rt+=" , ";
			rt+=tmp->two.Y;
			rt+=" , ";
			rt+=tmp->two.Z;
			editor->d_rot->setText(rt.c_str());
		}else{
			editor->d_nb->setText(L"NodeBox: -");
			editor->d_pos->setText(L"One: - , - , -");
			editor->d_rot->setText(L"Two: - , - , -");
		}
	}else if (editor->type==TOOL_NODE){
		editor->d_nb->setText(L"Node: -");
		editor->d_pos->setText(L"Type: -");
		editor->d_rot->setText(L"Boxes: -");
	}else{
		editor->d_nb->setText(L"EDITOR MODE UNAVAILABLE");
		editor->d_pos->setText(L"This tool has not yet been coded.");
		editor->d_rot->setText(L"Use a newer version if available.");
		return;
	}
}

void Node::resizeNodeBoxFace(CDR_TYPE type,vector3df position,bool both){
	NodeBox* node = GetCurrentNodeBox();
	if (!node)
		return;

	position.X-=getPosition().X;
	position.Y-=getPosition().Y;
	position.Z-=getPosition().Z;

	// Do snapping
	if (editor->snapping==true){
		for (int i=0;i<15;i++){
			if (position.X > snappers[i]-0.0313 && position.X < snappers[i]+0.0313){
				position.X = snappers[i];
			}
			if (position.Y > snappers[i]-0.0313 && position.Y < snappers[i]+0.0313){
				position.Y = snappers[i];
			}
			if (position.Z > snappers[i]-0.0313 && position.Z < snappers[i]+0.0313){
				position.Z = snappers[i];
			}
		}
	}

	// Do node limiting
	if (editor->limiting==true){
		// X Axis
		if (position.X < -0.5)
			position.X = -0.5;
		else if (position.X > 0.5)
			position.X = 0.5;

		// Y Axis
		if (position.Y < -0.5)
			position.Y = -0.5;
		else if (position.Y > 0.5)
			position.Y = 0.5;

		// Z Axis
		if (position.Z < -0.5)
			position.Z = -0.5;
		else if (position.Z > 0.5)
			position.Z = 0.5;
	}

	switch(type){
	case CDR_X_P:
		if (both){
			printf("Doing both... \n");

			f32 new_opp = node->one.X - (position.X - node->two.X);

			if (editor->limiting==true){
				if (new_opp > 0.5 || new_opp < -0.5)
					return;
			}

			node->one.X = new_opp;
		}
		node->two.X = position.X;
		break;
	case CDR_X_N:
		if (both){
			printf("Doing both... \n");

			f32 new_opp = node->two.X - (position.X - node->one.X);

			if (editor->limiting==true){
				if (new_opp > 0.5 || new_opp < -0.5)
					return;
			}

			node->two.X = new_opp;
		}
		node->one.X = position.X;
		break;
	case CDR_Y_P:
		if (both){
			printf("Doing both... \n");

			f32 new_opp = node->one.Y - (position.Y - node->two.Y);

			if (editor->limiting==true){
				if (new_opp > 0.5 || new_opp < -0.5)
					return;
			}

			node->one.Y = new_opp;
		}
		node->two.Y = position.Y;
		break;
	case CDR_Y_N:
		if (both){
			printf("Doing both... \n");

			f32 new_opp = node->two.Y - (position.Y - node->one.Y);

			if (editor->limiting==true){
				if (new_opp > 0.5 || new_opp < -0.5)
					return;
			}

			node->two.Y = new_opp;
		}
		node->one.Y = position.Y;
		break;
	case CDR_Z_P:
		if (both){
			printf("Doing both... \n");

			f32 new_opp = node->one.Z - (position.Z - node->two.Z);

			if (editor->limiting==true){
				if (new_opp > 0.5 || new_opp < -0.5)
					return;
			}

			node->one.Z = new_opp;
		}
		node->two.Z = position.Z;
		break;
	case CDR_Z_N:
		if (both){
			printf("Doing both... \n");

			f32 new_opp = node->two.Z - (position.Z - node->one.Z);

			if (editor->limiting==true){
				if (new_opp > 0.5 || new_opp < -0.5)
					return;
			}

			node->two.Z = new_opp;
		}
		node->one.Z = position.Z;
		break;
	}

	update(GetId());
}

void Node::moveNodeBox(CDR_TYPE type,vector3df position){
	NodeBox* node = GetCurrentNodeBox();
	if (!node)
		return;

	position.X-=getPosition().X;
	position.Y-=getPosition().Y;
	position.Z-=getPosition().Z;

	vector3df new_one = node->one;
	vector3df new_two = node->two;
	vector3df move_dist = vector3df(0,0,0);

	switch(type){
	case CDR_XZ:		
		move_dist = vector3df(
			position.X - node->GetCenter().X,
			0,
			position.Z - node->GetCenter().Z
		);
		break;
	case CDR_XY:		
		move_dist = vector3df(
			position.X - node->GetCenter().X,
			position.Y - node->GetCenter().Y,
			0
		);
		break;
	case CDR_ZY:		
		move_dist = vector3df(
			0,
			position.Y - node->GetCenter().Y,
			position.Z - node->GetCenter().Z
		);
		break;
	}

	if (move_dist.X != 0){
		if ((new_one.X+move_dist.X <= 0.5 && new_one.X+move_dist.X >= -0.5 && new_two.X+move_dist.X <= 0.5 && new_two.X+move_dist.X >= -0.5)||editor->limiting==false){
			new_one.X+=move_dist.X;	
			new_two.X+=move_dist.X;		
		}
	}

	if (move_dist.Y != 0){
		if ((new_one.Y+move_dist.Y <= 0.5 && new_one.Y+move_dist.Y >= -0.5 && new_two.Y+move_dist.Y <= 0.5 && new_two.Y+move_dist.Y >= -0.5)||editor->limiting==false){
			new_one.Y+=move_dist.Y;	
			new_two.Y+=move_dist.Y;	
		}
	}

	if (move_dist.Z != 0){
		if ((new_one.Z+move_dist.Z <= 0.5 && new_one.Z+move_dist.Z >= -0.5 && new_two.Z+move_dist.Z <= 0.5 && new_two.Z+move_dist.Z >= -0.5)||editor->limiting==false){
			new_one.Z+=move_dist.Z;	
			new_two.Z+=move_dist.Z;	
		}
	}

	node->one = new_one;
	node->two = new_two;

	update(GetId());
}

void Node::deleteNodebox(int id){
	if (!GetNodeBox(id))
		return;

	delete boxes[id];
	boxes[id] = NULL;

	defrag();
}

vector3df Node::getPosition() const{
	return nd_position;
}

void Node::setPosition(vector3df in){
	nd_position = in;
}

// Build node models
void Node::update(int id){
	NodeBox* box = GetNodeBox(id);

	if (!box)
		return;

	if (box->model){
		box->model->remove();
		box->model = NULL;
	}

	vector3df position = vector3df(
			nd_position.X + box->one.X + ((box->two.X - box->one.X)/2),
			nd_position.Y + box->one.Y + ((box->two.Y - box->one.Y)/2),
			nd_position.Z + box->one.Z + ((box->two.Z - box->one.Z)/2)
		);

	vector3df size = vector3df(
			box->two.X - box->one.X,
			box->two.Y - box->one.Y,
			box->two.Z - box->one.Z
		);

	// Create the scene node
	box->model=smgr->addCubeSceneNode(1,0,-1,position);

	if (!box->model)
		return;
	
	box->model->setMaterialTexture(0, driver->getTexture("texture_box.png"));
	box->model->setMaterialFlag(video::EMF_BILINEAR_FILTER, false);	
	box->model->setName(box->name);
	box->model->setScale(size);
}

void Node::remesh(){
	for (int i=0;i<NODEB_MAX;i++){
		update(i);
	}
}

// Build code 
// This build individual nodebox tables
stringc* Node::build(NodeBox* nodebox){
	if (!nodebox)
		return NULL;

	stringc* res = new stringc();
	res->append("{");

	// Bounds one
	res->append(stringc(nodebox->one.X));
	res->append(",");
	res->append(stringc(nodebox->one.Y));
	res->append(",");
	res->append(stringc(nodebox->one.Z));
	res->append(",");

	// Bounds two
	res->append(stringc(nodebox->two.X));
	res->append(",");
	res->append(stringc(nodebox->two.Y));
	res->append(",");
	res->append(stringc(nodebox->two.Z));

	res->append("}");
	res->append(", --");
	res->append(nodebox->name);

	return res;
}

// This puts all the nodeboxes together
stringc* Node::build(BUILD_TYPE type){
	
	stringc* res = new stringc();

	if (type == NBT_FULL)
		res->append("minetest.register_node(\"test:node\",{\n");

	if (type >= NBT_NBS)
		if (type == NBT_FULL)
			res->append("\tdrawtype=\"nodebox\",\n\tparamtype = \"light\",\n\tnode_box = {\n\t\ttype = \"fixed\",\n\t\tfixed = {\n");
		else
			res->append("drawtype=\"nodebox\",\nparamtype = \"light\",\nnode_box = {\n\ttype = \"fixed\",\n\tfixed = {\n");

	int a;
	for (a=0;a<number;a++){
		stringc tmp = *build(boxes[a]);

		if (tmp!=NULL){
			for (int i=0;i<type;i++){
				res->append("\t");
			}
			res->append(tmp);
			res->append("\n");
		}
	}

	if (type >= NBT_NBS)
		if (type == NBT_FULL)
			res->append("\t\t}\n\t}\n");
		else
			res->append("\t}\n}\n");

	if (type == NBT_FULL)
		res->append("})");

	// Print result
	printf("\n===== res =====\n");
	printf("%s\n",res->c_str());
	printf("===============\n");
	return res;
}

// Private functions
void Node::defrag(){
	bool sep=false;

	// Delete context menu stuff
	if (editor->menu)
	{
		editor->menu->removeAllItems();
		editor->menu->addItem(L"Node properties",-1,false);
		editor->menu->addSeparator();
		editor->menu->addItem(L"Add a nodebox",GUI_ID_BOX);
		editor->menu->addItem(L"Nodebox properties",GUI_ID_TEXT);
		editor->menu->addItem(L"Delete current nodebox",GUI_ID_DELETENB);
	}

	int a=0;
	for (int i=0;i<NODEB_MAX;i++){
		if (boxes[i]!=NULL){
			boxes[a]=boxes[i];

			// Add menu item
			if (editor->menu)
			{
				if (sep==false){
					sep=true;
					editor->menu->addSeparator();
				}
				editor->menu->addItem(convert(boxes[a]->model->getName()),250+a);
			}

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
		}else
			printf("%i> NULL \n",i);
	}

	printf("There are %i boxes\n",a);
#endif
}