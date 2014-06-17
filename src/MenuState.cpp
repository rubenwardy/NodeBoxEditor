#include "util/string.hpp"
#include "MenuState.hpp"
#include "FileFormat/FileFormat.hpp"
#include "FileFormat/NBE.hpp"

MenuState::MenuState(EditorState* state) :
	state(state),
	projectMenubar(NULL),
	menubar(NULL),
	mode_icons_open(false)
{}


void MenuState::init()
{
	IGUIEnvironment *guienv = state->device->getGUIEnvironment();
	guienv->clear();
	guienv->getSkin()->setFont(guienv->getFont("media/fontlucida.png"));

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
	submenu->addItem(L"Perspective View", GUI_VIEW_SP_PER);
	submenu->addItem(L"Top View", GUI_VIEW_SP_TOP);
	submenu->addItem(L"Front View", GUI_VIEW_SP_FRT);
	submenu->addItem(L"Side View", GUI_VIEW_SP_RHT);

	// Project
	projectMenubar = menubar->getSubMenu(3);

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

static bool get_parent_box_cb(EditorState *state,
		const SEvent &event,
		IGUIWindow **parent,
		IGUIEditBox **box,
		IGUIComboBox **cb)
{
	*parent = static_cast<IGUIWindow *>(event.GUIEvent.Caller->getParent());
	if (!(*parent)) {
		state->device->getGUIEnvironment()->addMessageBox(L"Unable to save",
				L"Error trying to get the dialog.");
		return false;
	}
	*box = static_cast<IGUIEditBox *>((*parent)->getElementFromId(GUI_FILEDIALOG_PATH));
	*cb = static_cast<IGUIComboBox *>((*parent)->getElementFromId(GUI_FILEDIALOG_FORM));

	if (!(*box) || !(*cb)) {
		state->device->getGUIEnvironment()->addMessageBox(L"Unable to save",
				L"Error trying to get the dialog's contents.");
		return false;
	}
	return true;
}

bool MenuState::OnEvent(const SEvent& event){
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
				addFileDialog(EFPT_LOAD_PROJ, GUI_FILE_OPEN_PROJECT,
						L"Open Project", L"Open");
				return true;
				break;
			case GUI_FILE_SAVE_PROJECT:
				if (!state->project) {
					state->device->getGUIEnvironment()->addMessageBox(
							L"Unable to save",
							L"You have not yet opened a project.");
					return true;
				}
				addFileDialog(EFPT_SAVE_PROJ, GUI_FILE_SAVE_PROJECT,
						L"Save Project", L"Save");
				return true;
				break;
			case GUI_FILE_EXPORT:
				if (!state->project) {
					state->device->getGUIEnvironment()->addMessageBox(
							L"Unable to save",
							L"You have not yet opened a project.");
					return true;
				}
				addFileDialog(EFPT_EXPORT, GUI_FILE_SAVE_PROJECT, L"Export", L"Export");
				return true;
				break;
			case GUI_FILE_EXIT: {
				IGUIEnvironment *guienv = state->device->getGUIEnvironment();
				IGUIWindow *win = guienv->addWindow(rect<irr::s32>(100, 100, 356, 215),
						true, L"Are you sure?");
				guienv->addButton(rect<irr::s32>(128 - 40, 80, 128 + 40, 105),
						win, GUI_FILE_EXIT, L"Close", L"Close the editor");
				return true;
				break;
			}
			case GUI_EDIT_SNAP:
				if (menu->isItemChecked(menu->getSelectedItem())) {
					state->settings->set("snapping", "true");
				} else {
					state->settings->set("snapping", "false");
				}

				menu->setItemChecked(menu->getSelectedItem(),
						state->settings->getBool("snapping"));
				break;
			case GUI_EDIT_LIMIT:
				if (menu->isItemChecked(menu->getSelectedItem())) {
					state->settings->set("limiting", "true");
				} else {
					state->settings->set("limiting", "false");
				}

				menu->setItemChecked(menu->getSelectedItem(),
						state->settings->getBool("limiting"));
				break;
			case GUI_HELP_ABOUT: {
				core::stringw msg = L"The Nodebox Editor\n"
					L"Version: ";

				msg += EDITOR_TEXT_VERSION;
				msg += L"\n\n"
					L"This free nodebox editor was made by rubenwardy in C++ and Irrlicht.\n"
					L"You can download newer versions from the Minetest forum.";

				state->device->getGUIEnvironment()->addMessageBox(L"About", msg.c_str());
				return true;
				break;
			}
			}
		} else if (event.GUIEvent.EventType == EGET_BUTTON_CLICKED) {
			switch (event.GUIEvent.Caller->getID()) {
			case GUI_FILE_EXIT:
				if (state->project) {
					NBEFileFormat writer(state);
					writer.write(state->project, "exit.nbe");
				}
				state->CloseEditor();
				return true;
				break;
			case GUI_FILE_SAVE_PROJECT: {
				if (!state->project) {
					state->device->getGUIEnvironment()->addMessageBox(L"Unable to save",
							L"You have not yet opened a project.");
					return true;
				}
				IGUIWindow *parent = NULL;
				IGUIEditBox *box = NULL;
				IGUIComboBox *cb = NULL;
				if (!get_parent_box_cb(state, event, &parent, &box, &cb)) {
					return true;
				}
				FileFormat *writer = getFromType((FileFormatType) cb->getItemData(cb->getSelected()), state);
				if (writer) {
					irr::core::stringc t = box->getText();
					std::string after(t.c_str(), t.size());

					if (after.find('.') == std::string::npos) {
						after += '.';
						after += writer->getExtension();
					}

					if (!writer->write(state->project, after)) {
						state->device->getGUIEnvironment()->addMessageBox(L"Unable to save",
								L"File writer failed.");
					}
					delete writer;
				} else {
					state->device->getGUIEnvironment()->addMessageBox(L"Unable to save",
							L"File format does not exist.");
					return true;
				}

				parent->remove();
				break;
			}
			case GUI_FILE_OPEN_PROJECT: {
				IGUIWindow *parent = NULL;
				IGUIEditBox *box = NULL;
				IGUIComboBox *cb = NULL;
				if (!get_parent_box_cb(state, event, &parent, &box, &cb)) {
					return true;
				}

				FileFormat *parser = getFromType((FileFormatType) cb->getItemData(cb->getSelected()), state);
				if (!parser) {
					state->device->getGUIEnvironment()->addMessageBox(L"Unable to open",
							L"File format does not exist.");
					return true;
				}
				irr::core::stringc t = box->getText();
				std::string after(t.c_str(), t.size());

				if (after.find('.') == std::string::npos) {
					after += '.';
					after += parser->getExtension();
				}

				Project *tmp = parser->read(after);

				if (!tmp) {
					state->device->getGUIEnvironment()->addMessageBox(L"Unable to open",
							L"Reading file failed.");
				}
				delete state->project;
				state->project = tmp;
				state->project->remesh();
				state->project->SelectNode(0);
				state->Mode()->unload();
				init();
				state->Mode()->load();
				delete parser;
				return true;
				break;
			}
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
			addFileDialog(EFPT_SAVE_PROJ, GUI_FILE_SAVE_PROJECT, L"Save Project", L"Save");
			return true;
		}
	}
	return false;
}

void MenuState::draw(IVideoDriver* driver){
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

IGUIWindow *MenuState::addFileDialog(FileParserType type, int submit,
		const wchar_t *title, const wchar_t *button)
{
	IGUIEnvironment *guienv = state->device->getGUIEnvironment();
	IGUIWindow *win = guienv->addWindow(rect<irr::s32>(340, 50, 669, 160), true, title);
	guienv->addButton(rect<irr::s32>(250, 30, 320, 60), win, submit, button);
	std::string path = trim(state->project->name);
	if (path.empty()) {
		path = "test";
	}
	IGUIEditBox *box = guienv->addEditBox(narrow_to_wide(path).c_str(),
			rect<irr::s32>(10, 30, 240, 60),
			true, win, GUI_FILEDIALOG_PATH);

	IGUIComboBox* cb = guienv->addComboBox(rect<irr::s32>(60, 70, 240, 100),
			win, GUI_FILEDIALOG_FORM);
	IGUIStaticText* txt = guienv->addStaticText(L"Format:",
			rect<irr::s32>(10, 70, 60, 100), false,
			true, win, -1, false);

	if (box)
		txt->setTextAlignment(EGUIA_UPPERLEFT, EGUIA_CENTER);

	if (cb) {
		if (type == EFPT_SAVE_PROJ || type == EFPT_LOAD_PROJ) {
			cb->addItem(L"NodeBoxEditor file (*.nbe)", FILE_FORMAT_NBE);
		}
		if (type == EFPT_EXPORT) {
			cb->addItem(L"Lua file (*.lua)", FILE_FORMAT_LUA);
			cb->addItem(L"Minetest Classic (*.cpp)", FILE_FORMAT_MTC);
		}

		if (cb->getItemCount() > 0)
			cb->setSelected(0);
	}
	return win;
}

