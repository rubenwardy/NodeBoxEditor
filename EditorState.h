#ifndef _EDITORSTATE_H_INCLUDE_
#define _EDITORSTATE_H_INCLUDE_
#include "common.h"
#include "Configuration.h"
#include "Project.h"
#include "MenuState.h"

#define NUMBER_OF_KEYS 252
enum KeyState{
	EKS_UP = false,
	EKS_DOWN = true,
};

class Project;
class EditorMode;
class MenuState;
class EditorState
{
public:
	EditorState(irr::IrrlichtDevice* dev,Project* proj,Configuration* settings);

	// Irrlicht
	ITriangleSelector* plane_tri;
	irr::IrrlichtDevice* GetDevice() const {return _device;}	

	// Project
	Project* project;

	// Editor
	EditorMode* Mode() const;
	void AddMode(EditorMode* value);	
	MenuState* Menu() const{return _menu;}
	void SetMenu(MenuState* stat){_menu=stat;}	
	void CloseEditor(){close_requested = true;}
	bool NeedsClose() const{return close_requested;}

	// Input
	bool mousedown;
	irr::core::vector2di mouse_position;
	KeyState keys[NUMBER_OF_KEYS];
	
	
	// Settings
	Configuration* Settings() const{return _settings;}
private:
	irr::IrrlichtDevice* _device;
	int currentmode;
	list<EditorMode*>* _modes;
	Configuration* _settings;
	MenuState* _menu;
	bool close_requested;
};

class EditorMode:public irr::IEventReceiver
{
public:
	EditorMode(EditorState* st):_state(st){}
	virtual void load() = 0;
	virtual void unload() = 0;
	virtual void update(double dtime) = 0;
	virtual void draw(irr::video::IVideoDriver* driver) = 0;
	virtual void viewportTick(VIEWPORT window,irr::video::IVideoDriver* driver,rect<s32> offset) = 0;
	virtual bool OnEvent(const irr::SEvent &event) = 0;
	EditorState* GetState() const {return _state;}
private:
	EditorState* _state;
};
#endif