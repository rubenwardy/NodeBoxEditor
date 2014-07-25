#include "FileDialog.hpp"
#include "util/string.hpp"
#include "util/filesys.hpp"
#include "FileFormat/FileFormat.hpp"

FileDialog::FileDialog(EditorState *mstate, FileParserType type,
		const wchar_t* title, const wchar_t* button):
	Dialog(mstate), parser_type(type)
{
	IGUIEnvironment *guienv = state->device->getGUIEnvironment();
	win = guienv->addWindow(rect<irr::s32>(340, 50, 669, 160), true, title);
	guienv->addButton(rect<irr::s32>(250, 30, 320, 60), win, GUI_DIALOG_SUBMIT, button);
#if !(IRRLICHT_VERSION_MAJOR == 1 && IRRLICHT_VERSION_MINOR < 8)
	if (parser_type == EFPT_EXPORT) {
		guienv->addButton(rect<irr::s32>(250, 65, 320, 95), win, GUI_DIALOG_SUBMIT + 1, L"View");
	}
#endif
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
		if (type == EFPT_SAVE_PROJ || type == EFPT_LOAD_PROJ || type == EFPT_IMPORT) {
			cb->addItem(L"NodeBoxEditor file (*.nbe)", FILE_FORMAT_NBE);
		}
		if (type == EFPT_EXPORT) {
			cb->addItem(L"Lua file (*.lua)", FILE_FORMAT_LUA);
			cb->addItem(L"Minetest Classic (*.cpp)", FILE_FORMAT_MTC);
		}

		if (cb->getItemCount() > 0)
			cb->setSelected(0);
	}
}


bool FileDialog::show(EditorState *mstate, FileParserType type,
		const wchar_t* title, const wchar_t* button)
{
	// Check if last can close.
	if (mstate->menu->dialog != NULL) {
		if (mstate->menu->dialog->canClose()) {
			mstate->menu->dialog->close();
		} else {
			return false;
		}
	}

	// Create new
	FileDialog *dialog = new FileDialog(mstate, type, title, button);
	if (!dialog) {
		std::cerr << "Failed to create dialog!" << std::endl;
		return false;
	}
	mstate->menu->dialog = dialog;
	return true;
}

bool FileDialog::canClose()
{
	return true;
}

bool FileDialog::close()
{
	if (win) {
		win->remove();
		win = NULL;
	}
	delete state->menu->dialog;
	state->menu->dialog = NULL;
	return true;
}

static bool get_parent_box_cb(EditorState *state,
		const SEvent &event,
		IGUIWindow *parent,
		IGUIEditBox **box,
		IGUIComboBox **cb)
{
	*box = static_cast<IGUIEditBox *>(parent->getElementFromId(GUI_FILEDIALOG_PATH));
	*cb = static_cast<IGUIComboBox *>(parent->getElementFromId(GUI_FILEDIALOG_FORM));

	if (!(*box) || !(*cb)) {
		state->device->getGUIEnvironment()->addMessageBox(L"Unable to save",
				L"Error trying to get the dialog's contents.");
		return false;
	}
	return true;
}

void FileDialog::doSave(const SEvent &event)
{
	if (!state->project) {
		state->device->getGUIEnvironment()->addMessageBox(L"Unable to save",
				L"You have not yet opened a project.");
		return;
	}
	IGUIEditBox *box = NULL;
	IGUIComboBox *cb = NULL;
	if (!get_parent_box_cb(state, event, win, &box, &cb)) {
		return;
	}
	FileFormat *writer = getFromType((FileFormatType) cb->getItemData(cb->getSelected()), state);
	if (writer) {
		irr::core::stringc t = box->getText();
		std::string after(t.c_str(), t.size());

		if (after.find('.') == std::string::npos) {
			after += '.';
			after += writer->getExtension();
		}

		std::string dir = getSaveLoadDirectory(state->settings->get("save_directory"), state->settings->getBool("installed"));
		std::cerr << "Saving to " << dir + after << std::endl;
		if (!writer->write(state->project, dir + after)) {
			if (writer->error_code == EFFE_IO_ERROR) {
				state->device->getGUIEnvironment()->addMessageBox(L"Unable to Save",
						L"Unable to open file to save to");
			} else {
				state->device->getGUIEnvironment()->addMessageBox(L"Unable to Save",
						L"Unknown reason");
			}
		}
		delete writer;
	} else {
		state->device->getGUIEnvironment()->addMessageBox(L"Unable to save",
				L"File format does not exist.");
	}

	close();
}

void FileDialog::doText(const SEvent &event)
{
	IGUIEnvironment *guienv = state->device->getGUIEnvironment();
	if (!state->project) {
		guienv->addMessageBox(L"Unable to save",
				L"You have not yet opened a project.");
		return;
	}
	IGUIEditBox *box = NULL;
	IGUIComboBox *cb = NULL;
	if (!get_parent_box_cb(state, event, win, &box, &cb)) {
		return;
	}
	FileFormat *writer = getFromType((FileFormatType) cb->getItemData(cb->getSelected()), state);
	if (writer) {
		std::string res = writer->getAsString(state->project);
		if (res == "") {
			guienv->addMessageBox(L"Unable to generate code",
					L"Unknown reason");
		}
		IGUIWindow *dialog = guienv->addWindow(rect<irr::s32>(340, 50, 740, 350), false, L"Code Window");
		IGUIEditBox *db = guienv->addEditBox(narrow_to_wide(res).c_str(), rect<s32>(5, 25, 395, 295), false, dialog);
		db->setMultiLine(true);
		db->setWordWrap(false);
		delete writer;
	} else {
		state->device->getGUIEnvironment()->addMessageBox(L"Unable to save",
				L"File format does not exist.");
	}

	close();
}

void FileDialog::doOpen(const SEvent &event)
{
	IGUIEditBox *box = NULL;
	IGUIComboBox *cb = NULL;
	if (!get_parent_box_cb(state, event, win, &box, &cb)) {
		return;
	}

	// Get file parser
	FileFormat *parser = getFromType((FileFormatType) cb->getItemData(cb->getSelected()), state);
	if (!parser) {
		state->device->getGUIEnvironment()->addMessageBox(L"Unable to open",
				L"File format does not exist.");
		return;
	}

	// Get file name
	irr::core::stringc t = box->getText();
	std::string after(t.c_str(), t.size());
	if (after.find('.') == std::string::npos) {
		after += '.';
		after += parser->getExtension();
	}

	// Get directory, and load
	std::string dir = getSaveLoadDirectory(state->settings->get("save_directory"), state->settings->getBool("installed"));
	std::cerr << "Reading from " << dir + after << std::endl;
	Project *tmp = parser->read(dir + after);
	if (tmp) {
		if (state->project)
			delete state->project;

		win->remove();
		win = NULL;
		state->project = tmp;
		state->project->remesh();
		state->project->SelectNode(0);
		state->Mode()->unload();
		state->menu->init();
		state->Mode()->load();
		delete parser;
		parser = NULL;
		close();
		return;
	} else {
		switch(parser->error_code) {
		case EFFE_IO_ERROR:
			state->device->getGUIEnvironment()->addMessageBox(L"Unable to open",
					L"Failed to open the file\n\t(Does it not exist, or is it readonly?)");
			break;
		case EFFE_READ_OLD_VERSION:
			state->device->getGUIEnvironment()->addMessageBox(L"Unable to open",
					L"This file is outdated and is not supported");
			break;
		case EFFE_READ_NEW_VERSION:
			state->device->getGUIEnvironment()->addMessageBox(L"Unable to open",
					L"This file was created with a new version of NBE\n\t(Update your copy)");
			break;
		case EFFE_READ_PARSE_ERROR:
			state->device->getGUIEnvironment()->addMessageBox(L"Unable to open",
					L"An error occurred while reading the file - it may be corrupted\n\t(This should never happen)");
			break;
		case EFFE_READ_WRONG_TYPE:
			state->device->getGUIEnvironment()->addMessageBox(L"Unable to open",
					L"The file is not in the correct format\n\t(Are you opening the wrong type of file?)");
			break;
		default:
			state->device->getGUIEnvironment()->addMessageBox(L"Unable to open",
					L"Unknown error");
			break;
		}			
		delete parser;
		parser = NULL;
	}
}

void FileDialog::doImport(const SEvent &event)
{
	IGUIEditBox *box = NULL;
	IGUIComboBox *cb = NULL;
	if (!get_parent_box_cb(state, event, win, &box, &cb)) {
		return;
	}

	// Get file parser
	FileFormat *parser = getFromType((FileFormatType) cb->getItemData(cb->getSelected()), state);
	if (!parser) {
		state->device->getGUIEnvironment()->addMessageBox(L"Unable to open",
				L"File format does not exist.");
		return;
	}

	// Get file name
	irr::core::stringc t = box->getText();
	std::string after(t.c_str(), t.size());
	if (after.find('.') == std::string::npos) {
		after += '.';
		after += parser->getExtension();
	}

	// Get directory, and load
	std::string dir = getSaveLoadDirectory(state->settings->get("save_directory"), state->settings->getBool("installed"));
	std::cerr << "Reading from " << dir + after << std::endl;
	Project *tmp = parser->read(dir + after, state->project);
	if (tmp) {
		state->project->remesh();
		close();
	} else {
		switch(parser->error_code) {
		case EFFE_IO_ERROR:
			state->device->getGUIEnvironment()->addMessageBox(L"Unable to open",
					L"Failed to open the file\n\t(Does it not exist, or is it readonly?)");
			break;
		case EFFE_READ_OLD_VERSION:
			state->device->getGUIEnvironment()->addMessageBox(L"Unable to open",
					L"This file is outdated and is not supported");
			break;
		case EFFE_READ_NEW_VERSION:
			state->device->getGUIEnvironment()->addMessageBox(L"Unable to open",
					L"This file was created with a new version of NBE\n\t(Update your copy)");
			break;
		case EFFE_READ_PARSE_ERROR:
			state->device->getGUIEnvironment()->addMessageBox(L"Unable to open",
					L"An error occurred while reading the file - it may be corrupted\n\t(This should never happen)");
			break;
		case EFFE_READ_WRONG_TYPE:
			state->device->getGUIEnvironment()->addMessageBox(L"Unable to open",
					L"The file is not in the correct format\n\t(Are you opening the wrong type of file?)");
			break;
		default:
			state->device->getGUIEnvironment()->addMessageBox(L"Unable to open",
					L"Unknown error");
			break;
		}			
		delete parser;
		parser = NULL;
	}
}

bool FileDialog::OnEvent(const SEvent &event)
{
	if (event.EventType != EET_GUI_EVENT)
		return false;

	if (event.GUIEvent.EventType == EGET_ELEMENT_CLOSED && event.GUIEvent.Caller == win) {
		if (canClose())
			close();
		return true;		
	}

	if (event.GUIEvent.EventType == EGET_BUTTON_CLICKED) {
		if (event.GUIEvent.Caller->getID() == GUI_DIALOG_SUBMIT) {
			if (parser_type == EFPT_SAVE_PROJ || parser_type == EFPT_EXPORT)
				doSave(event);
			else if (parser_type == EFPT_IMPORT)
				doImport(event);
			else
				doOpen(event);
			return true;
		} else if (event.GUIEvent.Caller->getID() == GUI_DIALOG_SUBMIT + 1) {
			doText(event);
			return true;
		}
	}
	return false;
}
