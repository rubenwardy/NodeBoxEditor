#include "MenuState.h"
#include "FileParser.h"

MenuState::MenuState(EditorState* state)
:_state(state),_projectmb(NULL),menubar(NULL),mode_icons_open(false)
{}


void MenuState::init(){
	IGUIEnvironment* guienv = GetState()->GetDevice()->getGUIEnvironment();
	guienv->clear();
	guienv->getSkin()->setFont(guienv->getFont("media/fontlucida.png"));

	// Main menu bar
	menubar=guienv->addMenu();
	menubar->addItem(L"File",-1,true,true);
	menubar->addItem(L"Edit",-1,true,true);
	menubar->addItem(L"View",-1,true,true);
	menubar->addItem(L"Project",-1,true,true);
	menubar->addItem(L"Help",-1,true,true);
	gui::IGUIContextMenu* submenu;

	// File
	submenu = menubar->getSubMenu(0);
	//submenu->addItem(L"New Project",GUI_FILE_NEW_PROJECT,false);
	//submenu->addItem(L"New Item",GUI_FILE_NEW_ITEM,false);
	//submenu->addSeparator();
	submenu->addItem(L"Open Project",GUI_FILE_OPEN_PROJECT);
	submenu->addSeparator();
	submenu->addItem(L"Save Project",GUI_FILE_SAVE_PROJECT);
	submenu->addItem(L"Export",GUI_FILE_EXPORT);
	submenu->addSeparator();
	submenu->addItem(L"Exit",GUI_FILE_EXIT);

	// Edit
	submenu = menubar->getSubMenu(1);
	//submenu->addItem(L"Undo",GUI_EDIT_UNDO,false);
	//submenu->addItem(L"Redo",GUI_EDIT_REDO,false);
	//submenu->addSeparator();
	submenu->addItem(
		L"Snapping",GUI_EDIT_SNAP,true,false,
		GetState()->Settings()->getSettingAsBool("snapping"),
		true
	);
	submenu->addItem(
		L"Limiting",GUI_EDIT_LIMIT,true,false,
		GetState()->Settings()->getSettingAsBool("limiting"),
		true
	);

	// View
	submenu = menubar->getSubMenu(2);
	submenu->addItem(L"Tiled View",GUI_VIEW_SP_ALL);
	submenu->addItem(L"Perspective View",GUI_VIEW_SP_PER);
	submenu->addItem(L"Top View",GUI_VIEW_SP_TOP);
	submenu->addItem(L"Front View",GUI_VIEW_SP_FRT);
	submenu->addItem(L"Side View",GUI_VIEW_SP_RHT);

	// Project
	_projectmb = menubar->getSubMenu(3);

	// Help
	submenu = menubar->getSubMenu(4);
	//submenu->addItem(L"Help",GUI_HELP_HELP,false);
	submenu->addItem(L"About",GUI_HELP_ABOUT);

	// Sidebar root
	u32 top = menubar->getAbsoluteClippingRect().LowerRightCorner.Y;
	_sidebar = guienv->addStaticText(L"Loading...",
		rect<s32>(
			(GetState()->GetDevice()->getVideoDriver()->getScreenSize().Width - 246), top+10,
			GetState()->GetDevice()->getVideoDriver()->getScreenSize().Width, GetState()->GetDevice()->getVideoDriver()->getScreenSize().Height
		),false,true,0,GUI_SIDEBAR
	);
	_sidebar->setAlignment(EGUIA_LOWERRIGHT,EGUIA_LOWERRIGHT,EGUIA_UPPERLEFT,EGUIA_UPPERLEFT);
}

bool MenuState::OnEvent(const SEvent& event){
	if (
		event.EventType == irr::EET_MOUSE_INPUT_EVENT &&
		event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN
	){
		if (rect<s32>(10,32,42,64).isPointInside(GetState()->mouse_position)){
				mode_icons_open = !mode_icons_open;
				return true;
		}
		if (mode_icons_open){
			if (!(rect<s32>(10,32,210,64).isPointInside(GetState()->mouse_position))){
				mode_icons_open = false;
				return true;
			}
			EditorMode* curs = GetState()->Mode();
			int x = 0;
			for (int i=0;i<5;i++){
				EditorMode* m = GetState()->Mode(i);

				if (m && m!=curs){
					if (rect<s32>(47+37*x,32,79+37*x,64).isPointInside(GetState()->mouse_position)){
						// Run select
						GetState()->SelectMode(i);
						mode_icons_open = false;
						return true;
					}
					x ++;
				}
			}
		}
	}
	if (event.EventType == EET_GUI_EVENT){
		if (event.GUIEvent.EventType == EGET_MENU_ITEM_SELECTED){
			IGUIContextMenu* menu = (IGUIContextMenu*)event.GUIEvent.Caller;
			switch (menu->getItemCommandId(menu->getSelectedItem())){
			case GUI_FILE_OPEN_PROJECT:
				{
					addFileDialog(EFPT_LOAD_PROJ,GUI_FILE_OPEN_PROJECT,L"Open Project",L"Open");
					return true;
				}
				break;
			case GUI_FILE_SAVE_PROJECT:
				{
					if (!GetState()->project){
						GetState()->GetDevice()->getGUIEnvironment()->addMessageBox(L"Unable to save",L"You have not yet opened a project.");
						return true;
					}
					addFileDialog(EFPT_SAVE_PROJ,GUI_FILE_SAVE_PROJECT,L"Save Project",L"Save");
					return true;
				}
				break;
			case GUI_FILE_EXPORT:
				{
					if (!GetState()->project){
						GetState()->GetDevice()->getGUIEnvironment()->addMessageBox(L"Unable to save",L"You have not yet opened a project.");
						return true;
					}
					addFileDialog(EFPT_EXPORT,GUI_FILE_SAVE_PROJECT,L"Export",L"Export");
					return true;
				}
				break;
			case GUI_FILE_EXIT:
				{
					IGUIEnvironment* guienv = GetState()->GetDevice()->getGUIEnvironment();
					IGUIWindow* win = guienv->addWindow(rect<irr::s32>(100,100,356,215),true,L"Are you sure?");
					guienv->addButton(rect<irr::s32>(128-40,80,128+40,105),win,GUI_FILE_EXIT,L"Close",L"Close the editor");
					return true;
				}
				break;
			case GUI_EDIT_SNAP:
				{					
					if (menu->isItemChecked(menu->getSelectedItem()))
						GetState()->Settings()->setStringSetting("snapping","true");
					else
						GetState()->Settings()->setStringSetting("snapping","false");

					menu->setItemChecked(menu->getSelectedItem(),GetState()->Settings()->getSettingAsBool("snapping"));
				}
				break;
			case GUI_EDIT_LIMIT:
				{					
					if (menu->isItemChecked(menu->getSelectedItem()))
						GetState()->Settings()->setStringSetting("limiting","true");
					else
						GetState()->Settings()->setStringSetting("limiting","false");

					menu->setItemChecked(menu->getSelectedItem(),GetState()->Settings()->getSettingAsBool("limiting"));
				}
				break;
			case GUI_HELP_ABOUT:
				{
					core::stringw msg = L"The Nodebox Editor\n"
						L"Version: ";

					msg.append(EDITOR_TEXT_VERSION);
					msg.append(
						L"\n\n"
						L"This free nodebox editor was made by rubenwardy in C++ and Irrlicht.\n"
						L"You can download newer versions from the Minetest forum."
					);

					GetState()->GetDevice()->getGUIEnvironment()->addMessageBox(L"About",msg.c_str());
					return true;
				}
				break;
			}	
		}else if(event.GUIEvent.EventType == EGET_BUTTON_CLICKED){
			switch (event.GUIEvent.Caller->getID()){
			case GUI_FILE_EXIT:
				{
					if (GetState()->project){
						NBEFileParser parser(GetState());
						parser.save(GetState()->project,"exit.nbe");
					}
					GetState()->CloseEditor();
					return true;
				}
				break;
			case GUI_FILE_SAVE_PROJECT:
				{
					if (!GetState()->project){
						GetState()->GetDevice()->getGUIEnvironment()->addMessageBox(L"Unable to save",L"You have not yet opened a project.");
						return true;
					}
					IGUIWindow* parent = static_cast<IGUIWindow*>(event.GUIEvent.Caller->getParent());
					if (!parent){
						GetState()->GetDevice()->getGUIEnvironment()->addMessageBox(L"Unable to save",L"Error trying to get the dialog.");
						return true;
					}
					IGUIEditBox* box = static_cast<IGUIEditBox*>(parent->getElementFromId(GUI_FILEDIALOG_PATH));
					IGUIComboBox* cb = static_cast<IGUIComboBox*>(parent->getElementFromId(GUI_FILEDIALOG_FORM));

					if (!box || !cb){						
						GetState()->GetDevice()->getGUIEnvironment()->addMessageBox(L"Unable to save",L"Error trying to get the dialog's contents.");
						return true;
					}

					FileParser* parser = getFromType(cb->getItemData(cb->getSelected()),GetState());
					if (parser){
						stringc after = box->getText();

						if (after.findFirst('.')==-1)
							after.append(parser->getEXT());

						parser->save(GetState()->project,after);
						delete parser;
					}else{
						GetState()->GetDevice()->getGUIEnvironment()->addMessageBox(L"Unable to save",L"File format does not exist.");
						return true;
					}

					// Remove
					parent->remove();
				}
				break;
			case GUI_FILE_OPEN_PROJECT:
				{
					IGUIWindow* parent = static_cast<IGUIWindow*>(event.GUIEvent.Caller->getParent());
					if (!parent){
						GetState()->GetDevice()->getGUIEnvironment()->addMessageBox(L"Unable to open",L"Error trying to get the dialog.");
						return true;
					}
					IGUIEditBox* box = static_cast<IGUIEditBox*>(parent->getElementFromId(GUI_FILEDIALOG_PATH));
					IGUIComboBox* cb = static_cast<IGUIComboBox*>(parent->getElementFromId(GUI_FILEDIALOG_FORM));

					if (!box || !cb){						
						GetState()->GetDevice()->getGUIEnvironment()->addMessageBox(L"Unable to open",L"Error trying to get the dialog's contents.");
						return true;
					}

					FileParser* parser = getFromType(cb->getItemData(cb->getSelected()),GetState());
					if (parser){
						stringc after = box->getText();

						if (after.findFirst('.')==-1)
							after.append(parser->getEXT());
		
						Project* tmp = parser->open(after);

						if (tmp){
							delete GetState()->project;
							GetState()->project = tmp;
							GetState()->project->SelectNode(0);
							GetState()->Mode()->unload();
							init();
							GetState()->Mode()->load();
						}else{
							GetState()->GetDevice()->getGUIEnvironment()->addMessageBox(L"Unable to open",L"Something went wrong.");
						}
						delete parser;
						return true;
					}else{
						GetState()->GetDevice()->getGUIEnvironment()->addMessageBox(L"Unable to open",L"File format does not exist.");
						return true;
					}

					// Remove
					parent->remove();
				}
				break;
			}
		}
	}else if (event.EventType == EET_KEY_INPUT_EVENT){
		if (event.KeyInput.Control && event.KeyInput.Key == KEY_KEY_S && !event.KeyInput.PressedDown){
			if (!GetState()->project){
				GetState()->GetDevice()->getGUIEnvironment()->addMessageBox(L"Unable to save",L"You have not yet opened a project.");
				return true;
			}
			addFileDialog(EFPT_SAVE_PROJ,GUI_FILE_SAVE_PROJECT,L"Save Project",L"Save");
			return true;
		}
	}
	return false;
}

void MenuState::draw(IVideoDriver* driver){
	EditorState* state = GetState();
	EditorMode* curs = state->Mode();

	if (state->Settings()->getSettingAsBool("hide_sidebar")) {
		GetSideBar()->setVisible(false);
	} else {
		GetSideBar()->setVisible(true);
		u32 top = menubar->getAbsoluteClippingRect().LowerRightCorner.Y;
		state->GetDevice()->getGUIEnvironment()->getSkin()
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

IGUIWindow* MenuState::addFileDialog(FileParserType type,int submit,const wchar_t* title,const wchar_t* button){
	IGUIEnvironment* guienv = GetState()->GetDevice()->getGUIEnvironment();
	IGUIWindow* win = guienv->addWindow(rect<irr::s32>(340, 50, 669, 160),true,title);
	guienv->addButton(rect<irr::s32>(250, 30, 320, 60),win,submit,button);
	stringc path = GetState()->project->name.trim();
	if (path == ""){
		path = "test";
	}
	IGUIEditBox* box = guienv->addEditBox(convert(path.c_str()),rect<irr::s32>(10, 30, 240, 60),true,win,GUI_FILEDIALOG_PATH);

	IGUIComboBox* cb = guienv->addComboBox(rect<irr::s32>(60, 70, 240, 100),win,GUI_FILEDIALOG_FORM);
	IGUIStaticText* txt = guienv->addStaticText(L"Format:",rect<irr::s32>(10, 70, 60, 100),false,true,win,-1,false);
					
	if (box)
		txt->setTextAlignment(EGUIA_UPPERLEFT,EGUIA_CENTER);

	if (cb){
		if (type==EFPT_SAVE_PROJ || type==EFPT_LOAD_PROJ)
			cb->addItem(L"NodeBoxEditor file (*.nbe)",0);

		if (type==EFPT_EXPORT)
			cb->addItem(L"Lua file (*.lua)",1);

		if (cb->getItemCount() > 0)
			cb->setSelected(0);
	}
	return win;
}
