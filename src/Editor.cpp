#include "Editor.h"
#include <ctime>
#include <time.h>

Editor::Editor()
:_state(NULL),_device(NULL),target(NULL),pivot(NULL),currentWindow(-1)
{
	for (int i=0;i<4;i++){
		camera[i] = NULL;
	}
}

bool Editor::run(IrrlichtDevice* irr_device,Configuration* conf){
	// Do Irrlicht Stuff
	_device = irr_device;
	IVideoDriver* driver = GetDevice()->getVideoDriver();
	ISceneManager* smgr = GetDevice()->getSceneManager();
	IGUIEnvironment* guienv = GetDevice()->getGUIEnvironment();	
	GetDevice()->setEventReceiver(this);

	if (!conf->getSettingAsBool("fullscreen"))
		GetDevice()->setResizable(true);

	// Project and state
	Project* proj = new Project();
	_state = new EditorState(GetDevice(),proj,conf);

	// Menu State
	MenuState* ms = new MenuState(GetState());
	GetState()->SetMenu(ms);

	// Add editor modes
	GetState()->AddMode(new NBEditor(GetState()));

	// Set up project
	proj->AddNode(new Node(GetDevice(),GetState()));
	proj->SelectNode(0);

	// Load user interface
	LoadScene();	
	GetState()->Menu()->init();
	GetState()->Mode()->load();
	printf("Complete!\n");

	int LastX = driver->getScreenSize().Width;
	if (!GetState()->Settings()->getSettingAsBool("hide_sidebar"))
			LastX -= 256;
	int LastY = driver->getScreenSize().Height;
	int lastFPS = -1;

	bool dosleep = GetState()->Settings()->getSettingAsBool("use_sleep");
	u32 last = std::clock();
	double dtime = 0;
	while (GetDevice()->run()){
		if (GetState()->NeedsClose()){
			GetDevice()->closeDevice();
			return true;
		}

		driver->beginScene(true, true, irr::video::SColor(255,150,150,150));

		int ResX = driver->getScreenSize().Width;
		if (!GetState()->Settings()->getSettingAsBool("hide_sidebar"))
			ResX -= 256;
		
		int ResY = driver->getScreenSize().Height;

		if (currentWindow == -1)
		{
			// Draw Camera 0
			if (camera[0]){
				smgr->setActiveCamera(camera[0]);
				rect<s32> offset = rect<s32>(0,0,ResX/2,ResY/2);
				driver->setViewPort(offset);
				smgr->drawAll();

				if (GetState()->Mode())
					GetState()->Mode()->viewportTick(EVIEW_PERS, driver, offset);
			}

			// Draw Camera 1
			if (camera[1]){
				smgr->setActiveCamera(camera[1]);
				rect<s32> offset = rect<s32>(ResX/2,0,ResX,ResY/2);
				driver->setViewPort(offset);
				smgr->drawAll();

				if (GetState()->Mode())
					GetState()->Mode()->viewportTick(EVIEW_XZ, driver, offset);
			}

			// Draw Camera 2
			if (camera[2]){
				smgr->setActiveCamera(camera[2]);
				rect<s32> offset = rect<s32>(0,ResY/2,ResX/2,ResY);
				driver->setViewPort(offset);
				smgr->drawAll();

				if (GetState()->Mode())
					GetState()->Mode()->viewportTick(EVIEW_XY, driver, offset);

			}

			// Draw Camera 3
			if (camera[3]){
				smgr->setActiveCamera(camera[3]);
				rect<s32> offset = rect<s32>(ResX/2,ResY/2,ResX,ResY);
				driver->setViewPort(offset);
				smgr->drawAll();

				if (GetState()->Mode())
					GetState()->Mode()->viewportTick(EVIEW_ZY, driver, offset);
			}

			// Draw GUI
			driver->setViewPort(rect<s32>(0,0,driver->getScreenSize().Width,driver->getScreenSize().Height));
			driver->draw2DLine(vector2d<irr::s32>(0,ResY/2),vector2d<irr::s32>(ResX,ResY/2),SColor(175,255,255,255));
			driver->draw2DLine(vector2d<irr::s32>(0,ResY/2-1),vector2d<irr::s32>(ResX,ResY/2-1),SColor(175,255,255,255));
			driver->draw2DLine(vector2d<irr::s32>(ResX/2,0),vector2d<irr::s32>(ResX/2,ResY),SColor(175,255,255,255));
			driver->draw2DLine(vector2d<irr::s32>(ResX/2+1,0),vector2d<irr::s32>(ResX/2+1,ResY),SColor(175,255,255,255));
		}else if (camera[currentWindow]){
			smgr->setActiveCamera(camera[currentWindow]);
			driver->setViewPort(rect<s32>(0,0,ResX,ResY));
			smgr->drawAll();

			if (GetState()->Mode())
					GetState()->Mode()->viewportTick((VIEWPORT)currentWindow, driver, rect<s32>(0,0,ResX,ResY));

			driver->setViewPort(rect<s32>(0,0,driver->getScreenSize().Width,driver->getScreenSize().Height));
		}

		if (GetState()->Menu())
			GetState()->Menu()->draw(driver);
		if (GetState()->Mode())
			GetState()->Mode()->draw(driver);

		guienv->drawAll();
		driver->endScene();

		int fps = driver->getFPS();
        if (lastFPS != fps)
        {
            irr::core::stringw str = L"Nodebox Editor [FPS: ";
            str += fps;
			str += "]";

            GetDevice()->setWindowCaption(str.c_str());
            lastFPS = fps;
        }

		if (LastX != ResX || LastY != ResY){
			printf("Adjusting FOV to new screen size...\n");
			LastX = ResX;
			LastY = ResY;
			camera[0]->setAspectRatio((float)ResX/(float)ResY);

			// reset matrix
			matrix4 projMat;
			irr::f32 orth_w = (float)ResX / (float)ResY;
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

		// Update
		if (GetState()->Mode())
			GetState()->Mode()->update(dtime);

		// Do sleep
		unsigned int now = std::clock();
		if (dosleep){
			u32 sleeptime = int(double(1000)/double(65)) - (now - last);
			if (sleeptime > 0 && sleeptime < 200)
				GetDevice()->sleep(sleeptime);
		}
		dtime = double(now - last)/1000;
		last = now;		
	}

	return true;
}

bool Editor::OnEvent(const SEvent& event)
{
	if (event.EventType == irr::EET_MOUSE_INPUT_EVENT &&
		event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP ){
		GetState()->mousedown=false;
	}else if (event.EventType == irr::EET_MOUSE_INPUT_EVENT &&
		event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN ){
		GetState()->mousedown=true;
	}else if (event.EventType == irr::EET_MOUSE_INPUT_EVENT &&
		event.MouseInput.Event == EMIE_MOUSE_MOVED ){
		GetState()->mouse_position.X=event.MouseInput.X;
		GetState()->mouse_position.Y=event.MouseInput.Y;
	}

	if (GetState()->Mode()){
		if (GetState()->Mode()->OnEvent(event)){
			return true;
		}
	}

	if (GetState()->Menu()){
		if (GetState()->Menu()->OnEvent(event)){
			return true;
		}
	}

	if (event.EventType == EET_KEY_INPUT_EVENT && event.KeyInput.Key < NUMBER_OF_KEYS){
		if (event.KeyInput.PressedDown){
			GetState()->keys[event.KeyInput.Key] = EKS_DOWN;
		}else{
			GetState()->keys[event.KeyInput.Key] = EKS_UP;
		}
	}
	if (event.EventType == EET_GUI_EVENT){
		if (event.GUIEvent.EventType == EGET_MENU_ITEM_SELECTED){
			IGUIContextMenu* menu = (IGUIContextMenu*)event.GUIEvent.Caller;
			switch (menu->getItemCommandId(menu->getSelectedItem())){
			case GUI_VIEW_SP_ALL:
				currentWindow = -1;
				break;
			case GUI_VIEW_SP_PER:
				currentWindow = 0;
				break;
			case GUI_VIEW_SP_TOP:
				currentWindow = 1;
				break;
			case GUI_VIEW_SP_FRT:
				currentWindow = 2;
				break;
			case GUI_VIEW_SP_RHT:
				currentWindow = 3;
				break;
			}
		}
	}
	return false;
}

void Editor::LoadScene(){
	IVideoDriver* driver = GetDevice()->getVideoDriver();
	ISceneManager* smgr = GetDevice()->getSceneManager();

	// Calculate Projection Matrix
	matrix4 projMat;
	irr::f32 orth_w = (float)(driver->getScreenSize().Width-256) / (float)driver->getScreenSize().Height;
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
	camera[3]=smgr->addCameraSceneNode(target,vector3df(5,0,0),vector3df(0,0,0));
	camera[3]->setProjectionMatrix(projMat,true);

	// Add Light
	ILightSceneNode* light=smgr->addLightSceneNode(target,vector3df(25,50,0));
	light->setLightType(ELT_POINT);
	light->setRadius(2000);

	// Add Plane
	IMeshSceneNode* plane = smgr->addCubeSceneNode(1,0,-1,vector3df(0.5,-5.5,0.5),vector3df(0,0,0),vector3df(10,10,10));
	plane->setMaterialTexture(0, driver->getTexture("media/texture_terrain.png"));
	plane->setMaterialFlag(video::EMF_BILINEAR_FILTER, false);
	plane->getMaterial(0).getTextureMatrix(0).setTextureScale(10,10);

	// Add sky box
	scene::IMeshSceneNode* skybox=smgr->addCubeSceneNode(50);
	skybox->setMaterialTexture(0, driver->getTexture("media/sky.jpg"));
	skybox->setMaterialFlag(video::EMF_BILINEAR_FILTER, false);
	skybox->setMaterialFlag(video::EMF_LIGHTING,false);
	smgr->getMeshManipulator()->flipSurfaces(skybox->getMesh());
	GetState()->plane_tri=smgr->createOctreeTriangleSelector(skybox->getMesh(),skybox);
}