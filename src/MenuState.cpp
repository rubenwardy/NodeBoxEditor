#include "util/string.hpp"
#include "util/filesys.hpp"
#include "MenuState.hpp"
#include "FileFormat/FileFormat.hpp"
#include "FileFormat/NBE.hpp"
#include "FileDialog.hpp"
#include "ImageDialog.hpp"

MenuState::MenuState(EditorState* state) :
	state(state),
	projectMenubar(NULL),
	menubar(NULL),
	mode_icons_open(false),
	dialog(NULL)
{}


void MenuState::init()
{
	IGUIEnvironment *guienv = state->device->getGUIEnvironment();
	guienv->clear();
	guienv->getSkin()->setFont(guienv->getFont("media/fontlucida.png"));
	if (dialog) {
		dialog->close();
		if (dialog) {
			delete dialog;
			dialog = NULL;		
		}
	}

	// Main menu bar
	menubar = guienv->addMenu();
	menubar->addItem(L"File", -1, true, true);
	menubar->addItem(L"Edit", -1, true, true);
	menubar->addItem(L"View", -1, true, true);
	menubar->addItem(L"Project", -1, true, true);
	menubar->addItem(L"Help", -1, true, true);
	gui::IGUIContextMenu *submenu;

	// File
	submenu = menubar->getSubMenu(0);
	submenu->addItem(L"Open Project", GUI_FILE_OPEN_PROJECT);
	submenu->addSeparator();
	submenu->addItem(L"Save Project", GUI_FILE_SAVE_PROJECT);
	submenu->addItem(L"Export", GUI_FILE_EXPORT);
	submenu->addSeparator();
	submenu->addItem(L"Exit", GUI_FILE_EXIT);

	// Edit
	submenu = menubar->getSubMenu(1);
	submenu->addItem(
		L"Snapping", GUI_EDIT_SNAP, true, false,
		state->settings->getBool("snapping"),
		true
	);
	submenu->addItem(
		L"Limiting", GUI_EDIT_LIMIT, true, false,
		state->settings->getBool("limiting"),
		true
	);

	// View
	submenu = menubar->getSubMenu(2);
	submenu->addItem(L"Tiled View", GUI_VIEW_SP_ALL);
	submenu->addSeparator();
	submenu->addItem(L"Top Left", GUI_VIEW_SP_PER);
	submenu->addItem(L"Top Right", GUI_VIEW_SP_TOP);
	submenu->addItem(L"Bottom Left", GUI_VIEW_SP_FRT);
	submenu->addItem(L"Bottom Right", GUI_VIEW_SP_RHT);

	// Project
	projectMenubar = menubar->getSubMenu(3);
	projectMenubar->addItem(L"Import Image", GUI_PROJ_IMAGE_IM);


	// Help
	submenu = menubar->getSubMenu(4);
	//submenu->addItem(L"Help",GUI_HELP_HELP,false);
	submenu->addItem(L"About", GUI_HELP_ABOUT);

	// Sidebar root
	u32 top = menubar->getAbsoluteClippingRect().LowerRightCorner.Y;
	sidebar = guienv->addStaticText(L"Loading...",
		rect<s32>(
			state->device->getVideoDriver()->getScreenSize().Width - 246,
			top + 10,
			state->device->getVideoDriver()->getScreenSize().Width,
			state->device->getVideoDriver()->getScreenSize().Height
		), false, true, 0, GUI_SIDEBAR
	);
	sidebar->setAlignment(EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
}

bool MenuState::OnEvent(const SEvent& event){
	if (dialog)
		return dialog->OnEvent(event);
	
	if (event.EventType == irr::EET_MOUSE_INPUT_EVENT &&
			event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN) {
		if (rect<s32>(10, 32, 42, 64).isPointInside(state->mouse_position)){
			mode_icons_open = !mode_icons_open;
			return true;
		}
		if (mode_icons_open) {
			if (!(rect<s32>(10, 32, 210, 64).isPointInside(state->mouse_position))) {
				mode_icons_open = false;
				return true;
			}
			EditorMode *curs = state->Mode();
			int x = 0;
			for (int i = 0; i < 5; i++){
				EditorMode *m = state->Mode(i);

				if (m && m != curs){
					if (rect<s32>(47 + 37 * x, 32, 79 + 37 * x, 64)
							.isPointInside(state->mouse_position)) {
						state->SelectMode(i);
						mode_icons_open = false;
						return true;
					}
					x++;
				}
			}
		}
	}
	if (event.EventType == EET_GUI_EVENT){
		if (event.GUIEvent.EventType == EGET_MENU_ITEM_SELECTED){
			IGUIContextMenu *menu = (IGUIContextMenu *)event.GUIEvent.Caller;
			switch (menu->getItemCommandId(menu->getSelectedItem())){
			case GUI_FILE_OPEN_PROJECT:
				addFileDialog(EFPT_LOAD_PROJ, L"Open Project", L"Open");
				return true;
			case GUI_FILE_SAVE_PROJECT:
				if (!state->project) {
					state->device->getGUIEnvironment()->addMessageBox(
							L"Unable to save",
							L"You have not yet opened a project.");
					return true;
				}
				addFileDialog(EFPT_SAVE_PROJ, L"Save Project", L"Save");
				return true;
			case GUI_FILE_EXPORT:
				if (!state->project) {
					state->device->getGUIEnvironment()->addMessageBox(
							L"Unable to save",
							L"You have not yet opened a project.");
					return true;
				}
				addFileDialog(EFPT_EXPORT, L"Export", L"Export");
				return true;
			case GUI_FILE_EXIT: {
				IGUIEnvironment *guienv = state->device->getGUIEnvironment();
				IGUIWindow *win = guienv->addWindow(rect<irr::s32>(100, 100, 356, 215),
						true, L"Are you sure?");
				guienv->addButton(rect<irr::s32>(128 - 40, 80, 128 + 40, 105),
						win, GUI_FILE_EXIT, L"Close", L"Close the editor");
				return true;
			}
			case GUI_EDIT_SNAP:
				if (menu->isItemChecked(menu->getSelectedItem())) {
					state->settings->set("snapping", "true");
				} else {
					state->settings->set("snapping", "false");
				}

				menu->setItemChecked(menu->getSelectedItem(),
						state->settings->getBool("snapping"));
				return true;
			case GUI_EDIT_LIMIT:
				if (menu->isItemChecked(menu->getSelectedItem())) {
					state->settings->set("limiting", "true");
				} else {
					state->settings->set("limiting", "false");
				}

				menu->setItemChecked(menu->getSelectedItem(),
						state->settings->getBool("limiting"));
				return true;
			case GUI_PROJ_IMAGE_IM:
				ImageDialog::show(state, NULL, ECS_TOP);
				return true;
			case GUI_HELP_ABOUT: {
				core::stringw msg = L"The Nodebox Editor\n"
					L"Version: ";

				msg += EDITOR_TEXT_VERSION;
				msg += L"\n\n"
					L"This free nodebox editor was made by rubenwardy in C++ and Irrlicht.\n"
					L"You can download newer versions from the Minetest forum.";

				state->device->getGUIEnvironment()->addMessageBox(L"About", msg.c_str());
				return true;
			}
			}
		} else if (event.GUIEvent.EventType == EGET_BUTTON_CLICKED) {
			switch (event.GUIEvent.Caller->getID()) {
			case GUI_FILE_EXIT:
				if (state->project) {
					NBEFileFormat writer(state);

					// Get directory to save to
					std::string dir = getSaveLoadDirectory(state->settings->get("save_directory"), state->settings->getBool("installed"));
					
					std::cerr << "Saving to " << dir + "exit.nbe" << std::endl;
					if (!writer.write(state->project, dir + "exit.nbe"))
						std::cerr << "Failed to save file for unknown reason." << std::endl;
				}
				state->CloseEditor();
				return true;
			}
		}
	} else if (event.EventType == EET_KEY_INPUT_EVENT){
		if (event.KeyInput.Control && event.KeyInput.Key == KEY_KEY_S &&
				!event.KeyInput.PressedDown) {
			if (!state->project) {
				state->device->getGUIEnvironment()->addMessageBox(L"Unable to save",
						L"You have not yet opened a project.");
				return true;
			}
			addFileDialog(EFPT_SAVE_PROJ, L"Save Project", L"Save");
			return true;
		}
	}
	return false;
}

void MenuState::draw(IVideoDriver *driver){
	EditorMode* curs = state->Mode();

	if (state->settings->getBool("hide_sidebar")) {
		sidebar->setVisible(false);
	} else {
		sidebar->setVisible(true);
		u32 top = menubar->getAbsoluteClippingRect().LowerRightCorner.Y;
		state->device->getGUIEnvironment()->getSkin()
			->draw3DWindowBackground(NULL, false, 0,
				rect<s32>((driver->getScreenSize().Width - 256),
					top,
					driver->getScreenSize().Width,
					driver->getScreenSize().Height
				)
			);
	}

	if (dialog)
		return;

	if (curs) {
		driver->draw2DImage(curs->icon(),
			rect<s32>(10, 32, 42, 64),
			rect<s32>( 0,  0, 32, 32),
			0, 0, true);
	}

	if (mode_icons_open) {
		int x = 0;
		for (int i = 0; i < 5; i++) {
			EditorMode* m = state->Mode(i);

			if (m && m != curs) {
				driver->draw2DImage(m->icon(),
					rect<s32>(47 + 37 * x, 32, 79 + 37 * x, 64),
					rect<s32>(0, 0, 32, 32),
					0, 0, true);
				x++;
			}
		}
	}
}

void MenuState::addFileDialog(FileParserType type,
		const wchar_t *title, const wchar_t *button)
{
	FileDialog::show(state, type, title, button);
}

