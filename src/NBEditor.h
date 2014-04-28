#ifndef _NBEDITOR_H_INCLUDED_
#define _NBEDITOR_H_INCLUDED_
#include "common.h"
#include "EditorState.h"

class NBEditor;
class CDR
{
public:
	CDR():visible(false){}
	CDR(VIEWPORT win,CDR_TYPE typ):window(win),type(typ),visible(false){}
	VIEWPORT window;
	CDR_TYPE type;
	vector2d<irr::s32> position;
	bool visible;
	void update(NBEditor* editor,bool drag,rect<s32> offset);
};

class EditorMode;
class NBEditor :public EditorMode
{
public:
	NBEditor(EditorState* st);

	// Interface
	virtual void load();
	virtual void unload();
	virtual void update(double dtime);
	virtual void draw(irr::video::IVideoDriver* driver);
	virtual void viewportTick(VIEWPORT window,irr::video::IVideoDriver* driver,rect<s32> offset);
	virtual bool OnEvent(const irr::SEvent &event);
	virtual irr::video::ITexture* icon(){
		static irr::video::ITexture* icon = GetState()->GetDevice()->
			getVideoDriver()->getTexture("media/icon_mode_nodebox.png");
		return icon;
	}
	void triggerCDRmoved(){prop_needs_update = true;}	
private:
	bool wasmd;
	int current;
	CDR cdrs[15];
	void load_ui();
	void fillProperties();
	void updateProperties();
	bool prop_needs_update;
};

#endif
