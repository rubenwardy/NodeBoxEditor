#ifndef TEXTUREDIALOG_HPP_INCLUDED
#define TEXTUREDIALOG_HPP_INCLUDED
#include "Dialog.hpp"

class TextureDialog : public Dialog
{
public:
	TextureDialog(EditorState *pstate, Node *pnode, CubeSide pface);
	static bool show(EditorState *pstate, Node *pnode, CubeSide pface);
	virtual bool canClose();
	virtual bool close();
	virtual bool OnEvent(const SEvent &event);
	virtual void draw(IVideoDriver *driver);
private:
	Node *node;
	CubeSide face;
	IGUIWindow *win;
	IGUIListBox *lb;
};

#endif

