#ifndef GUIHELPERS_HPP_INCLUDED
#define GUIHELPERS_HPP_INCLUDED

#include "common.hpp"

extern void fillTB(IGUIElement* sidebar, int parentId, int id, int value);
extern void addBox(IGUIElement* parent, IGUIEnvironment* guienv, vector2di pos, int index, const wchar_t* label);
extern void addXYZ(IGUIElement* parent, IGUIEnvironment* guienv, vector2di pos, int startIndex);

#endif
