#include "Editor.hpp"
#include "NBEditor.hpp"
#include "Node.hpp"
#include <ctime>
#include <time.h>

Editor::Editor() :
	state(NULL),
	device(NULL),
	target(NULL),
	pivot(NULL),
	currentWindow(-1)
{
	for (int i = 0; i < 4; i++) {
		camera[i] = NULL;
	}
}

void drawCoord(IGUIFont* font, IVideoDriver *driver, unsigned int x, unsigned int y, const wchar_t* xlabel, const wchar_t* ylabel)
{
	static ITexture *axes = driver->getTexture("media/coordinates.png");
	driver->draw2DImage(
			axes,
			position2d<s32>(x, y),
			rect<s32>(0, 0, 32, 32),
			NULL, SColor(255, 255, 255, 255), true
		);
	font->draw(
			ylabel,
			core::rect<s32>(x - 2, y - 20, 300, 50),
			video::SColor(255, 255, 255, 255)
		);
	font->draw(
			xlabel,
			core::rect<s32>(x + 40, y + 22, 300, 50),
			video::SColor(255, 255, 255, 255)
		);
}

bool Editor::run(IrrlichtDevice* irr_device,Configuration* conf)
{
	// Do Irrlicht Stuff
	device = irr_device;
	IVideoDriver *driver = device->getVideoDriver();
	ISceneManager *smgr = device->getSceneManager();
	IGUIEnvironment *guienv = device->getGUIEnvironment();	
	device->setEventReceiver(this);
	device->setWindowCaption(L"Node Box Editor");

	if (!conf->getBool("fullscreen")) {
		device->setResizable(true);
	}

	// Project and state
	Project *proj = new Project();
	state = new EditorState(device, proj, conf);

	// Menu State
	state->menu = new MenuState(state);

	// Add editor modes
	state->AddMode(new NBEditor(state));
	state->AddMode(new NodeEditor(state));

	// Set up project
	proj->AddNode(state, true, false);

	// Load user interface
	LoadScene();	
	state->SelectMode(0);

	int LastX = driver->getScreenSize().Width;
	if (!state->settings->getBool("hide_sidebar")) {
			LastX -= 256;
	}
	int LastY = driver->getScreenSize().Height;
#ifdef _DEBUG
	int lastFPS = -1;
#endif

	bool dosleep = state->settings->getBool("use_sleep");
	u32 last = std::clock();
	double dtime = 0;
	while (device->run()) {
		if (state->NeedsClose()) {
			device->closeDevice();
			return true;
		}

		driver->beginScene(true, true, irr::video::SColor(255, 150, 150, 150));

		int ResX = driver->getScreenSize().Width;
		if (!state->settings->getBool("hide_sidebar"))
			ResX -= 256;
		
		int ResY = driver->getScreenSize().Height;

		if (currentWindow == -1) {
			viewportTick(VIEW_TL, rect<s32>(0,	0,	ResX/2,	ResY/2	));
			viewportTick(VIEW_TR, rect<s32>(ResX/2,	0,	ResX,	ResY/2	));
			viewportTick(VIEW_BL, rect<s32>(0,	ResY/2,	ResX/2,	ResY	));
			viewportTick(VIEW_BR, rect<s32>(ResX/2,	ResY/2,	ResX,	ResY	));

			// Draw separating lines
			driver->setViewPort(rect<s32>(0, 0, driver->getScreenSize().Width, driver->getScreenSize().Height));
			driver->draw2DLine(vector2d<irr::s32>(0, ResY/2), vector2d<irr::s32>(ResX, ResY/2), SColor(175,255,255,255));
			driver->draw2DLine(vector2d<irr::s32>(0, ResY/2-1), vector2d<irr::s32>(ResX, ResY/2-1), SColor(175,255,255,255));
			driver->draw2DLine(vector2d<irr::s32>(ResX/2, 0), vector2d<irr::s32>(ResX/2, ResY), SColor(175,255,255,255));
			driver->draw2DLine(vector2d<irr::s32>(ResX/2+1, 0), vector2d<irr::s32>(ResX/2+1, ResY), SColor(175,255,255,255));
		} else if (camera[currentWindow]) {
			smgr->setActiveCamera(camera[currentWindow]);
			driver->setViewPort(rect<s32>(0, 0, ResX, ResY));
			smgr->drawAll();

			if (state->Mode()) {
				state->Mode()->viewportTick((Viewport) currentWindow,
						driver, rect<s32>(0, 0, ResX, ResY));
			}

			driver->setViewPort(rect<s32>(0, 0, driver->getScreenSize().Width, driver->getScreenSize().Height));
		}

		if (state->menu) {
			state->menu->draw(driver);
		}
		if (state->Mode()) {
			state->Mode()->draw(driver);
		}

		if (state->project && state->project->GetCurrentNode()) {
			vector3df pos = vector3df(
				state->project->GetCurrentNode()->position.X,
				state->project->GetCurrentNode()->position.Y,
				state->project->GetCurrentNode()->position.Z
			);
			target->setPosition(pos);

			camera[0]->setTarget(pos);
			camera[1]->setTarget(pos);
			camera[2]->setTarget(pos);
			camera[3]->setTarget(pos);
		}

		guienv->drawAll();

		driver->endScene();

		#ifdef _DEBUG
		int fps = driver->getFPS();
		if (lastFPS != fps) {
			irr::core::stringw str = L"Node Box Editor [FPS: ";
			str += fps;
			str += "]";

			device->setWindowCaption(str.c_str());
			lastFPS = fps;
		}
		#endif

		if (LastX != ResX || LastY != ResY) {
			LastX = ResX;
			LastY = ResY;
			recreateCameras();
		}

		// Update
		if (state->Mode()) {
			state->Mode()->update(dtime);
		}

		// Do sleep
		unsigned int now = std::clock();
		if (dosleep) {
			u32 sleeptime = int(double(1000) / double(65)) - (now - last);
			if (sleeptime > 0 && sleeptime < 200)
				device->sleep(sleeptime);
		}
		dtime = double(now - last) / 1000;
		last = now;		
	}

	return true;
}

bool Editor::OnEvent(const SEvent& event)
{
	if (event.EventType == irr::EET_MOUSE_INPUT_EVENT &&
			event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP) {
		state->mousedown = false;
	} else if (event.EventType == irr::EET_MOUSE_INPUT_EVENT &&
			event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN) {
		state->mousedown = true;
	} else if (event.EventType == irr::EET_MOUSE_INPUT_EVENT &&
			event.MouseInput.Event == EMIE_MOUSE_MOVED) {
		state->mouse_position.X = event.MouseInput.X;
		state->mouse_position.Y = event.MouseInput.Y;
	}

	if (state->Mode()){
		if (state->Mode()->OnEvent(event)){
			return true;
		}
	}

	if (state->menu){
		if (state->menu->OnEvent(event)){
			return true;
		}
	}

	if (event.EventType == EET_KEY_INPUT_EVENT && event.KeyInput.Key < NUMBER_OF_KEYS){
		if (event.KeyInput.PressedDown) {
			state->keys[event.KeyInput.Key] = EKS_DOWN;
		} else {
			state->keys[event.KeyInput.Key] = EKS_UP;
		}

		if (
			state->device->getGUIEnvironment()->getFocus() &&
			state->device->getGUIEnvironment()->getFocus()->getType() == EGUIET_EDIT_BOX
			)
			return false;

		switch (event.KeyInput.Key) {
		case KEY_KEY_S:
			pivot->setRotation(vector3df(pivot->getRotation().X - 1,
					pivot->getRotation().Y, pivot->getRotation().Z));
			break;
		case KEY_KEY_W:
			pivot->setRotation(vector3df(pivot->getRotation().X + 1,
					pivot->getRotation().Y, pivot->getRotation().Z));
			break;
		case KEY_KEY_A:
			pivot->setRotation(vector3df(pivot->getRotation().X,
					pivot->getRotation().Y + 1, pivot->getRotation().Z));
			break;
		case KEY_KEY_D:
			pivot->setRotation(vector3df(pivot->getRotation().X,
					pivot->getRotation().Y - 1, pivot->getRotation().Z));
			break;
		case KEY_KEY_B:
			if (!event.KeyInput.PressedDown)
				state->SelectMode(0);
			break;
		case KEY_KEY_N:
			if (!event.KeyInput.PressedDown)
				state->SelectMode(1);
			break;
		}
	}
	if (event.EventType == EET_GUI_EVENT) {
		if (event.GUIEvent.EventType == EGET_MENU_ITEM_SELECTED) {
			IGUIContextMenu* menu = (IGUIContextMenu*)event.GUIEvent.Caller;
			switch (menu->getItemCommandId(menu->getSelectedItem())) {
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

void Editor::LoadScene()
{
	IVideoDriver *driver = device->getVideoDriver();
	ISceneManager *smgr = device->getSceneManager();

	// Create target
	target = smgr->addEmptySceneNode(0, 200);
	target->setPosition(vector3df(0, 0, 0));

	// Create cameras
	pivot = smgr->addEmptySceneNode(target, 199);	
	pivot->setRotation(vector3df(25, -45, 0));
	recreateCameras();

	// Add Light
	ILightSceneNode* light = smgr->addLightSceneNode(target, vector3df(25, 50, 0));
	light->setLightType(ELT_POINT);
	light->setRadius(2000);

	// Add Plane
	plane = smgr->addCubeSceneNode(1, 0, -1,
			vector3df(0.5, -5.5, 0.5), vector3df(0, 0, 0),
			vector3df(10, 10, 10));
	plane->setMaterialTexture(0, driver->getTexture("media/texture_terrain.png"));
	plane->setMaterialFlag(video::EMF_BILINEAR_FILTER, false);
	plane->setMaterialFlag(video::EMF_LIGHTING, false);
	plane->setMaterialFlag(video::EMF_BACK_FACE_CULLING, true);
	plane->getMaterial(0).getTextureMatrix(0).setTextureScale(10, 10);

	// Add sky box
	scene::IMeshSceneNode* skybox = smgr->addCubeSceneNode(50);
	skybox->setMaterialTexture(0, driver->getTexture("media/sky.jpg"));
	skybox->setMaterialFlag(video::EMF_BILINEAR_FILTER, false);
	skybox->setMaterialFlag(video::EMF_LIGHTING, false);
	smgr->getMeshManipulator()->flipSurfaces(skybox->getMesh());
	state->plane_tri = smgr->createOctreeTriangleSelector(skybox->getMesh(), skybox);
}

void Editor::recreateCameras()
{
	// Irrlicht
	IVideoDriver *driver = device->getVideoDriver();
	ISceneManager *smgr = device->getSceneManager();

	// Get screen sizes
	int ResX = driver->getScreenSize().Width;
	if (!state->settings->getBool("hide_sidebar"))
		ResX -= 256;	
	int ResY = driver->getScreenSize().Height;

	// reset matrix
	matrix4 projMat;
	irr::f32 orth_w = (float)ResX / (float)ResY;
	orth_w = 3 * orth_w;
	projMat.buildProjectionMatrixOrthoLH(orth_w,3,1,100);

	// Loop through cameras
	for (int i = 0; i < 4; i++) {
		// Delete old camera
		if (camera[i]) {
			camera[i]->remove();
			camera[i] = NULL;
		}

		ViewportType type = state->getViewportType((Viewport)i);
		std::cerr << "Creating camera " << i << " of type " << (int)type << std::endl;
		if (type == VIEWT_PERS) {
			vector3df oldrot = pivot->getRotation();
			pivot->setRotation(vector3df(0, 0, 0));
			camera[i] = smgr->addCameraSceneNode(NULL, vector3df(0, 0, -2), vector3df(0, 0, 0));
			camera[i]->setParent(pivot);
			camera[i]->setAspectRatio((float)ResX / (float)ResY);
			pivot->setRotation(oldrot);
		} else {			
			camera[i] = smgr->addCameraSceneNode(target);
			switch(type) {
			case VIEWT_TOP:
				camera[i]->setPosition(vector3df(0, 2, -0.01));
				break;
			case VIEWT_BOTTOM:
				camera[i]->setPosition(vector3df(0, -2, -0.01));
				break;
			case VIEWT_LEFT:
				camera[i]->setPosition(vector3df(-5, 0, 0));	
				break;		
			case VIEWT_RIGHT:
				camera[i]->setPosition(vector3df(5, 0, 0));
				break;
			case VIEWT_FRONT:
				camera[i]->setPosition(vector3df(0, 0, -5));
				break;
			case VIEWT_BACK:
				camera[i]->setPosition(vector3df(0, 0, 5));
				break;
			}
			camera[i]->setProjectionMatrix(projMat, true);
		}
	}
}

typedef rect<s32> rects32;
void Editor::viewportTick(Viewport viewport, rect<s32> rect)
{
	// Init
	IVideoDriver *driver = device->getVideoDriver();
	ISceneManager *smgr = device->getSceneManager();
	IGUIEnvironment *guienv = device->getGUIEnvironment();
	ViewportType type = state->getViewportType(viewport);

	// Draw camera
	smgr->setActiveCamera(camera[(int)viewport]);
	driver->setViewPort(rect);
	if (type == VIEWT_BOTTOM)
		plane->setVisible(false);
	smgr->drawAll();
	if (type == VIEWT_BOTTOM)
		plane->setVisible(true);

	// Callbacks
	if (state->Mode())
		state->Mode()->viewportTick(viewport, driver, rect);

	// Draw text	
	driver->setViewPort(rects32(0, 0, driver->getScreenSize().Width, driver->getScreenSize().Height));
	{
		const wchar_t* label = L"";
		switch(type) {
		case VIEWT_PERS:
			label = L"Perspective";
			break;
		case VIEWT_TOP:
			label = L"Top";
			break;
		case VIEWT_BOTTOM:
			label = L"Bottom";
			break;
		case VIEWT_LEFT:
			label = L"Left";
			break;		
		case VIEWT_RIGHT:
			label = L"Right";
			break;
		case VIEWT_FRONT:
			label = L"Front";
			break;
		case VIEWT_BACK:
			label = L"Back";
			break;
		}
		guienv->getSkin()->getFont()->draw(
				label,
				core::rect<s32>(rect.UpperLeftCorner.X + 10,
					rect.UpperLeftCorner.Y + ((viewport==VIEW_TL || viewport==VIEW_TR)?25:5),
					200, 50),
				video::SColor(255, 255, 255, 255)
			);
	}

	// Draw coordinate arrows
	if (type != VIEWT_PERS) {
		switch(type) {
		case VIEWT_TOP:
			drawCoord(guienv->getSkin()->getFont(), driver, rect.UpperLeftCorner.X + 10,
					rect.LowerRightCorner.Y - 42, L"X", L"Z");
			break;
		case VIEWT_BOTTOM:
			drawCoord(guienv->getSkin()->getFont(), driver, rect.UpperLeftCorner.X + 10,
					rect.LowerRightCorner.Y - 42, L"X", L"-Z");
			break;
		case VIEWT_LEFT:
			drawCoord(guienv->getSkin()->getFont(), driver, rect.UpperLeftCorner.X + 10,
					rect.LowerRightCorner.Y - 42, L"-Z", L"Y");
			break;		
		case VIEWT_RIGHT:
			drawCoord(guienv->getSkin()->getFont(), driver, rect.UpperLeftCorner.X + 10,
					rect.LowerRightCorner.Y - 42, L"Z", L"Y");
			break;
		case VIEWT_FRONT:
			drawCoord(guienv->getSkin()->getFont(), driver, rect.UpperLeftCorner.X + 10,
					rect.LowerRightCorner.Y - 42, L"X", L"Y");
			break;
		case VIEWT_BACK:
			drawCoord(guienv->getSkin()->getFont(), driver, rect.UpperLeftCorner.X + 10,
					rect.LowerRightCorner.Y - 42, L"-X", L"Y");
			break;
		}		
	}
}

