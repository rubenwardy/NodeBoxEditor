#include "Editor.h"

Editor::Editor()
:isSplitScreen(true),currentWindow(0),point_on(-1),mouse_down(false),KEY_MOVE_DOWN(false),KEY_SCALE_DOWN(false),menubar(NULL)
{
	for (int i=0;i<4;i++){
		camera[i]=NULL;
	}

	for (int i=0;i<12;i++){
		points[i]=NULL;
	}

	menus = new list<IEventReceiver*>();
}

bool Editor::run(IrrlichtDevice* irr_device){
	// Irrlicht Device
	device=irr_device;
	driver = device->getVideoDriver();
	smgr = device->getSceneManager();
	guienv = device->getGUIEnvironment();
	coli=smgr->getSceneCollisionManager();
	device->setEventReceiver(this);

	// Editor Settings
	data=new ed_data();
	data->snapping = true;
	data->limiting = true;
	data->type = TOOL_NODEB;
	data->menu = NULL;

	// Load editor
	Load_Scene();
	Load_UI();

	// Create project
	project = new Project();
	project->nodes[0] = new Node(device,data);
	MoveTarget(project->nodes[0]->getPosition());

	// Set up window	
	device->setWindowCaption(convert((project->mod + stringc(" - The NodeBox Editor")).c_str()));
	device->setResizable(true);

	// Step data
	unsigned int counter=0;
	dimension2d<irr::u32> last_render_size = driver->getCurrentRenderTargetSize();

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
				
				updatePoint(0,5);
			}

			// Draw Camera 2
			if (camera[2]){
				smgr->setActiveCamera(camera[2]);
				driver->setViewPort(rect<s32>(0,ResY/2,ResX/2,ResY));
				smgr->drawAll();

				updatePoint(5,10);
			}

			// Draw Camera 3
			if (camera[3]){
				smgr->setActiveCamera(camera[3]);
				driver->setViewPort(rect<s32>(ResX/2,ResY/2,ResX,ResY));				
				smgr->drawAll();

				updatePoint(10,15);
			}

			// Draw GUI
			driver->setViewPort(rect<s32>(0,0,ResX,ResY));
			driver->draw2DLine(vector2d<irr::s32>(0,ResY/2),vector2d<irr::s32>(ResX,ResY/2),SColor(175,255,255,255));
			driver->draw2DLine(vector2d<irr::s32>(0,ResY/2-1),vector2d<irr::s32>(ResX,ResY/2-1),SColor(175,255,255,255));
			driver->draw2DLine(vector2d<irr::s32>(ResX/2,0),vector2d<irr::s32>(ResX/2,ResY),SColor(175,255,255,255));
			driver->draw2DLine(vector2d<irr::s32>(ResX/2+1,0),vector2d<irr::s32>(ResX/2+1,ResY),SColor(175,255,255,255));
		}else if (camera[currentWindow]){
			smgr->setActiveCamera(camera[currentWindow]);
			driver->setViewPort(rect<s32>(0,0,ResX,ResY));			
			smgr->drawAll();
			updatePoint(0,15);
		}

		guienv->drawAll();
		driver->endScene();

		
		if ( driver->getCurrentRenderTargetSize() != last_render_size){
			printf("Adjusting FOV to new screen size...\n");
			last_render_size = driver->getCurrentRenderTargetSize();
			camera[0]->setAspectRatio((float)driver->getScreenSize().Width/(float)driver->getScreenSize().Height);

			// reset matrix
			matrix4 projMat;
			irr::f32 orth_w = (float)driver->getScreenSize().Width / (float)driver->getScreenSize().Height;
			orth_w = 3 * orth_w;
			projMat.buildProjectionMatrixOrthoLH(orth_w,3,1,100);

			for (int i=1;i<4;i++){
				if (camera[i]){
					camera[i]->remove();
					camera[i]=NULL;
				}
			}

			// Remake cameras
			camera[1]=smgr->addCameraSceneNode(target,vector3df(0,2,-0.01),vector3df(0,0,0));
			camera[1]->setProjectionMatrix(projMat,true);
			camera[2]=smgr->addCameraSceneNode(target,vector3df(0,0,-5),vector3df(0,0,0));
			camera[2]->setProjectionMatrix(projMat,true);
			camera[3]=smgr->addCameraSceneNode(target,vector3df(-5,0,0),vector3df(0,0,0));
			camera[3]->setProjectionMatrix(projMat,true);	
		}

		if (counter>500){	
			counter=0;
			if (project->GetCurNode())
				project->GetCurNode()->update();	
		}

	}

	return true;
}

void Editor::Load_UI(){
	std::cout << "Loading the User Interface" << std::endl;
	guienv->clear();
	guienv->getSkin()->setFont(guienv->getFont("fontlucida.png"));

	// The Status Text
	int tmp_b=driver->getScreenSize().Height-58;
	data->d_nb=guienv->addStaticText(L"NodeBox: -",rect<s32>(5,tmp_b,200,tmp_b+15));
	data->d_pos=guienv->addStaticText(L"One: - , - , -",rect<s32>(5,tmp_b+15,300,tmp_b+30));
	data->d_rot=guienv->addStaticText(L"Two: - , - , -",rect<s32>(5,tmp_b+30,300,tmp_b+45));

	data->d_nb->setAlignment(EGUIA_UPPERLEFT, EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT);
	data->d_pos->setAlignment(EGUIA_UPPERLEFT, EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT);
	data->d_rot->setAlignment(EGUIA_UPPERLEFT, EGUIA_UPPERLEFT,EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT);


	// The Menu
	{
		menubar=guienv->addMenu();
		menubar->addItem(L"File",-1,true,true);
		menubar->addItem(L"Edit",-1,true,true);
		menubar->addItem(L"View",-1,true,true);
		menubar->addItem(L"Project",-1,true,true);
		menubar->addItem(L"Node",-1,true,true);
		menubar->addItem(L"Help",-1,true,true);
		gui::IGUIContextMenu* submenu;

		// File
		submenu = menubar->getSubMenu(0);
		submenu->addItem(L"New",GUI_ID_NEW);
		submenu->addItem(L"Load",GUI_ID_LOAD,false);
		submenu->addItem(L"Save",GUI_ID_SAVE,false);
		submenu->addSeparator();
		submenu->addItem(L"Import",GUI_ID_IMPORT,false);
		submenu->addItem(L"Export",-1,true,true);
		submenu->addSeparator();
		submenu->addItem(L"Exit",GUI_ID_EXIT);

		// Export
		submenu = submenu->getSubMenu(5);
		if (submenu){
			submenu->addItem(L"Project",GUI_ID_EX_PROJ);
			submenu->addSeparator();
			submenu->addItem(L"Entire Node",GUI_ID_EX_NODE);
			submenu->addItem(L"Nodebox table",GUI_ID_EX_NBS);
			submenu->addItem(L"Raw nodebox data",GUI_ID_EX_NB);
		}

		// Edit
		submenu = menubar->getSubMenu(1);
		submenu->addItem(L"Snap to grid",GUI_ID_SNAP,true,false,true,true);
		submenu->addItem(L"Limit to node size",GUI_ID_LIMIT,true,false,true,true);
		submenu->addSeparator();
		submenu->addItem(L"Node creator tool",GUI_ID_TOOL_NNODE);
		submenu->addItem(L"Node tool",GUI_ID_TOOL_NODE);
		submenu->addItem(L"Nodebox tool",GUI_ID_TOOL_NODEB);
		submenu->addItem(L"Texture tool",GUI_ID_TOOL_TEXT);

		UpdateEditToolMode();

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
		submenu->addItem(L"Node properties",-1,false);
		submenu->addSeparator();
		submenu->addItem(L"Add a nodebox",GUI_ID_BOX);
		submenu->addItem(L"Nodebox properties",GUI_ID_TEXT);
		submenu->addItem(L"Delete current nodebox",GUI_ID_DELETENB);

		data->menu = submenu;

		// Help
		submenu = menubar->getSubMenu(5);
		submenu->addItem(L"Help",GUI_ID_HELP,false);
		submenu->addSeparator();
		submenu->addItem(L"About",GUI_ID_ABOUT);
	}

	// CDRs
	{
		// Add Topdown camera's CDRs
		addPoint(0,CDR_Z_P,1);
		addPoint(1,CDR_X_N,1);
		addPoint(2,CDR_X_P,1);
		addPoint(3,CDR_Z_N,1);
		addPoint(4,CDR_XZ,1);
		// Add front camera's CDRs
		addPoint(5,CDR_Y_P,2);
		addPoint(6,CDR_X_N,2);
		addPoint(7,CDR_X_P,2);
		addPoint(8,CDR_Y_N,2);
		addPoint(9,CDR_XY,1);
		// Add side camera's CDRs
		addPoint(10,CDR_Y_P,3);
		addPoint(11,CDR_Z_P,3);
		addPoint(12,CDR_Z_N,3);
		addPoint(13,CDR_Y_N,3);
		addPoint(14,CDR_ZY,1);
	}
}

void Editor::Load_Scene(){
// Calculate Projection Matrix
	matrix4 projMat;
	irr::f32 orth_w = (float)driver->getScreenSize().Width / (float)driver->getScreenSize().Height;
	orth_w = 3 * orth_w;
	projMat.buildProjectionMatrixOrthoLH(orth_w,3,1,100);

	// Create target
	target = smgr->addEmptySceneNode(0,200);
	target->setPosition(vector3df(0,0,0));
	
	// Add rotational camera
	pivot=smgr->addEmptySceneNode(target,199);
	camera[0]=smgr->addCameraSceneNode(NULL,vector3df(0,0,-2),vector3df(0,0,0));
	camera[0]->setParent(pivot);
	pivot->setRotation(vector3df(25,-45,0));

	// Add Topdown camera
	camera[1]=smgr->addCameraSceneNode(target,vector3df(0,2,-0.01),vector3df(0,0,0));
	camera[1]->setProjectionMatrix(projMat,true);
	
	// Add front camera
	camera[2]=smgr->addCameraSceneNode(target,vector3df(0,0,-5),vector3df(0,0,0));
	camera[2]->setProjectionMatrix(projMat,true);

	// Add side camera
	camera[3]=smgr->addCameraSceneNode(target,vector3df(-5,0,0));
	camera[3]->setProjectionMatrix(projMat,true);	

	// Add Light
	ILightSceneNode* light=smgr->addLightSceneNode(target,vector3df(25,50,0));
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
}

bool Editor::OnEvent(const SEvent& event)
{
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
		printf("key down %i \n",event.KeyInput.Key);

		if (event.KeyInput.Key== KEY_DOWN || event.KeyInput.Key== KEY_KEY_S){
			pivot->setRotation(vector3df(pivot->getRotation().X-1,pivot->getRotation().Y,pivot->getRotation().Z));
		}else if (event.KeyInput.Key== KEY_UP || event.KeyInput.Key== KEY_KEY_W){
			pivot->setRotation(vector3df(pivot->getRotation().X+1,pivot->getRotation().Y,pivot->getRotation().Z));
		}else if (event.KeyInput.Key== KEY_LEFT || event.KeyInput.Key== KEY_KEY_A){
			pivot->setRotation(vector3df(pivot->getRotation().X,pivot->getRotation().Y+1,pivot->getRotation().Z));
		}else if (event.KeyInput.Key== KEY_RIGHT || event.KeyInput.Key== KEY_KEY_D){
			pivot->setRotation(vector3df(pivot->getRotation().X,pivot->getRotation().Y-1,pivot->getRotation().Z));
		}else if (event.KeyInput.Key == 162){
			printf("cntrl toggle\n");
			KEY_SCALE_DOWN  = event.KeyInput.PressedDown;
		}else if (event.KeyInput.Key == 160){
			printf("shift toggle\n");
			KEY_MOVE_DOWN  = event.KeyInput.PressedDown;
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
			case EGET_MESSAGEBOX_OK:
				if (event.GUIEvent.Caller->getID()==GUI_ID_NEW){
					printf("New project...\n");
					delete project;
					project = new Project();
					project->nodes[0] = new Node(device,data);
					MoveTarget(project->nodes[0]->getPosition());
				}else if (event.GUIEvent.Caller->getID()==GUI_ID_EXIT){
					printf("Exiting...\n");
					device->closeDevice();
				}
				break;
			}

			for(list<IEventReceiver*>::Iterator Iterator = menus->begin(); Iterator != menus->end(); ++Iterator)
			{
				IEventReceiver* m = *Iterator;
				if (m && m->OnEvent(event)==true)
					return true;
			} 
	}

	return false;
}

void Editor::OnMenuItemSelected( IGUIContextMenu* menu )
{
	s32 id = menu->getItemCommandId(menu->getSelectedItem());
	IGUIEnvironment* env = device->getGUIEnvironment();
	switch(id)
	{
	case GUI_ID_NEW:
		guienv->addMessageBox(L"Are you sure?",L"You will lose all unsaved changes.",true,(EMBF_OK|EMBF_CANCEL),0,GUI_ID_NEW);
		break;
	case GUI_ID_EXIT:
		guienv->addMessageBox(L"Are you sure?",L"You will lose all unsaved changes.",true,(EMBF_OK|EMBF_CANCEL),0,GUI_ID_EXIT);
		break;
	case GUI_ID_EX_NODE:
		{
			stringc* res = project->GetCurNode()->build(NBT_FULL);
			menus->push_back(new CodeDialog(guienv,convert(res->c_str())));
		}
		break;
	case GUI_ID_EX_NBS:
		{
			stringc* res = project->GetCurNode()->build(NBT_NBS);
			menus->push_back(new CodeDialog(guienv,convert(res->c_str())));
		}
		break;
	case GUI_ID_EX_NB:
		{
			stringc* res = project->GetCurNode()->build(NBT_NB);
			menus->push_back(new CodeDialog(guienv,convert(res->c_str())));		
		}
		break;
	case GUI_ID_TEXT:
		menus->push_back(new NBPropertiesDialog(guienv,project->GetCurNode()->GetCurrentNodeBox(),project->GetCurNode()));
		break;
	case GUI_ID_DELETENB:
		project->GetCurNode()->deleteNodebox(project->GetCurNode()->GetId());
		break;
	case GUI_ID_BOX:
		project->GetCurNode()->addNodeBox();
		break;
	case GUI_ID_ABOUT:
		{
			core::stringw* msg = new core::stringw(L"The Nodebox Editor\n");
			msg->append("Version: ");
			msg->append(EDITOR_TEXT_VERSION);
			msg->append("\n\n");
			msg->append("This free nodebox editor was made by Rubenwardy in C++ and Irrlicht.\n");
			msg->append("You can download newer versions from the Minetest forum.");		
			guienv->addMessageBox(L"About",msg->c_str());
		}
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
	case GUI_ID_TOOL_NNODE:
		data->type = TOOL_NEWNODE;
		UpdateEditToolMode();
		break;
	case GUI_ID_TOOL_NODE:
		data->type = TOOL_NODE;
		UpdateEditToolMode();
		break;
	case GUI_ID_TOOL_NODEB:
		data->type = TOOL_NODEB;
		UpdateEditToolMode();
		break;
	case GUI_ID_TOOL_TEXT:
		data->type = TOOL_TEXT;
		UpdateEditToolMode();
		break;
	default:
		if (id>=250){
			int i = id - 250;
			project->GetCurNode()->select(i);
		}
	}
}

void Editor::UpdateEditToolMode(){
	gui::IGUIContextMenu* submenu = menubar->getSubMenu(1);

	submenu->setItemChecked(3,(data->type==TOOL_NEWNODE));
	submenu->setItemChecked(4,(data->type==TOOL_NODE));
	submenu->setItemChecked(5,(data->type==TOOL_NODEB));
	submenu->setItemChecked(6,(data->type==TOOL_TEXT));	
}

void Editor::updatePoint(int start, int count){
	for (int id=start;id<count;id++){
		if (!points[id])
			continue;

		bool visible = (
					project->GetCurNode() &&
					data->type == TOOL_NODEB &&
					project->GetCurNode()->GetCurrentNodeBox()!=NULL &&
					(isSplitScreen==true || currentWindow==points[id]->camera) &&
					((KEY_MOVE_DOWN==false && points[id]->type < CDR_XZ) || (KEY_MOVE_DOWN==true && points[id]->type > CDR_Z_N))
				);

		points[id] -> image -> setVisible (visible);

		if (!visible)
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

			// Execute ray
			coli->getCollisionPoint(ray,plane_tri,wpos,tmpTri,tmpNode);

			// Call required function
			if (points[id]->type < CDR_XZ){
				project->GetCurNode()->resizeNodeBoxFace(points[id]->type,wpos,KEY_SCALE_DOWN);
			}else{
				project->GetCurNode()->moveNodeBox(points[id]->type,wpos);
			}
		}else{
			vector3df position;

			switch (points[id]->type){
			case CDR_X_P:
				position = project->GetCurNode()->GetCurrentNodeBox()->GetCenter();
				position.X = project->GetCurNode()->GetCurrentNodeBox()->two.X;
				break;
			case CDR_X_N:
				position = project->GetCurNode()->GetCurrentNodeBox()->GetCenter();
				position.X = project->GetCurNode()->GetCurrentNodeBox()->one.X;
				break;
			case CDR_Y_P:
				position = project->GetCurNode()->GetCurrentNodeBox()->GetCenter();
				position.Y = project->GetCurNode()->GetCurrentNodeBox()->two.Y;
				break;
			case CDR_Y_N:
				position = project->GetCurNode()->GetCurrentNodeBox()->GetCenter();
				position.Y = project->GetCurNode()->GetCurrentNodeBox()->one.Y;
				break;
			case CDR_Z_P:
				position = project->GetCurNode()->GetCurrentNodeBox()->GetCenter();
				position.Z = project->GetCurNode()->GetCurrentNodeBox()->two.Z;
				break;
			case CDR_Z_N:
				position = project->GetCurNode()->GetCurrentNodeBox()->GetCenter();
				position.Z = project->GetCurNode()->GetCurrentNodeBox()->one.Z;
				break;
			case CDR_XZ:
				position = project->GetCurNode()->GetCurrentNodeBox()->GetCenter();
				break;
			case CDR_XY:
				position = project->GetCurNode()->GetCurrentNodeBox()->GetCenter();
				break;
			case CDR_ZY:
				position = project->GetCurNode()->GetCurrentNodeBox()->GetCenter();
				break;
			}

			position.X+=project->GetCurNode()->getPosition().X;
			position.Y+=project->GetCurNode()->getPosition().Y;
			position.Z+=project->GetCurNode()->getPosition().Z;

			#if IRRLICHT_VERSION_MAJOR == 1 && IRRLICHT_VERSION_MINOR < 8
				vector2d<irr::s32> cpos = coli -> getScreenCoordinatesFrom3DPosition(position,smgr->getActiveCamera());
			#else
				vector2d<irr::s32> cpos = coli -> getScreenCoordinatesFrom3DPosition(position,smgr->getActiveCamera(),true);
			#endif
			points[id] -> image -> setRelativePosition(position2di(driver->getViewPort().UpperLeftCorner.X+cpos.X-5, driver->getViewPort().UpperLeftCorner.Y+cpos.Y-5));
		}
	}
}
 
void Editor::addPoint(int id, CDR_TYPE type, int camera){
	points[id] = new CDR();
	points[id] -> type = type;
	points[id] -> camera = camera;
	points[id] -> image = guienv->addImage(rect<irr::s32>(0,0,10,10),NULL,300+id);
	points[id] -> image -> setImage(driver->getTexture("gui_scale.png"));
	points[id] -> image -> setVisible (false);
}

void Editor::MoveTarget(vector3df pos){
	 target->setPosition(pos);

	 for (int i=0;i<4;i++){
		camera[i]->setTarget(pos);
	 }
 }