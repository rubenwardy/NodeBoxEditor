#ifndef _EDITOR_H_INCLUDED_
#define _EDITOR_H_INCLUDED_
#include "common.h"
#include "Project.h"
#include "NBPropertiesDialog.h"
#include "CodeDialog.h"

class Editor : public IEventReceiver
{
public:
	Editor();

	// Run the editor
	bool run(IrrlichtDevice* irr_device);

	// Load the GUI
	void Load_UI();
	void Load_Scene();

	// OnEvent handler
	virtual bool OnEvent(const SEvent& event);
private:
	// Current project
	Project* project;

	// Handle MenuItem Selection
	void OnMenuItemSelected(IGUIContextMenu* menu);

	// Label data
	ed_data* data;

	// Hold Irrlicht Stuff
	IrrlichtDevice* device;
	IVideoDriver* driver;
	ISceneManager* smgr;
	ISceneCollisionManager* coli;

	// GUI
	IGUIEnvironment* guienv;
	bool isSplitScreen;
	int currentWindow;
	vector2di mouse_position;
	bool mouse_down;
	bool KEY_MOVE_DOWN;
	bool KEY_SCALE_DOWN;
	ITriangleSelector* plane_tri;
	list<IEventReceiver*>* menus;
	void UpdateEditToolMode();
	IGUIContextMenu* menubar;

	// Hold Navigation
	ISceneNode* target;
	ISceneNode* pivot;
	ICameraSceneNode* camera[4];
	bool allow_input;

	// Points and Scalers
	CDR* points[16];
	int point_on;
	void updatePoint(int start, int count);
	void addPoint(int id, CDR_TYPE type, int camera);

	void MoveTarget(vector3df pos);
};

#endif