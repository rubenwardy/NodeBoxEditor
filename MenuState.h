#ifndef _MENUSTATE_H_INCLUDED_
#define _MENUSTATE_H_INCLUDED_
#include "common.h"
#include "EditorState.h"


// FILE
// --> New Project
// --> New Item
// -----------------
// --> Open Project
// -----------------
// --> Save Project
// --> Export
// --------------------
// --> Exit
//
// EDIT
// --> Undo
// --> Redo
// --------------------
// --> Snapping
// --> Limiting
//
// VIEW
// --> Tiled View
// --> Perspective View
// --> Top View
// --> Front View
// --> Side View
//
// PROJECT
// --> New Node box
// --> Delete Node box
// --------------------
// --> [nodeboxes]
//
// HELP
// --> Help Index
// --> About

enum GUI_ID
{
	// File
	GUI_FILE_NEW_PROJECT = 201,
	GUI_FILE_NEW_ITEM = 202,
	GUI_FILE_OPEN_PROJECT = 203,
	GUI_FILE_SAVE_PROJECT = 204,
	GUI_FILE_EXPORT = 205,
	GUI_FILE_EXIT = 206,

	// Edit
	GUI_EDIT_UNDO = 207,
	GUI_EDIT_REDO = 208,
	GUI_EDIT_SNAP = 209,
	GUI_EDIT_LIMIT = 210,

	// View
	GUI_VIEW_SP_ALL = 211,
	GUI_VIEW_SP_PER = 212,
	GUI_VIEW_SP_TOP = 213,
	GUI_VIEW_SP_FRT = 214,
	GUI_VIEW_SP_RHT = 215,

	// Tools
	GUI_PROJ_NEW_BOX = 216,
	GUI_PROJ_DELETE_BOX = 217,
	GUI_PROJ_LIST_AREA = 240,

	// Help
	GUI_HELP_HELP =218,
	GUI_HELP_ABOUT = 219,

	// Sidebar
	GUI_SIDEBAR_TITLE = 220,
	GUI_SIDEBAR_LISTBOX = 221,
	GUI_SIDEBAR_LABEL = 222,

	// File Dialog
	GUI_FILEDIALOG_PATH = 223,
	GUI_FILEDIALOG_FORM = 224
};

class EditorState;
class MenuState{
public:
	MenuState(EditorState* state);
	void init();
	void draw(IVideoDriver* driver);
	bool OnEvent(const SEvent& event);
	EditorState* GetState() const{return _state;}
	IGUIContextMenu* GetMenu() const{return menubar;}
	IGUIContextMenu* GetProjectMenu() const{return _projectmb;}
	IGUIStaticText* GetSideBar() const{return _sidebar;}
private:
	EditorState* _state;
	IGUIContextMenu* menubar;
	IGUIContextMenu* _projectmb;
	IGUIStaticText* _sidebar;
	IGUIWindow* addFileDialog(FileParserType type,int submit,const wchar_t* title,const wchar_t* button);
};

#endif