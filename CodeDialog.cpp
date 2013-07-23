#include "CodeDialog.h"

CodeDialog::CodeDialog(IGUIEnvironment* guidevice, const wchar_t* input)
	:save(NULL),guienv(guidevice)
{
	win = guidevice->addWindow(rect<irr::s32>(50,50,50+320,50+360),false,L"Raw Code");
	codebox = guidevice->addEditBox(input,rect<irr::s32>(10,30,310,310),true,win);
	codebox->setMultiLine(true);
	codebox->setTextAlignment(EGUIA_UPPERLEFT,EGUIA_UPPERLEFT);
	codebox->setToolTipText(L"Ctrl+A, Ctrl+C to copy");

	guidevice->addButton(rect<irr::s32>(110,320,210,350),win,GUI_ID_SUBMIT,L"Save",L"Save the code to a file");
}

bool CodeDialog::OnEvent(const SEvent& event)
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
			if (win && win->getElementFromId(GUI_ID_SUBMIT)==event.GUIEvent.Caller){
				guienv->addMessageBox(L"Feature not implemented",L"Save feature not yet implemented");
			}
			break;
		}		
		break;
	case EGET_FILE_SELECTED:
		//event.GUIEvent.Caller->gett
	case EGET_FILE_CHOOSE_DIALOG_CANCELLED:
		if (event.GUIEvent.Caller == save){
			save = NULL;
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