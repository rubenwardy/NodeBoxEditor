#ifndef EDITOR_H_INCLUDED
#define EDITOR_H_INCLUDED

#include "common.h"
#include "EditorState.h"
#include "NodeEditor.h"
#include "Project.h"

class Editor : public IEventReceiver
{
public:
	Editor();
	bool run(IrrlichtDevice *irr_device, Configuration *conf);
	virtual bool OnEvent(const SEvent &event);

	EditorState *state;
	irr::IrrlichtDevice *device;
private:
	// Helper functions
	int currentWindow;
	ISceneNode *target;
	ISceneNode *pivot;
	ICameraSceneNode *camera[4];
	void LoadScene();
};

#endif
