#ifndef MENUSTATE_HPP_INCLUDED
#define MENUSTATE_HPP_INCLUDED
#include "common.hpp"
#include "EditorState.hpp"
#include "dialogs/Dialog.hpp"

#define SIDEBAR_MAX_IDS 20
enum GUI_ID
{
	// File
	GUI_FILE_NEW_PROJECT = 201,
	GUI_FILE_NEW_ITEM,
	GUI_FILE_OPEN_PROJECT,
	GUI_FILE_SAVE_PROJECT,
	GUI_FILE_RUN_IN_MINETEST,
	GUI_FILE_EXPORT_LUA,
	GUI_FILE_EXPORT_CPP,
	GUI_FILE_EXPORT_MOD,
	GUI_FILE_EXPORT_OBJ,
	GUI_FILE_EXPORT_TEX,
	GUI_FILE_IMPORT,
	GUI_FILE_EXIT,

	// Edit
	GUI_EDIT_UNDO,
	GUI_EDIT_REDO,
	GUI_EDIT_SNAP,
	GUI_EDIT_LIMIT,

	// View
	GUI_VIEW_SP_ALL,
	GUI_VIEW_SP_PER,
	GUI_VIEW_SP_TOP,
	GUI_VIEW_SP_FRT,
	GUI_VIEW_SP_RHT,

	// Tools
	GUI_PROJ_NEW_BOX,
	GUI_PROJ_DELETE_BOX,
	GUI_PROJ_IMAGE_IM,

	// Help
	GUI_HELP_HELP,
	GUI_HELP_ABOUT,

	// Sidebar
	GUI_SIDEBAR,

	// File Dialog
	GUI_FILEDIALOG_PATH = GUI_SIDEBAR + SIDEBAR_MAX_IDS,
	GUI_DIALOG_SUBMIT,
	GUI_DIALOG
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
	bool mode_icons_open;
};

#endif
