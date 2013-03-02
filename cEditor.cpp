#include "cEditor.h"
#include <iostream>

enum
{
	GUI_ID_HELP=200,
	GUI_ID_NEW=201,
	GUI_ID_LOAD=202,
	GUI_ID_SAVE=203,
	GUI_ID_IMPORT=204,
	GUI_ID_EX_PROJ=205,
	GUI_ID_EX_NODE=206,
	GUI_ID_ADDNODE=207,
	GUI_ID_SWITCH=208,
	GUI_ID_BOX=209,
};

bool cEditor::run(IrrlichtDevice* irr_device){
	// Get Pointers
	device=irr_device;
	driver = device->getVideoDriver();
	smgr = device->getSceneManager();
	guienv = device->getGUIEnvironment();

	data=new ed_data();

	loadUI();

	coli=smgr->getSceneCollisionManager();
	device->setWindowCaption(L"The NodeBox Generator");
	device->setResizable(true);
	
	//Add Camera and Pivot
	pivot=smgr->addEmptySceneNode(0,199);
	pivot->setPosition(vector3df(0,0,0));
	camera=smgr->addCameraSceneNode(pivot,vector3df(0,0,-2));
	camera->setTarget(vector3df(0,0,0));
	smgr->setActiveCamera(camera);

	//Add Light
	ILightSceneNode* light=smgr->addLightSceneNode(0,vector3df(25,50,0));
	light->setLightType(ELT_POINT);
	light->setRadius(2000);

	//Add Plane
	IMeshSceneNode* plane = smgr->addCubeSceneNode(1,0,-1,vector3df(0.5,-5.5,0.5),vector3df(0,0,0),vector3df(10,10,10));
	plane->setMaterialTexture(0, driver->getTexture("texture_terrain.png"));
    plane->setMaterialFlag(video::EMF_BILINEAR_FILTER, false);
	plane->getMaterial(0).getTextureMatrix(0).setTextureScale(10,10);

	//Setup Current Manager
	nodes[0]=new cNode(device,data);
	curId=0;

	unsigned int counter=0;

	while (device->run()){
		counter++;
		driver->beginScene(true, true, irr::video::SColor(255,100,101,140));
		smgr->drawAll();
		guienv->drawAll();
		driver->endScene();

		if (counter>500){	
			counter=0;
			if (nodes[curId])
				nodes[curId]->update();	
		}
	}

	return true;
}

void cEditor::loadUI(){
	std::cout << "Loading the User Interface" << std::endl;
	guienv->clear();

	// The Status Text
	int tmp_b=driver->getScreenSize().Height-70;
	data->d_nb=guienv->addStaticText(L"NodeBox: -",rect<s32>(5,tmp_b,300,tmp_b+15));
	data->d_pos=guienv->addStaticText(L"Position: - , - , -",rect<s32>(5,tmp_b+15,300,tmp_b+30));
	data->d_rot=guienv->addStaticText(L"Rotation: - , - , -",rect<s32>(5,tmp_b+30,300,tmp_b+60));

	data->d_nb->setAlignment(EGUIA_UPPERLEFT, EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT);
	data->d_pos->setAlignment(EGUIA_UPPERLEFT, EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT);
	data->d_rot->setAlignment(EGUIA_UPPERLEFT, EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT);


	// The Menu
	IGUIContextMenu* menubar=guienv->addMenu();
	menubar->addItem(L"Project",-1,true,true);
	menubar->addItem(L"Node",-1,true,true);
	menubar->addItem(L"Help",-1,true,true);

	gui::IGUIContextMenu* submenu;

    submenu = menubar->getSubMenu(0);
	submenu->addItem(L"New",GUI_ID_NEW);
	submenu->addItem(L"Load",GUI_ID_LOAD);
	submenu->addItem(L"Save",GUI_ID_SAVE);
	submenu->addSeparator();
	submenu->addItem(L"Import",GUI_ID_IMPORT);
	submenu->addItem(L"Export",GUI_ID_EX_PROJ);
	

    submenu = menubar->getSubMenu(1);
	submenu->addItem(L"Add a Node",GUI_ID_ADDNODE);
	submenu->addItem(L"Switch Node",GUI_ID_SWITCH);
	submenu->addSeparator();
	submenu->addItem(L"Add a Node Box",GUI_ID_BOX);
	submenu->addSeparator();
	submenu->addItem(L"Export",GUI_ID_EX_NODE);
	

	submenu = menubar->getSubMenu(2);
	submenu->addItem(L"Help",GUI_ID_HELP);
	submenu->addSeparator();
	submenu->addItem(L"About");
}

bool cEditor::OnEvent(const SEvent& event)
{
	if (event.EventType == irr::EET_MOUSE_INPUT_EVENT &&
		event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP ){
		
		ISceneNode* hit=coli->getSceneNodeFromScreenCoordinatesBB(core::vector2d<s32>(event.MouseInput.X,event.MouseInput.Y));

		if (hit==0){
			std::cout << "No Nodes at that position" << std::endl;
		}else{
			nodes[curId]->switchFocus(hit);
		}

	}else if (event.EventType == EET_KEY_INPUT_EVENT){

		if (event.KeyInput.Key== KEY_DOWN){
			pivot->setRotation(vector3df(pivot->getRotation().X-1,pivot->getRotation().Y,pivot->getRotation().Z));
		}else if (event.KeyInput.Key== KEY_UP){
			pivot->setRotation(vector3df(pivot->getRotation().X+1,pivot->getRotation().Y,pivot->getRotation().Z));
		}else if (event.KeyInput.Key== KEY_LEFT){
			pivot->setRotation(vector3df(pivot->getRotation().X,pivot->getRotation().Y+1,pivot->getRotation().Z));
		}else if (event.KeyInput.Key== KEY_RIGHT){
			pivot->setRotation(vector3df(pivot->getRotation().X,pivot->getRotation().Y-1,pivot->getRotation().Z));

		}else if (event.KeyInput.Key== KEY_KEY_A){
			if (nodes[curId])
				nodes[curId]->resize(0,0.05);
		}else if (event.KeyInput.Key== KEY_KEY_Z){
			if (nodes[curId])
				nodes[curId]->resize(0,-0.05);
		}
		

	}else if (event.EventType == EET_GUI_EVENT){
		
		irr::s32 id = event.GUIEvent.Caller->getID();
		irr::gui::IGUIEnvironment* env = device->getGUIEnvironment();

			switch(event.GUIEvent.EventType)
			{
			case EGET_MENU_ITEM_SELECTED:
				OnMenuItemSelected( (IGUIContextMenu*)event.GUIEvent.Caller );
				break;
			case irr::gui::EGET_BUTTON_CLICKED:
					switch(id)
					{
					case 0:
						break;
					default:
						return false;
					}
				break;
			default:
				break;
			}
	}

	return false;
}

 void cEditor::OnMenuItemSelected( IGUIContextMenu* menu )
{
	s32 id = menu->getItemCommandId(menu->getSelectedItem());
	IGUIEnvironment* env = device->getGUIEnvironment();
	std::cout << "MenuItem Selected" << std::endl;
	switch(id)
	{
	case GUI_ID_HELP:
		std::cout << "--Help" << std::endl;
		guienv->addMessageBox(L"Help",L"Use insert>node box to add a node box. Use arrows to move the selected box, and wasd to resize it.");
		break;
	case GUI_ID_BOX:
		std::cout << "--Node Box Added" << std::endl;
		nodes[curId]->addNodeBox();
		break;
	}
}