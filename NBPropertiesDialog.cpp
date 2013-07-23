#include "NBPropertiesDialog.h"
#include <wchar.h>

NBPropertiesDialog::NBPropertiesDialog(IGUIEnvironment* guidevice, NodeBox* box, Node* node)
:guienv(guidevice),nodebox(box),_node(node)
{
	if (!guienv || !nodebox || !_node){
		win = 0;
		return;
	}

	win = guienv->addWindow(rect<irr::s32>(50,50,50+320,50+162),false,L"Properties");
	guienv->addEditBox(box->name.c_str(),rect<irr::s32>(32,32,290,52),true,win,GUI_ID_BOX_NAME);

	// Add the co-ordinate boxes
	{
		guienv->addEditBox(convert(stringc(nodebox->one.X).c_str()),rect<irr::s32>(32,60,112,80),true,win,230);
		guienv->addEditBox(convert(stringc(nodebox->one.Y).c_str()),rect<irr::s32>(32+90,60,112+90,80),true,win,231);
		guienv->addEditBox(convert(stringc(nodebox->one.Z).c_str()),rect<irr::s32>(32+180,60,112+180,80),true,win,232);

		guienv->addEditBox(convert(stringc(nodebox->two.X).c_str()),rect<irr::s32>(32,60+30,112,80+30),true,win,233);
		guienv->addEditBox(convert(stringc(nodebox->two.Y).c_str()),rect<irr::s32>(32+90,60+30,112+90,80+30),true,win,234);
		guienv->addEditBox(convert(stringc(nodebox->two.Z).c_str()),rect<irr::s32>(32+180,60+30,112+180,80+30),true,win,235);
	}

	guienv->addButton(rect<irr::s32>(100,120,220,152),win,GUI_ID_SUBMIT,L"Save");
}

bool NBPropertiesDialog::OnEvent(const SEvent& event)
{
	if (!win)
		return false;

	irr::s32 id = event.GUIEvent.Caller->getID();
	switch(event.GUIEvent.EventType)
	{
	case EGET_BUTTON_CLICKED:
		switch(id)
		{
		case 0:						
			break;
		case GUI_ID_SUBMIT:
			save(event.GUIEvent.Caller);
			break;
		}		
		break;
	case EGDT_WINDOW_CLOSE:
		if (event.GUIEvent.Caller == win){
			win = NULL;
		}
		break;
	}
	return false;
	
}

void NBPropertiesDialog::save(IGUIElement* button){
	if (win && win->getElementFromId(GUI_ID_SUBMIT)==button){
		printf("Submitting Properties \n");

		// Set name
		nodebox->name = win->getElementFromId(GUI_ID_BOX_NAME)->getText();

		// Set one
		nodebox->one.X = wcstod(win->getElementFromId(230)->getText(),NULL);
		nodebox->one.Y = wcstod(win->getElementFromId(231)->getText(),NULL);
		nodebox->one.Z = wcstod(win->getElementFromId(232)->getText(),NULL);

		// Set two
		nodebox->two.X = wcstod(win->getElementFromId(233)->getText(),NULL);
		nodebox->two.Y = wcstod(win->getElementFromId(234)->getText(),NULL);
		nodebox->two.Z = wcstod(win->getElementFromId(235)->getText(),NULL);

		// Recreate node mesh
		_node->remesh();
		_node->defrag();
	}
}