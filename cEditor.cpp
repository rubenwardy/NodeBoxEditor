#include "cEditor.h"
#include <iostream>

enum GUI_ID
{
	// File
	GUI_ID_NEW=201,
	GUI_ID_LOAD=202,
	GUI_ID_SAVE=203,
	GUI_ID_IMPORT=204,
	GUI_ID_EX=205,

	// Node
	GUI_ID_ADDNODE=207,
	GUI_ID_SWITCH=208,
	GUI_ID_BOX=209,

	// Help
	GUI_ID_HELP=200,

	// View
	GUI_ID_SP_ALL=210,
	GUI_ID_SP_PER=211,
	GUI_ID_SP_TOP=212,
	GUI_ID_SP_FRT=213,
	GUI_ID_SP_RHT=214,
};

cEditor::cEditor(){
	for (int i=0;i<4;i++){
		camera[i]=NULL;
	}

	isSplitScreen=true;
	currentWindow=0;
}

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

	// Calculate Projection Matrix
	matrix4 projMat;
	irr::f32 orth_w = (float)driver->getScreenSize().Width / (float)driver->getScreenSize().Height;
	orth_w = 3 * orth_w;
	projMat.buildProjectionMatrixOrthoLH(orth_w,3,1,10);
	
	// Add rotational camera
	pivot=smgr->addEmptySceneNode(0,199);
	pivot->setPosition(vector3df(0,0,0));
	camera[0]=smgr->addCameraSceneNode(pivot,vector3df(0,0,-2),vector3df(0,0,0));
	smgr->setActiveCamera(camera[0]);
	pivot->setRotation(vector3df(25,-45,0));

	// Add Topdown camera
	camera[1]=smgr->addCameraSceneNode(NULL,vector3df(0,2,0),vector3df(0,0,0));
	camera[1]->setProjectionMatrix(projMat,true);

	// Add front camera
	camera[2]=smgr->addCameraSceneNode(NULL,vector3df(0,0,-5),vector3df(0,0,0));
	camera[2]->setProjectionMatrix(projMat,true);

	// Add side camera
	camera[3]=smgr->addCameraSceneNode(NULL,vector3df(-5,0,0),vector3df(0,0,0));
	camera[3]->setProjectionMatrix(projMat,true);

	// Add Light
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
	
		int ResX = driver->getScreenSize().Width;
		int ResY = driver->getScreenSize().Height;

		if (isSplitScreen==true)
		{
			// Draw Camera 0
			if (camera[0]){
				smgr->setActiveCamera(camera[0]);
				driver->setViewPort(rect<s32>(0,0,ResX/2,ResY/2));
				smgr->drawAll();
			}

			// Draw Camera 1
			if (camera[1]){
				smgr->setActiveCamera(camera[1]);
				driver->setViewPort(rect<s32>(ResX/2,0,ResX,ResY/2));
				smgr->drawAll();
			}

			// Draw Camera 2
			if (camera[2]){
				smgr->setActiveCamera(camera[2]);
				driver->setViewPort(rect<s32>(0,ResY/2,ResX/2,ResY));
				smgr->drawAll();
			}

			// Draw Camera 3
			if (camera[3]){
				smgr->setActiveCamera(camera[3]);
				driver->setViewPort(rect<s32>(ResX/2,ResY/2,ResX,ResY));				
				smgr->drawAll();
			}
		}else if (camera[currentWindow]){
			smgr->setActiveCamera(camera[currentWindow]);
			driver->setViewPort(rect<s32>(0,0,ResX,ResY));			
			smgr->drawAll();
		}
  
		driver->setViewPort(rect<s32>(0,0,ResX,ResY));
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
	int tmp_b=driver->getScreenSize().Height-46;
	data->d_nb=guienv->addStaticText(L"NodeBox: -",rect<s32>(5,tmp_b,300,tmp_b+15));
	data->d_pos=guienv->addStaticText(L"Position: - , - , -",rect<s32>(5,tmp_b+15,300,tmp_b+30));
	data->d_rot=guienv->addStaticText(L"Size: - , - , -",rect<s32>(5,tmp_b+30,300,tmp_b+45));

	data->d_nb->setAlignment(EGUIA_UPPERLEFT, EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT);
	data->d_pos->setAlignment(EGUIA_UPPERLEFT, EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT);
	data->d_rot->setAlignment(EGUIA_UPPERLEFT, EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT);


	// The Menu
	IGUIContextMenu* menubar=guienv->addMenu();
	menubar->addItem(L"File",-1,true,true);
	//menubar->addItem(L"Edit",-1,true,true);
	menubar->addItem(L"View",-1,true,true);
	menubar->addItem(L"Project",-1,true,true);
	menubar->addItem(L"Node",-1,true,true);
	menubar->addItem(L"Help",-1,true,true);

	gui::IGUIContextMenu* submenu;

	// File
 submenu = menubar->getSubMenu(0);
	submenu->addItem(L"New",GUI_ID_NEW);
	submenu->addItem(L"Load",GUI_ID_LOAD);
	submenu->addItem(L"Save",GUI_ID_SAVE);
	submenu->addSeparator();
	submenu->addItem(L"Import",GUI_ID_IMPORT);
	submenu->addItem(L"Export",GUI_ID_EX); 

	// View
	submenu = menubar->getSubMenu(1);
	submenu->addItem(L"Tiles",GUI_ID_SP_ALL);
	submenu->addSeparator();
	submenu->addItem(L"Perspective",GUI_ID_SP_PER);
	submenu->addItem(L"Top",GUI_ID_SP_TOP);
	submenu->addItem(L"Front",GUI_ID_SP_FRT);
	submenu->addItem(L"Right",GUI_ID_SP_RHT);		

	// Project
	submenu = menubar->getSubMenu(2);
	submenu->addItem(L"Add a Node");
	submenu->addItem(L"Switch Node",GUI_ID_SWITCH);

	// Node
	submenu = menubar->getSubMenu(3);
	submenu->addItem(L"Add a Node Box",GUI_ID_BOX);
	submenu->addItem(L"Delete a Node Box",GUI_ID_BOX);
	submenu->addSeparator();
	submenu->addItem(L"Set texture...");

	// Help
	submenu = menubar->getSubMenu(4);
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
	switch(id)
	{
	case GUI_ID_HELP:
		printf("Showing help dialog\n");
		guienv->addMessageBox(L"Help",L"Use insert>node box to add a node box. Use arrows to move the selected box, and wasd to resize it.");
		break;
	case GUI_ID_BOX:
		nodes[curId]->addNodeBox();
		break;
	case GUI_ID_SP_ALL:
		printf("View mode changed to tiles\n");
		isSplitScreen=true;
		currentWindow=0;
		break;
	case GUI_ID_SP_PER:
		printf("View mode changed to perspective\n");
		isSplitScreen=false;
		currentWindow=0;
		break;
	case GUI_ID_SP_TOP:
		printf("View mode changed to top\n");
		isSplitScreen=false;
		currentWindow=1;
		break;
	case GUI_ID_SP_FRT:
		printf("View mode changed to front\n");
		isSplitScreen=false;
		currentWindow=2;
		break;
	case GUI_ID_SP_RHT:
		printf("View mode changed to right\n");
		isSplitScreen=false;
		currentWindow=3;
		break;
	}
}