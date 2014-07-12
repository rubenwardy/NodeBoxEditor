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

void drawImageAt(IVideoDriver* driver, ITexture* texture, int x, int y)
{	
	driver->draw2DImage(texture, rect<s32>(x, y, x + 64, y + 64),
			rect<s32>(0, 0, texture->getSize().Width, texture->getSize().Height));
}

void TextureEditor::draw(irr::video::IVideoDriver* driver)
{
	if (!state || !state->project || state->settings->getBool("hide_sidebar"))
		return;

	Node *node = state->project->GetCurrentNode();

	if (!node)
		return;

	unsigned int start_x = driver->getScreenSize().Width - 256;
	drawImageAt(driver, driver->addTexture("media/texture_box.png", node->getTexture(ECS_TOP)->get()), start_x + 50, 100);
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
			getVideoDriver()->getTexture("media/icon_mode_node.png");
	return icon;
}

