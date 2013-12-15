#ifndef _EDITOR_H_INCLUDED_
#define _EDITOR_H_INCLUDED_
#include "common.h"
#include "EditorState.h"
#include "NBEditor.h"
#include "Project.h"
#include "FileParser.h"

class Editor :public IEventReceiver
{
public:
	Editor();
	bool run(IrrlichtDevice* irr_device,Configuration* conf);
	virtual bool OnEvent(const SEvent& event);
	EditorState* GetState(){return _state;}
	irr::IrrlichtDevice* GetDevice()const{return _device;}
private:
	EditorState* _state;
	irr::IrrlichtDevice* _device;

	// Helper functions
	//void LoadUI();
	int currentWindow;
	ISceneNode* target;
	ISceneNode* pivot;
	ICameraSceneNode* camera[4];
	void LoadScene();
};

#endif