#ifndef IMAGEDIALOG_HPP_INCLUDED
#define IMAGEDIALOG_HPP_INCLUDED
#include "Dialog.hpp"

class ImageDialog : public Dialog
{
public:
	ImageDialog(EditorState *mstate, Node *tnode, CubeSide tface);
	static bool show(EditorState *mstate, Node *node, CubeSide face);
	void doSave(const SEvent &event);
	void doOpen(const SEvent &event);
	virtual bool canClose();
	virtual bool close();
	virtual bool OnEvent(const SEvent &event);
private:
	FileParserType parser_type;
	IGUIWindow *win;
	Node *node;
	CubeSide face;
};

#endif

