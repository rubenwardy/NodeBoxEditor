#ifndef DIALOG_HPP_INCLUDED
#define DIALOG_HPP_INCLUDED
#include "common.hpp"
#include "EditorState.hpp"

class EditorState;
class Dialog : public IEventReceiver
{
public:
	Dialog(EditorState *mstate):
		state(mstate)
	{}

	virtual bool canClose() = 0;
	virtual bool close() = 0;
	virtual bool OnEvent(const SEvent &event) = 0;
	virtual void draw(IVideoDriver *driver) {}
protected:
	EditorState *state;
};

#endif

