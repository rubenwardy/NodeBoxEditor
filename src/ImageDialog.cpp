#include "ImageDialog.hpp"
#include "util/string.hpp"
#include "util/filesys.hpp"
#include "TextureDialog.hpp"

ImageDialog::ImageDialog(EditorState *mstate, Node *tnode, CubeSide tface):
	Dialog(mstate),
	node(tnode),
	face(tface)
{
	IGUIEnvironment *guienv = state->device->getGUIEnvironment();
	win = guienv->addWindow(rect<irr::s32>(340, 50, 669, 120), true, L"Import Image");
	guienv->addButton(rect<irr::s32>(250, 30, 320, 60), win, GUI_DIALOG_SUBMIT, L"Import");	
	IGUIEditBox *box = guienv->addEditBox(L"", rect<irr::s32>(10, 30, 240, 60),
			true, win, GUI_FILEDIALOG_PATH);
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
	delete state->menu->dialog;
	state->menu->dialog = NULL;
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

	if (event.GUIEvent.EventType == EGET_BUTTON_CLICKED && event.GUIEvent.Caller->getID() == GUI_DIALOG_SUBMIT) {
		IGUIEditBox *box = static_cast<IGUIEditBox *>(win->getElementFromId(GUI_FILEDIALOG_PATH));
		irr::core::stringc t = box->getText();
		std::string path(t.c_str(), t.size());
		path = trim(path);
		std::cerr << path.c_str() << std::endl;
		if (!state->project->media.add(filenameWithExt(path).c_str(), state->device->getVideoDriver()->createImageFromFile(path.c_str()))) {
			state->device->getGUIEnvironment()->addMessageBox(L"Unable to import",
					L"Failed to open the image\n\t(Does it not exist, or is it readonly?)");
			return true;
		}
		if (node) {
			TextureDialog::show(state, node, face);
		} else {
			close();
		}
		return true;
	}
	return false;
}
