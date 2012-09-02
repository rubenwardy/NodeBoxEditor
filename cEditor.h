#ifndef _cEditor_h_included_
#define _cEditor_h_included_

#include "cNodes.h"

class cEditor : public IEventReceiver
{
public:
	bool run(IrrlichtDevice* irr_device);
	void loadUI();
	virtual bool OnEvent(const SEvent& event);
private:
	cNode* nodes[5];
	int curId;
	void OnMenuItemSelected(IGUIContextMenu* menu);
	IrrlichtDevice* device;
	IVideoDriver* driver;
	ISceneManager* smgr;
	ISceneCollisionManager* coli;
	IGUIEnvironment* guienv;

	ISceneNode* pivot;
	ICameraSceneNode* camera;
};

#endif