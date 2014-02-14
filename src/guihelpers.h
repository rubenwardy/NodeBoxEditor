#ifndef _GUIHELPERS_H_INCLUDED_
#define _GUIHELPERS_H_INCLUDED_
#include "common.h"

void fillTB(IGUIElement* sidebar,int parentId,int id,float value);
void fillTB(IGUIElement* sidebar,int parentId,int id,int value);
void addBox(IGUIElement* parent,IGUIEnvironment* guienv, vector2di pos,int index,const wchar_t* label);
void addXYZ(IGUIElement* parent,IGUIEnvironment* guienv, vector2di pos,int startIndex);

#endif