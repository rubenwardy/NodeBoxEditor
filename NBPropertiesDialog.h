#ifndef _NBPropertiesDialog_H_INCLUDED_
#define _NBPropertiesDialog_H_INCLUDED_
#include "common.h"
#include "Node.h"

class NBPropertiesDialog :public IEventReceiver 
{
public:
	NBPropertiesDialog(IGUIEnvironment* guidevice, NodeBox* box, Node* node);
	virtual bool OnEvent(const SEvent& event);
	void save(IGUIElement* button);
private:
	IGUIEnvironment* guienv;
	IGUIWindow* win;
	Node* _node;
	NodeBox* nodebox;	
};

#endif