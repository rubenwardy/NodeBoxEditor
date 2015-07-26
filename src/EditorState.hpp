#ifndef EDITORSTATE_HPP_INCLUDED
#define EDITORSTATE_HPP_INCLUDED
#include "common.hpp"
#include "Configuration.hpp"
#include "project/project.hpp"
#include "MenuState.hpp"

#define NUMBER_OF_KEYS 252
enum EKeyState
{
	EKS_UP = false,
	EKS_DOWN = true
};

class Project;
class EditorMode;
class MenuState;
class EditorState
{
public:
	EditorState(irr::IrrlichtDevice* dev, Project* proj, Configuration* settings);

	// Irrlicht
	ITriangleSelector* plane_tri;
	IrrlichtDevice* device;

	// Project
	Project* project;

	// Editor
	EditorMode* Mode(int id) const
	{
		if (id < 0 || id >= 5) {
			return NULL;
		}
		return modes[id];
	}
	EditorMode* Mode() const
	{
		if (!Mode(currentmode))
			std::cerr << "Warning! Null mode returned..." << std::endl;
		return Mode(currentmode);
	}

	void SelectMode(int id);

	void AddMode(EditorMode *value);
	void CloseEditor() { close_requested = true; }
	bool NeedsClose() const { return close_requested; }

	// Input
	bool mousedown;
	vector2di mouse_position;
	EKeyState keys[NUMBER_OF_KEYS];

	Configuration *settings;
	MenuState *menu;

	EViewportType getEViewportType(EViewport id);

	bool isInstalled;
private:
	int currentmode;
	EditorMode *modes[5];
	int modeCount;
	bool close_requested;
};

class EditorMode : public irr::IEventReceiver
{
public:
	EditorMode(EditorState* st) : state(st) {}
	virtual void load() = 0;
	virtual void unload() = 0;
	virtual void update(double dtime) = 0;
	virtual void draw(irr::video::IVideoDriver* driver) {}
	virtual void drawViewport(irr::video::IVideoDriver* driver, EViewport viewport, rect<s32> area) {}
	virtual void viewportTick(EViewport window, irr::video::IVideoDriver* driver, rect<s32> offset) = 0;
	virtual bool OnEvent(const irr::SEvent &event) = 0;
	virtual irr::video::ITexture* icon() = 0;

	int id;
	EditorState* state;
};

#endif
