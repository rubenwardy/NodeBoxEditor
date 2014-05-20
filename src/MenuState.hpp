#ifndef MENUSTATE_HPP_INCLUDED
#define MENUSTATE_HPP_INCLUDED
#include "common.hpp"
#include "EditorState.hpp"


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

#define SIDEBAR_MAX_IDS 20
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
	GUI_PROJ_LIST_AREA = 240 + SIDEBAR_MAX_IDS,

	// Help
	GUI_HELP_HELP = 218,
	GUI_HELP_ABOUT = 219,

	// Sidebar
	GUI_SIDEBAR = 220,

	// File Dialog
	GUI_FILEDIALOG_PATH = 220 + SIDEBAR_MAX_IDS,
	GUI_FILEDIALOG_FORM = 221 + SIDEBAR_MAX_IDS
};

class EditorState;
class MenuState{
public:
	MenuState(EditorState* state);
	void init();
	void draw(IVideoDriver* driver);
	bool OnEvent(const SEvent& event);
	EditorState *state;
	IGUIStaticText *sidebar;

private:
	IGUIContextMenu *projectMenubar;
	IGUIContextMenu* menubar;
	IGUIWindow* addFileDialog(FileParserType type, int submit,
			const wchar_t* title, const wchar_t* button);
	bool mode_icons_open;
};

#endif
