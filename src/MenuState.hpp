#ifndef MENUSTATE_HPP_INCLUDED
#define MENUSTATE_HPP_INCLUDED
#include "common.hpp"
#include "EditorState.hpp"
#include "dialogs/Dialog.hpp"


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
	GUI_FILE_IMPORT = 205,
	GUI_FILE_EXPORT = 206,
	GUI_FILE_EXIT = 207,

	// Edit
	GUI_EDIT_UNDO = 208,
	GUI_EDIT_REDO = 209,
	GUI_EDIT_SNAP = 210,
	GUI_EDIT_LIMIT = 211,

	// View
	GUI_VIEW_SP_ALL = 212,
	GUI_VIEW_SP_PER = 213,
	GUI_VIEW_SP_TOP = 214,
	GUI_VIEW_SP_FRT = 215,
	GUI_VIEW_SP_RHT = 216,

	// Tools
	GUI_PROJ_NEW_BOX = 217,
	GUI_PROJ_DELETE_BOX = 218,
	GUI_PROJ_IMAGE_IM = 219,

	// Help
	GUI_HELP_HELP = 220,
	GUI_HELP_ABOUT = 221,

	// Sidebar
	GUI_SIDEBAR = 222,

	// File Dialog
	GUI_FILEDIALOG_PATH = 222 + SIDEBAR_MAX_IDS,
	GUI_FILEDIALOG_FORM = 223 + SIDEBAR_MAX_IDS,
	GUI_FILEDIALOG_BROWSER = 224 + SIDEBAR_MAX_IDS,
	GUI_DIALOG_SUBMIT = 225 + SIDEBAR_MAX_IDS,
	GUI_DIALOG = 226 + SIDEBAR_MAX_IDS
};

class EditorState;
class Dialog;
class MenuState{
public:
	MenuState(EditorState* state);
	void init();
	void draw(IVideoDriver* driver);
	bool OnEvent(const SEvent& event);
	EditorState *state;
	IGUIStaticText *sidebar;
	Dialog *dialog;
private:
	IGUIContextMenu *projectMenubar;
	IGUIContextMenu* menubar;
	void addFileDialog(FileParserType type,
			const wchar_t* title, const wchar_t* button);
	bool mode_icons_open;
};

#endif
