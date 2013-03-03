#ifndef _cEditor_h_included_
#define _cEditor_h_included_

#include "define.h"
#include "cNodes.h"



class cEditor : public IEventReceiver
{
public:
	cEditor();

	// Run the editor
	bool run(IrrlichtDevice* irr_device);

	// Load the GUI
	void loadUI();

	// OnEvent handler
	virtual bool OnEvent(const SEvent& event);
private:
	// Handle MenuItem Selection
	void OnMenuItemSelected(IGUIContextMenu* menu);

	// Label data
	ed_data* data;

	// Nodes
	cNode* nodes[5];

	// Current Node Id
	int curId;

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

	// Hold Navigation
	ISceneNode* pivot;
	ICameraSceneNode* camera[4];
	bool allow_input;

	// Points and Scalers
	CDR* points[12];
	int point_on;
	void updatePoint(int start, int count);
	void addPoint(int id, CDR_TYPE type);
};

#endif