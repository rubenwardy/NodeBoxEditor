#include "GUIHelpers.hpp"

void fillTB(IGUIElement* sidebar, int parentId, int id, int value){
	IGUIElement* element = sidebar->getElementFromId(parentId)->getElementFromId(id);

	if (element) {
		IGUIEditBox* editbox = static_cast<IGUIEditBox*>(element);

		if (!editbox) {
			return;
		}

		editbox->setText(stringw(value).c_str());
	}
}


void addBox(IGUIElement* parent, IGUIEnvironment* guienv, vector2di pos, int index, const wchar_t* label){
	guienv->addStaticText(label,
			rect<s32>(pos.X, pos.Y, pos.X + 20, pos.Y + 20),
			false, true, parent)->setNotClipped(true);
	guienv->addEditBox(L"",
			rect<s32>(pos.X + 15, pos.Y, pos.X + 200, pos.Y + 20),
			true, parent, index)->setNotClipped(true);
}


void addXYZ(IGUIElement* parent,IGUIEnvironment* guienv, vector2di pos, int startIndex){
	addBox(parent, guienv, vector2di(pos.X, pos.Y),      startIndex,     L"X");  // 0,0
	addBox(parent, guienv, vector2di(pos.X, pos.Y + 30), startIndex + 1, L"Y");  // 80, 0
	addBox(parent, guienv, vector2di(pos.X, pos.Y + 60), startIndex + 2, L"Z");  // 160, 0
}

