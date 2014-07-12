#include <list>
#include "TextureEditor.hpp"
#include "util/string.hpp"

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

void drawIconAt(const wchar_t* label, int x, int y, ITexture *texture, IVideoDriver *driver, IGUIFont *font)
{	
	driver->draw2DImage(texture, rect<s32>(x, y, x + 64, y + 64),
			rect<s32>(0, 0, texture->getSize().Width, texture->getSize().Height));
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
	drawIconAt(L"Left", start_x + 96, 70, driver->addTexture("media/texture_box.png",
			node->getTexture(ECS_LEFT)->get()), driver, state->device->getGUIEnvironment()->getSkin()->getFont());
	drawIconAt(L"Top", start_x + 16, 170, driver->addTexture("media/texture_box.png",
			node->getTexture(ECS_TOP)->get()), driver, state->device->getGUIEnvironment()->getSkin()->getFont());
	drawIconAt(L"Front", start_x + 96, 170, driver->addTexture("media/texture_box.png",
			node->getTexture(ECS_FRONT)->get()), driver, state->device->getGUIEnvironment()->getSkin()->getFont());
	drawIconAt(L"Bottom", start_x + 180, 170, driver->addTexture("media/texture_box.png",
			node->getTexture(ECS_BOTTOM)->get()), driver, state->device->getGUIEnvironment()->getSkin()->getFont());
	drawIconAt(L"Right", start_x + 96, 270, driver->addTexture("media/texture_box.png",
			node->getTexture(ECS_RIGHT)->get()), driver, state->device->getGUIEnvironment()->getSkin()->getFont());
	drawIconAt(L"Back", start_x + 96, 370, driver->addTexture("media/texture_box.png",
			node->getTexture(ECS_BACK)->get()), driver, state->device->getGUIEnvironment()->getSkin()->getFont());
}


void TextureEditor::viewportTick(Viewport window, irr::video::IVideoDriver* driver, rect<s32> offset)
{}


bool TextureEditor::OnEvent(const irr::SEvent &event)
{
	return false;
}

irr::video::ITexture* TextureEditor::icon()
{
	static irr::video::ITexture* icon = state->device->
			getVideoDriver()->getTexture("media/icon_mode_texture.png");
	return icon;
}

