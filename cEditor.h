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
	IGUIEnvironment* guienv;

	bool isSplitScreen;
	int currentWindow;


	// Hold Navigation
	ISceneNode* pivot;
	ICameraSceneNode* camera[4];
	bool allow_input;
};

#endif