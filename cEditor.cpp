#include "cEditor.h"
#include <iostream>

cEditor::cEditor(){
	for (int i=0;i<4;i++){
		camera[i]=NULL;
	}

	for (int i=0;i<12;i++){
		points[i]=NULL;
	}

	for (int i=0;i<5;i++){
		nodes[i]=NULL;
	}

	isSplitScreen=true;
	currentWindow=0;
	point_on=-1;
	mouse_down=false;
}

bool cEditor::run(IrrlichtDevice* irr_device){
	// Get Pointers
	device=irr_device;
	driver = device->getVideoDriver();
	smgr = device->getSceneManager();
	guienv = device->getGUIEnvironment();
	
	data=new ed_data();
	data->snapping = true;
	data->limiting = true;

	coli=smgr->getSceneCollisionManager();
	device->setWindowCaption(L"The NodeBox Generator");
	device->setResizable(true);

	// Calculate Projection Matrix
	matrix4 projMat;
	irr::f32 orth_w = (float)driver->getScreenSize().Width / (float)driver->getScreenSize().Height;
	orth_w = 3 * orth_w;
	projMat.buildProjectionMatrixOrthoLH(orth_w,3,1,100);
	
	// Add rotational camera
	pivot=smgr->addEmptySceneNode(0,199);
	pivot->setPosition(vector3df(0,0,0));
	camera[0]=smgr->addCameraSceneNode(pivot,vector3df(0,0,-2),vector3df(0,0,0));
	smgr->setActiveCamera(camera[0]);
	pivot->setRotation(vector3df(25,-45,0));

	// Add Topdown camera
	camera[1]=smgr->addCameraSceneNode(NULL,vector3df(0,2,-0.01),vector3df(0,0,0));
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

	// Add Plane
	IMeshSceneNode* plane = smgr->addCubeSceneNode(1,0,-1,vector3df(0.5,-5.5,0.5),vector3df(0,0,0),vector3df(10,10,10));
	plane->setMaterialTexture(0, driver->getTexture("texture_terrain.png"));
	plane->setMaterialFlag(video::EMF_BILINEAR_FILTER, false);
	plane->getMaterial(0).getTextureMatrix(0).setTextureScale(10,10);

	// Add sky box
	scene::IMeshSceneNode* skybox=smgr->addCubeSceneNode(50);
	skybox->setMaterialTexture(0, driver->getTexture("sky.jpg"));
	skybox->setMaterialFlag(video::EMF_BILINEAR_FILTER, false);
	skybox->setMaterialFlag(video::EMF_LIGHTING,false);
	smgr->getMeshManipulator()->flipSurfaces(skybox->getMesh());
	plane_tri=smgr->createOctreeTriangleSelector(skybox->getMesh(),skybox);

	//Setup Current Manager
	nodes[0]=new cNode(device,data);
	curId=0;

	loadUI();

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
				
				updatePoint(0,4);
			}

			// Draw Camera 2
			if (camera[2]){
				smgr->setActiveCamera(camera[2]);
				driver->setViewPort(rect<s32>(0,ResY/2,ResX/2,ResY));
				smgr->drawAll();

				updatePoint(4,8);
			}

			// Draw Camera 3
			if (camera[3]){
				smgr->setActiveCamera(camera[3]);
				driver->setViewPort(rect<s32>(ResX/2,ResY/2,ResX,ResY));				
				smgr->drawAll();

				updatePoint(8,12);
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
	guienv->getSkin()->setFont(guienv->getFont("fontlucida.png"));

	// The Status Text
	int tmp_b=driver->getScreenSize().Height-46;
	data->d_nb=guienv->addStaticText(L"NodeBox: -",rect<s32>(5,tmp_b,300,tmp_b+15));
	data->d_pos=guienv->addStaticText(L"Position: - , - , -",rect<s32>(5,tmp_b+15,300,tmp_b+30));
	data->d_rot=guienv->addStaticText(L"Size: - , - , -",rect<s32>(5,tmp_b+30,300,tmp_b+45));

	data->d_nb->setAlignment(EGUIA_UPPERLEFT, EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT);
	data->d_pos->setAlignment(EGUIA_UPPERLEFT, EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT);
	data->d_rot->setAlignment(EGUIA_UPPERLEFT, EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT);

	// The Menu
	{
		IGUIContextMenu* menubar=guienv->addMenu();
		menubar->addItem(L"File",-1,true,true);
		menubar->addItem(L"Edit",-1,true,true);
		menubar->addItem(L"View",-1,true,true);
		menubar->addItem(L"Project",-1,true,true);
		menubar->addItem(L"Node",-1,true,true);
		menubar->addItem(L"Help",-1,true,true);
		gui::IGUIContextMenu* submenu;

		// File
		submenu = menubar->getSubMenu(0);
		submenu->addItem(L"New",GUI_ID_NEW,false);
		submenu->addItem(L"Load",GUI_ID_LOAD,false);
		submenu->addItem(L"Save",GUI_ID_SAVE,false);
		submenu->addSeparator();
		submenu->addItem(L"Import",GUI_ID_IMPORT,false);
		submenu->addItem(L"Export",-1,true,true);

		// Export
		submenu = submenu->getSubMenu(5);
		if (submenu){
			submenu->addItem(L"Project",GUI_ID_EX_PROJ);
			submenu->addSeparator();
			submenu->addItem(L"Entire Node",GUI_ID_EX_NODE);
			submenu->addItem(L"Nodebox table",GUI_ID_EX_NBS);
			submenu->addItem(L"Raw nodebox data",GUI_ID_EX_NB);
		}

		// View
		submenu = menubar->getSubMenu(1);
		submenu->addItem(L"Snap to grid",GUI_ID_SNAP,true,false,true,true);
		submenu->addItem(L"Limit to node size",GUI_ID_LIMIT,true,false,true,true);

		// View
		submenu = menubar->getSubMenu(2);
		submenu->addItem(L"Tiles",GUI_ID_SP_ALL);
		submenu->addSeparator();
		submenu->addItem(L"Perspective",GUI_ID_SP_PER);
		submenu->addItem(L"Top",GUI_ID_SP_TOP);
		submenu->addItem(L"Front",GUI_ID_SP_FRT);
		submenu->addItem(L"Right",GUI_ID_SP_RHT);

		// Project
		submenu = menubar->getSubMenu(3);
		submenu->addItem(L"Add a Node",GUI_ID_ADDNODE,false);
		submenu->addItem(L"Switch Node",GUI_ID_SWITCH,false);

		// Node
		submenu = menubar->getSubMenu(4);
		submenu->removeAllItems();
		submenu->addItem(L"Set texture...",-1,false);
		submenu->addSeparator();
		submenu->addItem(L"Add a nodebox",GUI_ID_BOX);
		submenu->addItem(L"Nodebox properties",GUI_ID_TEXT,false);
		submenu->addItem(L"Delete current nodebox",GUI_ID_DELETENB);

		for (int a=0;a<5;a++){
			if (nodes[a]!=NULL){
				nodes[a]->menu = submenu;
			}
		}

		// Help
		submenu = menubar->getSubMenu(5);
		submenu->addItem(L"Help",GUI_ID_HELP,false);
		submenu->addSeparator();
		submenu->addItem(L"About",GUI_ID_ABOUT);
	}

	// CDRs
	{
		// Add Topdown camera's CDRs
		addPoint(0,CDR_Z_P);
		addPoint(1,CDR_X_N);
		addPoint(2,CDR_X_P);
		addPoint(3,CDR_Z_N);
		// Add front camera's CDRs
		addPoint(4,CDR_Y_P);
		addPoint(5,CDR_X_N);
		addPoint(6,CDR_X_P);
		addPoint(7,CDR_Y_N);
		// Add side camera's CDRs
		addPoint(8,CDR_Y_P);
		addPoint(9,CDR_Z_P);
		addPoint(10,CDR_Z_N);
		addPoint(11,CDR_Y_N);
	}
}

bool cEditor::OnEvent(const SEvent& event)
{
	/*ISceneNode* hit=coli->getSceneNodeFromScreenCoordinatesBB(core::vector2d<s32>(event.MouseInput.X,event.MouseInput.Y));

	if (hit==0){
			std::cout << "No Nodes at that position" << std::endl;
	}else{
			nodes[curId]->switchFocus(hit);
	}*/

	if (event.EventType == irr::EET_MOUSE_INPUT_EVENT &&
		event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP ){
		mouse_down=false;

		// drop scale drag point
		if (point_on != -1)
			point_on = -1;
	}else if (event.EventType == irr::EET_MOUSE_INPUT_EVENT &&
		event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN ){
		mouse_down=true;
	}else if (event.EventType == irr::EET_MOUSE_INPUT_EVENT &&
		event.MouseInput.Event == EMIE_MOUSE_MOVED ){
			mouse_position.X=event.MouseInput.X;
			mouse_position.Y=event.MouseInput.Y;
	}else if (event.EventType == EET_KEY_INPUT_EVENT){
		if (event.KeyInput.Key== KEY_DOWN || event.KeyInput.Key== KEY_KEY_S){
			pivot->setRotation(vector3df(pivot->getRotation().X-1,pivot->getRotation().Y,pivot->getRotation().Z));
		}else if (event.KeyInput.Key== KEY_UP || event.KeyInput.Key== KEY_KEY_W){
			pivot->setRotation(vector3df(pivot->getRotation().X+1,pivot->getRotation().Y,pivot->getRotation().Z));
		}else if (event.KeyInput.Key== KEY_LEFT || event.KeyInput.Key== KEY_KEY_A){
			pivot->setRotation(vector3df(pivot->getRotation().X,pivot->getRotation().Y+1,pivot->getRotation().Z));
		}else if (event.KeyInput.Key== KEY_RIGHT || event.KeyInput.Key== KEY_KEY_D){
			pivot->setRotation(vector3df(pivot->getRotation().X,pivot->getRotation().Y-1,pivot->getRotation().Z));
		}
	}else if (event.EventType == EET_GUI_EVENT){
		
		irr::s32 id = event.GUIEvent.Caller->getID();
		irr::gui::IGUIEnvironment* env = device->getGUIEnvironment();

			switch(event.GUIEvent.EventType)
			{
			case EGET_MENU_ITEM_SELECTED:
				OnMenuItemSelected( (IGUIContextMenu*)event.GUIEvent.Caller );
				break;
			case EGET_ELEMENT_LEFT:
				if (id > 299 && id < 324){
					if (mouse_down == true && point_on == -1){	
						printf("Selected scaler id %i\n",id);
						point_on = id - 300;
					}
				}
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
	case GUI_ID_EX_NODE:
		{
			stringc* res = nodes[curId]->build(NBT_FULL);
			IGUIWindow* win = guienv->addWindow(rect<irr::s32>(50,50,50+320,50+320),false,L"Raw Code");
			IGUIEditBox* codebox = guienv->addEditBox(convert(res->c_str()),rect<irr::s32>(10,30,310,310),true,win);
			codebox->setMultiLine(true);
			codebox->setTextAlignment(EGUIA_UPPERLEFT,EGUIA_UPPERLEFT);
			codebox->setToolTipText(L"Ctrl+A, Ctrl+C to copy");
		}
		break;
	case GUI_ID_EX_NBS:
		{
			stringc* res = nodes[curId]->build(NBT_NBS);
			IGUIWindow* win = guienv->addWindow(rect<irr::s32>(50,50,50+320,50+320),false,L"Raw Code");
			IGUIEditBox* codebox = guienv->addEditBox(convert(res->c_str()),rect<irr::s32>(10,30,310,310),true,win);
			codebox->setMultiLine(true);
			codebox->setTextAlignment(EGUIA_UPPERLEFT,EGUIA_UPPERLEFT);
			codebox->setToolTipText(L"Ctrl+A, Ctrl+C to copy");
		}
		break;
	case GUI_ID_EX_NB:
		{
			stringc* res = nodes[curId]->build(NBT_NB);
			IGUIWindow* win = guienv->addWindow(rect<irr::s32>(50,50,50+320,50+320),false,L"Raw Code");
			IGUIEditBox* codebox = guienv->addEditBox(convert(res->c_str()),rect<irr::s32>(10,30,310,310),true,win);
			codebox->setMultiLine(true);
			codebox->setTextAlignment(EGUIA_UPPERLEFT,EGUIA_UPPERLEFT);
			codebox->setToolTipText(L"Ctrl+A, Ctrl+C to copy");
		}
		break;
	case GUI_ID_TEXT:
		if (nodes[curId] && nodes[curId]->getCurrentNodeBox() && nodes[curId]->getCurrentNodeBox()->model)
		{
			IGUIWindow* win = guienv->addWindow(rect<irr::s32>(50,50,50+320,50+162),false,L"Properties");
			guienv->addEditBox(convert(nodes[curId]->getCurrentNodeBox()->model->getName()),rect<irr::s32>(32,32,290,52),true,win,GUI_ID_BOX_NAME);

			// Add the co-ordinate boxes
			{
				guienv->addEditBox(convert(stringc(nodes[curId]->getCurrentNodeBox()->position.X - ((float)nodes[curId]->getCurrentNodeBox()->size.X / (float)2)).c_str()),rect<irr::s32>(32,60,112,80),true,win,230);
				guienv->addEditBox(convert(stringc(nodes[curId]->getCurrentNodeBox()->position.Y - ((float)nodes[curId]->getCurrentNodeBox()->size.Y / (float)2)).c_str()),rect<irr::s32>(32+90,60,112+90,80),true,win,230);
				guienv->addEditBox(convert(stringc(nodes[curId]->getCurrentNodeBox()->position.Z - ((float)nodes[curId]->getCurrentNodeBox()->size.Z / (float)2)).c_str()),rect<irr::s32>(32+180,60,112+180,80),true,win,230);

				guienv->addEditBox(convert(stringc(nodes[curId]->getCurrentNodeBox()->position.X + ((float)nodes[curId]->getCurrentNodeBox()->size.X / (float)2)).c_str()),rect<irr::s32>(32,60+30,112,80+30),true,win,230);
				guienv->addEditBox(convert(stringc(nodes[curId]->getCurrentNodeBox()->position.Y + ((float)nodes[curId]->getCurrentNodeBox()->size.Y / (float)2)).c_str()),rect<irr::s32>(32+90,60+30,112+90,80+30),true,win,230);
				guienv->addEditBox(convert(stringc(nodes[curId]->getCurrentNodeBox()->position.Z + ((float)nodes[curId]->getCurrentNodeBox()->size.Z / (float)2)).c_str()),rect<irr::s32>(32+180,60+30,112+180,80+30),true,win,230);
			}
			guienv->addButton(rect<irr::s32>(100,120,220,152),win,GUI_ID_SUBMIT,L"Update");
		}
		break;
	case GUI_ID_DELETENB:
		nodes[curId]->deleteNodebox(nodes[curId]->getCurrentNodeBox());
		break;
	case GUI_ID_BOX:
		nodes[curId]->addNodeBox();
		break;
	case GUI_ID_ABOUT:
		guienv->addMessageBox(L"About",L"This node box generator was made by Rubenwardy in C++ and Irrlicht");
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
	case GUI_ID_SNAP:
		data->snapping = menu->isItemChecked(menu->getSelectedItem());
		break;
	case GUI_ID_LIMIT:
		data->limiting = menu->isItemChecked(menu->getSelectedItem());
		break;
	default:
		if (id>=230){
			int i = id - 230;
			nodes[curId]->changeID(i);
		}
	}
}

void cEditor::updatePoint(int start, int count){
	for (int id=start;id<count;id++){
		if (!points[id])
			continue;

		points[id] -> image -> setVisible (nodes[curId] && nodes[curId]->getCurrentNodeBox()!=NULL);

		if (nodes[curId]==NULL || nodes[curId]->getCurrentNodeBox()==NULL)
			continue;

		if (point_on == id){
			// get mouse position
			position2di target = mouse_position;
			target.X -= 5;
			target.Y -= 5;

			// correct out of bounds
			if (target.X < driver->getViewPort().UpperLeftCorner.X){
				target.X = driver->getViewPort().UpperLeftCorner.X-5;
			}else if (target.X > driver->getViewPort().LowerRightCorner.X){
				target.X = driver->getViewPort().LowerRightCorner.X-5;
			}
				
			if (target.Y < driver->getViewPort().UpperLeftCorner.Y){
				target.Y = driver->getViewPort().UpperLeftCorner.Y-5;	
			}else if (target.Y > driver->getViewPort().LowerRightCorner.Y){
				target.Y = driver->getViewPort().LowerRightCorner.Y-5;
			}

			// set image location
			points[id] -> image -> setRelativePosition(target);

			position2di tar2 = target;
			tar2.X -= driver->getViewPort().UpperLeftCorner.X - 5;
			tar2.Y -= driver->getViewPort().UpperLeftCorner.Y - 5;

			// get the ray
			line3d<irr::f32> ray = coli -> getRayFromScreenCoordinates(tar2,smgr->getActiveCamera());
			
			// contains the output values
			vector3df wpos = vector3df(0,0,0); // the collision position
			#if IRRLICHT_VERSION_MAJOR == 1 && IRRLICHT_VERSION_MINOR < 8
				const ISceneNode* tmpNode; // not needed, but required for function
			#else
				ISceneNode* tmpNode; // not needed, but required for function
			#endif
			triangle3df tmpTri; // not needed, but required for function

			// Execute function
			coli->getCollisionPoint(ray,plane_tri,wpos,tmpTri,tmpNode);

			// Resize node box face
			nodes[curId]->resizeNodeBoxFace(nodes[curId]->getCurrentNodeBox(),points[id]->type,wpos);
		}else{
			vector3df position = vector3df(0,0,0);

			switch (points[id]->type){
			case CDR_X_P:
				position.X = nodes[curId]->getCurrentNodeBox()->position.X + ((float)nodes[curId]->getCurrentNodeBox()->size.X / (float)2);
				position.Y = nodes[curId]->getCurrentNodeBox()->position.Y;
				position.Z = nodes[curId]->getCurrentNodeBox()->position.Z;
				break;
			case CDR_X_N:
				position.X = nodes[curId]->getCurrentNodeBox()->position.X - ((float)nodes[curId]->getCurrentNodeBox()->size.X / (float)2);
				position.Y = nodes[curId]->getCurrentNodeBox()->position.Y;
				position.Z = nodes[curId]->getCurrentNodeBox()->position.Z;
				break;
			case CDR_Y_P:
				position.X = nodes[curId]->getCurrentNodeBox()->position.X;
				position.Y = nodes[curId]->getCurrentNodeBox()->position.Y + ((float)nodes[curId]->getCurrentNodeBox()->size.Y / (float)2);
				position.Z = nodes[curId]->getCurrentNodeBox()->position.Z;
				break;
			case CDR_Y_N:
				position.X = nodes[curId]->getCurrentNodeBox()->position.X;
				position.Y = nodes[curId]->getCurrentNodeBox()->position.Y - ((float)nodes[curId]->getCurrentNodeBox()->size.Y / (float)2);
				position.Z = nodes[curId]->getCurrentNodeBox()->position.Z;
				break;
			case CDR_Z_P:
				position.X = nodes[curId]->getCurrentNodeBox()->position.X;
				position.Y = nodes[curId]->getCurrentNodeBox()->position.Y;
				position.Z = nodes[curId]->getCurrentNodeBox()->position.Z + ((float)nodes[curId]->getCurrentNodeBox()->size.Z / (float)2);
				break;
			case CDR_Z_N:
				position.X = nodes[curId]->getCurrentNodeBox()->position.X;
				position.Y = nodes[curId]->getCurrentNodeBox()->position.Y;
				position.Z = nodes[curId]->getCurrentNodeBox()->position.Z - ((float)nodes[curId]->getCurrentNodeBox()->size.Z / (float)2);
				break;
			}
			#if IRRLICHT_VERSION_MAJOR == 1 && IRRLICHT_VERSION_MINOR < 8
				vector2d<irr::s32> cpos = coli -> getScreenCoordinatesFrom3DPosition(position,smgr->getActiveCamera());
			#else
				vector2d<irr::s32> cpos = coli -> getScreenCoordinatesFrom3DPosition(position,smgr->getActiveCamera(),true);
			#endif
			points[id] -> image -> setRelativePosition(position2di(driver->getViewPort().UpperLeftCorner.X+cpos.X-5, driver->getViewPort().UpperLeftCorner.Y+cpos.Y-5));
		}
	}
}

void cEditor::addPoint(int id, CDR_TYPE type){
	points[id] = new CDR();
	points[id] -> type = type;
	points[id] -> image = guienv->addImage(rect<irr::s32>(0,0,10,10),NULL,300+id);
	points[id] -> image -> setImage(driver->getTexture("gui_scale.png"));
	points[id] -> image -> setVisible (false);
}