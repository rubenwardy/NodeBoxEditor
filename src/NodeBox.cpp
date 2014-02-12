#include "NodeBox.h"

void NodeBox::resizeNodeBoxFace(EditorState* editor,CDR_TYPE type,vector3df position,bool both){
	switch(type){
	case CDR_X_P:
		if (both){

			f32 new_opp = one.X - (position.X - two.X);

			if (editor->Settings()->getSettingAsBool("limiting")==true){
				if (new_opp > 0.5 || new_opp < -0.5)
					return;
			}

			one.X = new_opp;
		}
		two.X = position.X;
		break;
	case CDR_X_N:
		if (both){
			f32 new_opp = two.X - (position.X - one.X);

			if (editor->Settings()->getSettingAsBool("limiting")==true){
				if (new_opp > 0.5 || new_opp < -0.5)
					return;
			}

			two.X = new_opp;
		}
		one.X = position.X;
		break;
	case CDR_Y_P:
		if (both){
			f32 new_opp = one.Y - (position.Y - two.Y);

			if (editor->Settings()->getSettingAsBool("limiting")==true){
				if (new_opp > 0.5 || new_opp < -0.5)
					return;
			}

			one.Y = new_opp;
		}
		two.Y = position.Y;
		break;
	case CDR_Y_N:
		if (both){
			f32 new_opp = two.Y - (position.Y - one.Y);

			if (editor->Settings()->getSettingAsBool("limiting")==true){
				if (new_opp > 0.5 || new_opp < -0.5)
					return;
			}

			two.Y = new_opp;
		}
		one.Y = position.Y;
		break;
	case CDR_Z_P:
		if (both){
			f32 new_opp = one.Z - (position.Z - two.Z);

			if (editor->Settings()->getSettingAsBool("limiting")==true){
				if (new_opp > 0.5 || new_opp < -0.5)
					return;
			}

			one.Z = new_opp;
		}
		two.Z = position.Z;
		break;
	case CDR_Z_N:
		if (both){
			f32 new_opp = two.Z - (position.Z - one.Z);

			if (editor->Settings()->getSettingAsBool("limiting")==true){
				if (new_opp > 0.5 || new_opp < -0.5)
					return;
			}

			two.Z = new_opp;
		}
		one.Z = position.Z;
		break;
	}
}

void NodeBox::moveNodeBox(EditorState* editor,CDR_TYPE type,vector3df position){
	vector3df new_one = one;
	vector3df new_two = two;
	vector3df move_dist = vector3df(0,0,0);

	switch(type){
	case CDR_XZ:
		move_dist = vector3df(
			position.X - GetCenter().X,
			0,
			position.Z - GetCenter().Z
		);
		break;
	case CDR_XY:
		move_dist = vector3df(
			position.X - GetCenter().X,
			position.Y - GetCenter().Y,
			0
		);
		break;
	case CDR_ZY:
		move_dist = vector3df(
			0,
			position.Y - GetCenter().Y,
			position.Z - GetCenter().Z
		);
		break;
	}

	if (move_dist.X != 0){
		if ((
				new_one.X+move_dist.X <= 0.5 &&
				new_one.X+move_dist.X >= -0.5 &&
				new_two.X+move_dist.X <= 0.5 &&
				new_two.X+move_dist.X >= -0.5)||
				!editor->Settings()->getSettingAsBool("limiting")
			){
			new_one.X+=move_dist.X;
			new_two.X+=move_dist.X;
		}
	}

	if (move_dist.Y != 0){
		if ((
				new_one.Y+move_dist.Y <= 0.5 &&
				new_one.Y+move_dist.Y >= -0.5 &&
				new_two.Y+move_dist.Y <= 0.5 &&
				new_two.Y+move_dist.Y >= -0.5)||
				!editor->Settings()->getSettingAsBool("limiting")
			){
			new_one.Y+=move_dist.Y;
			new_two.Y+=move_dist.Y;
		}
	}

	if (move_dist.Z != 0){
		if ((
				new_one.Z+move_dist.Z <= 0.5 &&
				new_one.Z+move_dist.Z >= -0.5 &&
				new_two.Z+move_dist.Z <= 0.5 &&
				new_two.Z+move_dist.Z >= -0.5)||
				!editor->Settings()->getSettingAsBool("limiting")){
			new_one.Z+=move_dist.Z;
			new_two.Z+=move_dist.Z;
		}
	}

	one = new_one;
	two = new_two;
}

void NodeBox::buildNode(vector3di nd_position,IrrlichtDevice* device){
    video::IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();

	if (model){
		model->remove();
		model = NULL;
	}

	vector3df position = vector3df(
			nd_position.X + one.X + ((two.X - one.X)/2),
			nd_position.Y + one.Y + ((two.Y - one.Y)/2),
			nd_position.Z + one.Z + ((two.Z - one.Z)/2)
		);

	vector3df size = vector3df(
			two.X - one.X,
			two.Y - one.Y,
			two.Z - one.Z
		);

	// Create the scene node
	model=smgr->addCubeSceneNode(1,0,-1,position);

	if (!model)
		return;

	model->setMaterialTexture(0, driver->getTexture("media/texture_box.png"));
	model->setMaterialFlag(video::EMF_BILINEAR_FILTER, false);
	model->setName(name);
	model->setScale(size);
}
