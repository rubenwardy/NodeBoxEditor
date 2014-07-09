#ifndef NBEDITOR_HPP_INCLUDED
#define NBEDITOR_HPP_INCLUDED
#include "common.hpp"
#include "EditorState.hpp"
enum CDR_TYPE
{
	CDR_L,
	CDR_R,
	CDR_U,
	CDR_D,
	CDR_M
};
enum CDRType
{
	CDR_NONE,
	CDR_X_P, 
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
	CDR(Viewport win, CDR_TYPE typ) : window(win), type(typ), visible(false) {}

	void update(NBEditor* editor, bool drag, rect<s32> offset);
	CDRType getActualType(EditorState* state);

	Viewport window;
	CDR_TYPE type;
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
	CDR cdrs[20];
	void load_ui();
	void fillProperties();
	void updateProperties();
	bool prop_needs_update;
};

#endif

