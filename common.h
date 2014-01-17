#ifndef _COMMON_H_INCLUDED_
#define _COMMON_H_INCLUDED_
#include <irrlicht.h>
#include <iostream>
using namespace irr;
using namespace core;
using namespace scene;
using namespace gui;
using namespace video;

enum VIEWPORT
{
	EVIEW_PERS = 0,
	EVIEW_XZ = 1,
	EVIEW_XY = 2,
	EVIEW_ZY = 3
};

enum CDR_TYPE 
{
	CDR_X_P = 1, 
	CDR_X_N = 2,
	CDR_Y_P = 3,
	CDR_Y_N = 4,
	CDR_Z_P = 5,
	CDR_Z_N = 6,
	CDR_XZ = 7,
	CDR_XY = 8,
	CDR_ZY = 9
};

// Enums
enum BUILD_TYPE
{
	NBT_NB = 1, // Just the node box table
	NBT_NBS = 2, // The node box table with draw type and parent holder (nodebox={})
	NBT_FULL = 3 // Whole node definition
};


enum FileParserType
{
	EFPT_SAVE_PROJ = 1,
	EFPT_LOAD_PROJ = 2,
	EFPT_EXPORT = 3,
	EFPT_IMPORT = 4
};

// Defines
#define EDITOR_TEXT_VERSION "0.6.1 - Stone"
#define EDITOR_VERSION 1
#define EDITOR_PARSER 1
#define NODE_RES 16 // The resolution of the snapping (16)
#define NODE_THIN 1/NODE_RES // The smallest a box can be (1/NODE_RES)
#define NODEB_MAX 50 // Maximum amount of nodeboxes (50)
#define NODEB_MENU_START 250


const wchar_t* convert(const char* input);
#endif