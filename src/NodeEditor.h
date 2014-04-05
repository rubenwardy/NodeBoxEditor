#ifndef _NODEEDITOR_H_INCLUDED_
#define _NODEEDITOR_H_INCLUDED_
#include "common.h"
#include "EditorState.h"

class EditorMode;
class NodeEditor :public EditorMode
{
public:
	NodeEditor(EditorState* st);

	// Interface
	virtual void load();
	virtual void unload();
	virtual void update(double dtime);
	virtual void draw(irr::video::IVideoDriver* driver);
	virtual void viewportTick(VIEWPORT window,irr::video::IVideoDriver* driver,rect<s32> offset);
	virtual bool OnEvent(const irr::SEvent &event);
	virtual const char* icon() const{ return "media/icon_mode_node.png"; }

	// The gui id numbers for this mode
	// NOTE: the maximum that can be here is 20
	//       see in MenuState.h to raise limit
	enum ENG_GUI{
		ENG_GUI_MAIN_LISTBOX = GUI_SIDEBAR + 1,
		ENG_GUI_MAIN_ADD = GUI_SIDEBAR + 2,
		ENG_GUI_MAIN_DEL = GUI_SIDEBAR + 3,
		ENG_GUI_MAIN_EDIT = GUI_SIDEBAR + 4,
		ENG_GUI_PROP = GUI_SIDEBAR + 5,
		ENG_GUI_PROP_NAME = GUI_SIDEBAR + 6,
		ENG_GUI_PROP_X = GUI_SIDEBAR + 7,
		ENG_GUI_PROP_Y = GUI_SIDEBAR + 8,
		ENG_GUI_PROP_Z = GUI_SIDEBAR + 9,
		ENG_GUI_PROP_UPDATE = GUI_SIDEBAR + 10,
		ENG_GUI_PROP_REVERT = GUI_SIDEBAR + 11		
	};
private:
	void load_ui();
	void fillProperties();
};

#endif
