#ifndef _NODEEDITOR_H_INCLUDED_
#define _NODEEDITOR_H_INCLUDED_
#include "common.h"
#include "EditorState.h"

class EditorMode;
class NodeEditor :public EditorMode
{
public:
	NodeEditor(EditorState* st):EditorMode(st){}

	// Interface
	virtual void load(){}
	virtual void unload(){}
	virtual void update(double dtime){}
	virtual void draw(irr::video::IVideoDriver* driver){}
	virtual void viewportTick(VIEWPORT window,irr::video::IVideoDriver* driver,rect<s32> offset){}
	virtual bool OnEvent(const irr::SEvent &event){return false;}
	virtual const char* icon() const{ return "media/icon_mode_node.png"; }
};

#endif
