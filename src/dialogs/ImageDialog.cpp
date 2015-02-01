#include "ImageDialog.hpp"
#include "../util/string.hpp"
#include "../util/filesys.hpp"
#include "TextureDialog.hpp"

enum IMAGE_DIALOG_GUI_IDS
{
	EID_GUI_ID_WARNING = GUI_DIALOG,
	EID_GUI_ID_NAME_AS,
	EID_GUI_ID_OVERWRITE
};

ImageDialog::ImageDialog(EditorState *mstate, Node *tnode, CubeSide tface):
	Dialog(mstate),
	node(tnode),
	face(tface)
{
	IGUIEnvironment *guienv = state->device->getGUIEnvironment();
	win = guienv->addWindow(rect<irr::s32>(340, 50, 669, 180), true, L"Import Image");
	guienv->addEditBox(L"", rect<irr::s32>(10, 30, 240, 60),
			true, win, GUI_FILEDIALOG_PATH);
	guienv->addEditBox(L"", rect<irr::s32>(10, 70, 240, 100),
			true, win, EID_GUI_ID_NAME_AS);
	guienv->addButton(rect<irr::s32>(250, 30, 320, 60), win, GUI_DIALOG_SUBMIT, L"Import");
	guienv->addCheckBox(false, rect<irr::s32>(250, 70, 320, 100), win,
			EID_GUI_ID_OVERWRITE, L"Overwrite existing");
}


bool ImageDialog::show(EditorState *mstate, Node *node, CubeSide face)
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
	ImageDialog *dialog = new ImageDialog(mstate, node, face);
	if (!dialog) {
		std::cerr << "Failed to create dialog!" << std::endl;
		return false;
	}
	mstate->menu->dialog = dialog;
	return true;
}

bool ImageDialog::canClose()
{
	return true;
}

bool ImageDialog::close()
{
	if (win) {
		win->remove();
		win = NULL;
	}
	state->menu->dialog = NULL;
	delete this;
	return true;
}

bool ImageDialog::OnEvent(const SEvent &event)
{
	if (event.EventType != EET_GUI_EVENT)
		return false;

	if (event.GUIEvent.EventType == EGET_ELEMENT_CLOSED && event.GUIEvent.Caller == win) {
		if (canClose())
			close();
		return true;
	}

	//
	// On 'import' clicked
	//
	if (event.GUIEvent.EventType == EGET_BUTTON_CLICKED && event.GUIEvent.Caller->getID() == GUI_DIALOG_SUBMIT) {
		IGUIEditBox *box = static_cast<IGUIEditBox *>(win->getElementFromId(GUI_FILEDIALOG_PATH));
		irr::core::stringc t = box->getText();

		// Get path and filename
		std::string path(t.c_str(), t.size());
		path = getSaveLoadDirectory(state->settings->get("save_directory"), state->settings->getBool("installed")) + path;
		path = trim(path);
		std::cerr << path.c_str() << std::endl;
		const char *shortname = trim(filenameWithExt(path)).c_str();

		// Check for existance in file manager
		bool already_exists = (state->project->media.get(shortname) != NULL);

		// Do import or show warning
		if (!already_exists) {
			IImage *image = state->device->getVideoDriver()->createImageFromFile(path.c_str());
			if (!image) {
				state->device->getGUIEnvironment()->addMessageBox(L"Unable to import",
						L"Failed to open the image\n\t(Does it not exist, or is it readonly?)");
				return true;
			} else if (!state->project->media.add(path.c_str(), image)) {
				state->device->getGUIEnvironment()->addMessageBox(L"Unable to import",
						L"Failed to add the image\n\t(Has an image with the same name already been added?)");
				return true;
			}
			state->project->remesh();
			if (node) {
				TextureDialog::show(state, node, face);
			} else {
				close();
			}
			return true;
		} else {
			// Show warning message, as overwriting.
			state->device->getGUIEnvironment()->addStaticText(L"An image with the same filename already exists.",
					rect<irr::s32>(10, 100, 240, 120), false, true, win, EID_GUI_ID_WARNING);
		}
	}
	return false;
}
