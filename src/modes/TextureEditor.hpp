#ifndef TEXTUREEDITOR_HPP_INCLUDED
#define TEXTUREEDITOR_HPP_INCLUDED
#include "../common.hpp"
#include "../EditorState.hpp"

class EditorMode;
class TextureEditor :public EditorMode
{
public:
	TextureEditor(EditorState* st);

	// Interface
	virtual void load();
	virtual void unload();
	virtual void update(double dtime);
	virtual void draw(irr::video::IVideoDriver* driver);
	virtual void viewportTick(EViewport window, irr::video::IVideoDriver* driver, rect<s32> offset);
	virtual bool OnEvent(const irr::SEvent &event);
	virtual irr::video::ITexture* icon();
};

#endif
