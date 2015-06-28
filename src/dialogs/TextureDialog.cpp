#include "TextureDialog.hpp"
#include "../util/string.hpp"
#include "ImageDialog.hpp"
#include "../util/filesys.hpp"
#include "../util/tinyfiledialogs.h"

enum TEXTURE_DIALOG_GUI_IDS
{
	ETD_GUI_ID_APPLY = GUI_DIALOG,
	ETD_GUI_ID_IMPORT,
	ETD_GUI_ID_ACTIONS,
	ETD_GUI_ID_ACTIONS_CM,
	ETD_GUI_ID_EXPORT
};

const char* getCubeSideName(CubeSide face)
{
	switch (face) {
	case ECS_TOP:
		return "top";
	case ECS_BOTTOM:
		return "bottom";
	case ECS_RIGHT:
		return "right";
	case ECS_LEFT:
		return "left";
	case ECS_FRONT:
		return "front";
	case ECS_BACK:
		return "back";
	default:
		std::cerr << "Error in getCubeSideName() - Unknown cubeside given" << std::endl;
		return ":/";
	}
}

TextureDialog::TextureDialog(EditorState *pstate, Node *pnode, CubeSide pface):
	Dialog(pstate),
	node(pnode),
	face(pface),
	lb(NULL),
	the_image(NULL),
	context(NULL)
{
	IVideoDriver *driver = state->device->getVideoDriver();
	IGUIEnvironment *guienv = state->device->getGUIEnvironment();

	// Window and basic items
	win = guienv->addWindow(rect<s32>(340, 50, 340 + 74 * 3 + 10, 50 + 74 * 3 + 10), true,
			narrow_to_wide(std::string(getCubeSideName(face)) + " texture").c_str());
	guienv->addButton(rect<s32>(155, 30, 74*3, 55), win, ETD_GUI_ID_APPLY,   L"Apply",  L"Apply this texture selection to the node face");
	guienv->addButton(rect<s32>(155, 60, 74*3, 85), win, ETD_GUI_ID_IMPORT,  L"Import", L"Import images from files");
	guienv->addButton(rect<s32>(84,  60, 150,  85), win, ETD_GUI_ID_ACTIONS, L"Actions");

	// Fill out listbox
	lb = guienv->addListBox(rect<s32>(10, 104, 74 * 3, 74 * 3), win, 502);
	Media *media = &state->project->media;
	std::map<std::string, Media::Image*>& images = media->getList();
	int count = 1;
	lb->addItem(L"");
	lb->setSelected(0);
	for (std::map<std::string, Media::Image*>::const_iterator it = images.begin();
			it != images.end();
			++it) {
		if (!it->second) {
			continue;
		}
		if (it->second->name == "default") {
			lb->addItem(L"");
		} else {
			lb->addItem(narrow_to_wide(it->second->name + " [used " +
					num_to_str(it->second->getHolders()) + " times]").c_str());
		}
		if (it->second == node->getTexture(face))
			lb->setSelected(count);
		count++;
	}

	Media::Image *image = node->getTexture(face);
	if (image) {
		if (the_image)
			driver->removeTexture(the_image);
		the_image = driver->addTexture("tmpicon.png", image->get());
	}

	// Context menu
	context = guienv->addContextMenu(rect<s32>(84, 85, 150, 180), win, ETD_GUI_ID_ACTIONS_CM);
	context->addItem(L"Export", ETD_GUI_ID_EXPORT);
	context->setCloseHandling(ECMC_HIDE);
	context->setVisible(false);
	context->setEventParent(win);
}

void TextureDialog::draw(IVideoDriver *driver)
{
	int x = win->getAbsolutePosition().UpperLeftCorner.X + 10;
	int y = win->getAbsolutePosition().UpperLeftCorner.Y + 30;
	if (!the_image) {
		driver->draw2DRectangle(SColor(100, 0, 0, 0), rect<s32>(x, y, x + 64, y + 64));
	} else {
		driver->draw2DImage(the_image, rect<s32>(x, y, x + 64, y + 64),
				rect<s32>(0, 0, the_image->getSize().Width, the_image->getSize().Height));
	}
}


bool TextureDialog::show(EditorState *mstate, Node *pnode, CubeSide pface)
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
	TextureDialog *dialog = new TextureDialog(mstate, pnode, pface);
	if (!dialog) {
		std::cerr << "Failed to create dialog!" << std::endl;
		return false;
	}
	mstate->menu->dialog = dialog;
	return true;
}

bool TextureDialog::canClose()
{
	return true;
}

bool TextureDialog::close()
{
	IVideoDriver *driver = state->device->getVideoDriver();
	if (the_image)
		driver->removeTexture(the_image);
	win->remove();
	state->menu->dialog = NULL;
	delete this;
	return true;
}

bool TextureDialog::OnEvent(const SEvent &event)
{
	if (event.EventType != EET_GUI_EVENT)
		return false;

	IVideoDriver *driver = state->device->getVideoDriver();

	if (event.GUIEvent.EventType == EGET_BUTTON_CLICKED) {
		switch (event.GUIEvent.Caller->getID()) {
		case ETD_GUI_ID_APPLY: {
			if (lb->getSelected() == 0) {
				node->setTexture(face, NULL);
				node->remesh();
				return true;
			}

			int count = 0;
			Media *media = &state->project->media;
			std::map<std::string, Media::Image*>& images = media->getList();
			for (std::map<std::string, Media::Image*>::const_iterator it = images.begin();
					it != images.end();
					++it) {
				if (count == lb->getSelected()-1) {
					node->setTexture(face, it->second);
					node->remesh();
					break;
				}
				count++;
			}


			close();
			return true;
		}
		case ETD_GUI_ID_IMPORT: {
			ImageDialog::show(state, node, face);
			return false;
		}
		case ETD_GUI_ID_ACTIONS:
			context->setVisible(true);
			state->device->getGUIEnvironment()->setFocus(context);
			return false;
		}
	} else if (event.GUIEvent.EventType == EGET_MENU_ITEM_SELECTED) {
		IGUIContextMenu *menu = (IGUIContextMenu *)event.GUIEvent.Caller;
		switch (menu->getItemCommandId(menu->getSelectedItem())) {
		case ETD_GUI_ID_EXPORT: {
			if (lb->getSelected() == 0)
				return true;

			int count = 0;
			Media *media = &state->project->media;
			Media::Image *image = NULL;
			std::map<std::string, Media::Image*>& images = media->getList();
			for (std::map<std::string, Media::Image*>::const_iterator it = images.begin();
					it != images.end();
					++it) {
				if (count == lb->getSelected() - 1) {
					image = it->second;
					break;
				}
				count++;
			}

			if (!image)
				return true;

			std::string path = getSaveLoadDirectory(state->settings->get("save_directory"),
					state->isInstalled);
			path += image->name;

			const char *filters[] = {"*.png"};
			const char *cfilename = tinyfd_saveFileDialog("Save Image", path.c_str(),
					1, filters);
			if (!cfilename)
				return true;

			std::string filename = cfilename;

			if (filename == "")
				return true;

			state->device->getVideoDriver()->writeImageToFile(image->get(),
					filename.c_str());

			return true;
		}} // end of switch
	} else if (event.GUIEvent.EventType == EGET_LISTBOX_CHANGED && event.GUIEvent.Caller == lb) {
		if (lb->getSelected() == 0) {
			if (the_image)
				driver->removeTexture(the_image);
			the_image = NULL;
			return true;
		}

		int count = 0;
		Media *media = &state->project->media;
		std::map<std::string, Media::Image*>& images = media->getList();
		for (std::map<std::string, Media::Image*>::const_iterator it = images.begin();
				it != images.end();
				++it) {
			if (count == lb->getSelected()-1) {
				if (the_image)
					driver->removeTexture(the_image);
				the_image = state->device->getVideoDriver()->addTexture("tmpicon.png", it->second->get());
				break;
			}
			count++;
		}
		return true;
	} else if (event.GUIEvent.EventType == EGET_ELEMENT_CLOSED && event.GUIEvent.Caller == win) {
		if (canClose())
			close();
		return true;
	}
	return false;
}
