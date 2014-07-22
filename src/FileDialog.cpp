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
	std::cerr << "x0" << std::endl;
	if (win) {
		std::cerr << "x1" << std::endl;
		win->remove();
		win = NULL;
	}
	std::cerr << "x2" << std::endl;
	delete state->menu->dialog;
	std::cerr << "x3" << std::endl;
	state->menu->dialog = NULL;
	std::cerr << "x4" << std::endl;
	return true;
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

void FileDialog::doSave(const SEvent &event)
{
	if (!state->project) {
		state->device->getGUIEnvironment()->addMessageBox(L"Unable to save",
				L"You have not yet opened a project.");
		return;
	}
	IGUIWindow *parent = NULL;
	IGUIEditBox *box = NULL;
	IGUIComboBox *cb = NULL;
	if (!get_parent_box_cb(state, event, &parent, &box, &cb)) {
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
			std::cerr << "Failed to save file for unknown reason." << std::endl;
			state->device->getGUIEnvironment()->addMessageBox(L"Unable to save",
					L"File writer failed.");
		}
		delete writer;
	} else {
		state->device->getGUIEnvironment()->addMessageBox(L"Unable to save",
				L"File format does not exist.");
	}

	close();
}

void FileDialog::doOpen(const SEvent &event)
{
	IGUIWindow *parent = NULL;
	IGUIEditBox *box = NULL;
	IGUIComboBox *cb = NULL;
	if (!get_parent_box_cb(state, event, &parent, &box, &cb)) {
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

		state->project = tmp;
		state->project->remesh();
		state->project->SelectNode(0);
		state->Mode()->unload();
		state->menu->init();
		state->Mode()->load();
		delete parser;
		parser = NULL;
	} else {
		state->device->getGUIEnvironment()->addMessageBox(L"Unable to open",
				L"Reading file failed.");					
		delete parser;
	}
	close();
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

	if (event.GUIEvent.EventType == EGET_BUTTON_CLICKED && event.GUIEvent.Caller->getID() == GUI_DIALOG_SUBMIT) {
		if (parser_type == EFPT_SAVE_PROJ || parser_type == EFPT_EXPORT)
			doSave(event);
		else
			doOpen(event);
		return true;
	}
	return false;
}
