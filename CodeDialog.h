#ifndef _CODEDIALOG_H_INCLUDED_
#define _CODEDIALOG_H_INCLUDED_
#include "common.h"

class CodeDialog :public IEventReceiver 
{
public:
	CodeDialog(IGUIEnvironment* guidevice, const wchar_t* input);
	virtual bool OnEvent(const SEvent& event);
private:
	IGUIEnvironment* guienv;
	IGUIWindow* win;
	IGUIEditBox* codebox;
	IGUIFileOpenDialog* save;	
};

#endif