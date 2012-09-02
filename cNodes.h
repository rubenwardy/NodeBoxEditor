#ifndef _cNodes_h_included_
#define _cNodes_h_included_

#include <irrlicht.h>
#include "define.h"

using namespace irr;
using namespace gui;
using namespace scene;
using namespace core;
using namespace video;

class cNode
{
public:
	bool load(wchar_t* input);
	bool save(wchar_t* output);
	bool switchFocus(ISceneNode* hit);
private:

};

#endif