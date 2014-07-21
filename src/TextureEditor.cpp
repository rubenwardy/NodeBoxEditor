#include <list>
#include "TextureEditor.hpp"
#include "util/string.hpp"
#include "TextureDialog.hpp"

TextureEditor::TextureEditor(EditorState* st) :
	EditorMode(st)
{
}

void TextureEditor::load()
{
	IGUIStaticText* sidebar = state->menu->sidebar;

	if (!sidebar)
		return;

	sidebar->setText(L"Texture Tool");
}

void TextureEditor::unload()
{}


void TextureEditor::update(double dtime)
{}

void drawIconAt(const wchar_t* label, int x, int y, Media::Image *image, IVideoDriver *driver, IGUIFont *font)
{
	if (!image || image->name == "default") {
		driver->draw2DRectangle(SColor(100, 0, 0, 0), rect<s32>(x, y, x + 64, y + 64));
	} else {
		ITexture *texture = driver->addTexture("tmpicon.png", image->get());
		driver->draw2DImage(texture, rect<s32>(x, y, x + 64, y + 64),
				rect<s32>(0, 0, texture->getSize().Width, texture->getSize().Height));
	}
	font->draw(label, rect<s32>(x + 10, y + 68, 64, 25), SColor(255, 255, 255, 255));
}

void TextureEditor::draw(irr::video::IVideoDriver* driver)
{
	if (!state || !state->project || state->settings->getBool("hide_sidebar"))
		return;

	Node *node = state->project->GetCurrentNode();

	if (!node)
		return;

	unsigned int start_x = driver->getScreenSize().Width - 256;
	drawIconAt(L"Left", start_x + 96, 70, node->getTexture(ECS_LEFT),
			driver, state->device->getGUIEnvironment()->getSkin()->getFont());
	drawIconAt(L"Top", start_x + 16, 170, node->getTexture(ECS_TOP),
			driver, state->device->getGUIEnvironment()->getSkin()->getFont());
	drawIconAt(L"Front", start_x + 96, 170, node->getTexture(ECS_FRONT),
			driver, state->device->getGUIEnvironment()->getSkin()->getFont());
	drawIconAt(L"Bottom", start_x + 180, 170, node->getTexture(ECS_BOTTOM),
			driver, state->device->getGUIEnvironment()->getSkin()->getFont());
	drawIconAt(L"Right", start_x + 96, 270, node->getTexture(ECS_RIGHT),
			driver, state->device->getGUIEnvironment()->getSkin()->getFont());
	drawIconAt(L"Back", start_x + 96, 370, node->getTexture(ECS_BACK),
			driver, state->device->getGUIEnvironment()->getSkin()->getFont());
}


void TextureEditor::viewportTick(Viewport window, irr::video::IVideoDriver* driver, rect<s32> offset)
{}

bool iconClicked(int x, int y, vector2di mouse)
{
	return (rect<s32>(x, y, x + 64, y + 64).isPointInside(mouse));
}


bool TextureEditor::OnEvent(const irr::SEvent &event)
{
	if (!state || !state->project || state->settings->getBool("hide_sidebar"))
		return false;

	Node *node = state->project->GetCurrentNode();

	if (!node)
		return false;

	if (event.EventType == EET_MOUSE_INPUT_EVENT &&
			event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP) {
		unsigned int start_x = state->device->getVideoDriver()->getScreenSize().Width - 256;

		if (iconClicked(start_x + 96, 70, state->mouse_position)) {
			TextureDialog::show(state, node, ECS_LEFT);
		} else if (iconClicked(start_x + 16, 170, state->mouse_position)) {
			TextureDialog::show(state, node, ECS_TOP);
		} else if (iconClicked(start_x + 96, 170, state->mouse_position)) {
			TextureDialog::show(state, node, ECS_FRONT);
		} else if (iconClicked(start_x + 180, 170, state->mouse_position)) {
			TextureDialog::show(state, node, ECS_BOTTOM);
		} else if (iconClicked(start_x + 96, 270, state->mouse_position)) {
			TextureDialog::show(state, node, ECS_RIGHT);
		} else if (iconClicked(start_x + 96, 370, state->mouse_position)) {
			TextureDialog::show(state, node, ECS_BACK);
		}
	}
	
	return false;
}

irr::video::ITexture* TextureEditor::icon()
{
	static irr::video::ITexture* icon = state->device->
			getVideoDriver()->getTexture("media/icon_mode_texture.png");
	return icon;
}

