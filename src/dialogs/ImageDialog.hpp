#ifndef IMAGEDIALOG_HPP_INCLUDED
#define IMAGEDIALOG_HPP_INCLUDED
#include "Dialog.hpp"

class ImageDialog : public Dialog
{
public:
	ImageDialog(EditorState *mstate, Node *tnode, ECUBE_SIDE tface);
	static bool show(EditorState *mstate, Node *node, ECUBE_SIDE face);
	void doSave(const SEvent &event);
	void doOpen(const SEvent &event);
	virtual bool canClose();
	virtual bool close();
	virtual bool OnEvent(const SEvent &event);
private:
	IGUIWindow *win;
	Node *node;
	ECUBE_SIDE face;
};

#endif
