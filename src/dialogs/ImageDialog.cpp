#include "ImageDialog.hpp"
#include "../util/string.hpp"
#include "../util/filesys.hpp"
#include "TextureDialog.hpp"
#include "../util/tinyfiledialogs.h"

enum IMAGE_DIALOG_GUI_IDS
{
	EID_GUI_ID_BROWSE = GUI_DIALOG,
	EID_GUI_ID_NAME_AS,
	EID_GUI_ID_OVERWRITE
};

ImageDialog::ImageDialog(EditorState *mstate, Node *tnode, ECUBE_SIDE tface):
	Dialog(mstate),
	node(tnode),
	face(tface)
{
	IGUIEnvironment *guienv = state->device->getGUIEnvironment();
	win = guienv->addWindow(rect<irr::s32>(340, 50, 669, 50+180), true, L"Import Image");
	guienv->addEditBox(L"", rect<irr::s32>(10, 30, 240, 60),
			true, win, GUI_FILEDIALOG_PATH);
	guienv->addEditBox(L"", rect<irr::s32>(10, 70, 240, 100),
			true, win, EID_GUI_ID_NAME_AS);
	guienv->addButton(rect<irr::s32>(250, 30, 320, 60), win, EID_GUI_ID_BROWSE, L"Browse");
	guienv->addCheckBox(false, rect<irr::s32>(30, 110, 270, 130), win,
			EID_GUI_ID_OVERWRITE, L"Overwrite Existing");
	guienv->addButton(rect<irr::s32>(130, 140, 200, 170), win, GUI_DIALOG_SUBMIT, L"Import");
}


bool ImageDialog::show(EditorState *mstate, Node *node, ECUBE_SIDE face)
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
	if (event.GUIEvent.EventType == EGET_BUTTON_CLICKED) {
		if (event.GUIEvent.Caller->getID() == EID_GUI_ID_BROWSE) {
			std::string path = getSaveLoadDirectory(state->settings->get("save_directory"),
					state->isInstalled);
			const char* filters[] = {"*.png", "*.jpg", "*.gif", "*.jpeg"};
			const char *cfile = tinyfd_openFileDialog("Select Image",
					path.c_str(), 1, filters, 0);

			if (!cfile)
				return false;

			std::string file = cfile;

			if (file == "")
				return false;


			IGUIEditBox *box = static_cast<IGUIEditBox *>(win->getElementFromId(GUI_FILEDIALOG_PATH));
			IGUIEditBox *nameas = static_cast<IGUIEditBox *>(win->getElementFromId(EID_GUI_ID_NAME_AS));

			box->setText(narrow_to_wide(file).c_str());
			nameas->setText(narrow_to_wide(filenameWithExt(file)).c_str());
		} else if (event.GUIEvent.Caller->getID() == GUI_DIALOG_SUBMIT) {
			IGUIEditBox *box = static_cast<IGUIEditBox *>(win->getElementFromId(GUI_FILEDIALOG_PATH));
			IGUIEditBox *nameas = static_cast<IGUIEditBox *>(win->getElementFromId(EID_GUI_ID_NAME_AS));
			IGUICheckBox *cb = static_cast<IGUICheckBox *>(win->getElementFromId(EID_GUI_ID_OVERWRITE));
			irr::core::stringc t = box->getText();
			irr::core::stringc t2 = nameas->getText();

			// Get path and filename
			std::string path(t.c_str(), t.size());
			std::string shortname(t2.c_str(), t2.size());
			path = trim(path);
			std::cerr << path.c_str() << std::endl;

			// Check for existance in file manager
			bool already_exists = (state->project->media.get(shortname.c_str()) != NULL);

			// Do import or show warning
			if (!already_exists || cb->isChecked()) {
				IImage *image = state->device->getVideoDriver()->createImageFromFile(path.c_str());
				if (!image) {
					state->device->getGUIEnvironment()->addMessageBox(L"Unable to import",
							L"Failed to open the image\n\t(Does it not exist, or is it readonly?)");
					return true;
				} else if (!state->project->media.add(path.c_str(), shortname, image, cb->isChecked())) {
					state->device->getGUIEnvironment()->addMessageBox(L"Unable to import",
							L"Failed to add the image\n\t(Unknown reason)");
					return true;
				}
				state->project->remesh();
				if (node)
					TextureDialog::show(state, node, face);
				else
					close();
				return true;
			} else {
				state->device->getGUIEnvironment()->addMessageBox(L"Unable to import",
				L"An image with the same filename has already been imported.\n\t(Change nameas or click overwrite.)");
			}
		}
	}
	return false;
}
