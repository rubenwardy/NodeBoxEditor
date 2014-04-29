#ifndef NBEDITOR_H_INCLUDED
#define NBEDITOR_H_INCLUDED
#include "common.h"
#include "EditorState.h"

enum CDRType
{
	CDR_X_P = 1, 
	CDR_X_N,
	CDR_Y_P,
	CDR_Y_N,
	CDR_Z_P,
	CDR_Z_N,
	CDR_XZ,
	CDR_XY,
	CDR_ZY
};

class NBEditor;

class CDR
{
public:
	CDR() : visible(false) {}
	CDR(Viewport win, CDRType typ) : window(win), type(typ), visible(false) {}

	void update(NBEditor* editor, bool drag, rect<s32> offset);

	Viewport window;
	CDRType type;
	vector2d<irr::s32> position;
	bool visible;
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
	virtual void viewportTick(Viewport window, irr::video::IVideoDriver* driver, rect<s32> offset);
	virtual bool OnEvent(const irr::SEvent &event);
	virtual irr::video::ITexture* icon();
	void triggerCDRmoved() { prop_needs_update = true; }

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
