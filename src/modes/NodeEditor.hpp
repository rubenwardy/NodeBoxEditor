#ifndef NODEEDITOR_HPP_INCLUDED
#define NODEEDITOR_HPP_INCLUDED
#include "../common.hpp"
#include "../EditorState.hpp"

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
	virtual void viewportTick(EViewport window, irr::video::IVideoDriver* driver, rect<s32> offset);
	virtual bool OnEvent(const irr::SEvent &event);
	virtual irr::video::ITexture* icon();

	// The GUI ID numbers for this mode
	// NOTE: the maximum that can be here is 20
	//       see in MenuState.h to raise limit
	enum NODE_EDITOR_GUI_ID {
		ENG_GUI_MAIN_LISTBOX = GUI_SIDEBAR + 1,
		ENG_GUI_MAIN_ADD,
		ENG_GUI_MAIN_DEL,
		ENG_GUI_MAIN_EDIT,
		ENG_GUI_PROP,
		ENG_GUI_PROP_NAME,
		ENG_GUI_PROP_X,
		ENG_GUI_PROP_Y,
		ENG_GUI_PROP_Z,
		ENG_GUI_PROP_SNAP_RES,
		ENG_GUI_PROP_UPDATE,
		ENG_GUI_PROP_REVERT
	};
private:
	void load_ui();
	void fillProperties();
	void updateProperties();
};

#endif
