#ifndef _COMMON_H_INCLUDED_
#define _COMMON_H_INCLUDED_
#include <irrlicht.h>
#include <iostream>
using namespace irr;
using namespace core;
using namespace scene;
using namespace gui;
using namespace video;

	// Enums
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
	// CDR_TYPE::CDR_<axis>_<positive/negitive-direction>

	enum BUILD_TYPE
	{
		NBT_NB = 1, // Just the node box table
		NBT_NBS = 2, // The node box table with draw type and parent holder (nodebox={})
		NBT_FULL = 3 // Whole node definition
	};

	enum TOOL_TYPE
	{
		TOOL_NEWNODE = 1,
		TOOL_NODE = 2,
		TOOL_NODEB = 3,
		TOOL_TEXT = 4
	};

	enum GUI_ID
	{
		// File
		GUI_ID_NEW=201,
		GUI_ID_LOAD=202,
		GUI_ID_SAVE=203,
		GUI_ID_IMPORT=204,
		GUI_ID_EX_PROJ=205,
		GUI_ID_EX_NODE=206,
		GUI_ID_EX_NBS=207,
		GUI_ID_EX_NB=208,
		GUI_ID_EXIT=230,

		// Project
		GUI_ID_ADDNODE=209,
		GUI_ID_SWITCH=210,
		GUI_ID_DELETENODE = 211,

		// Node
		GUI_ID_BOX=212,
		GUI_ID_DELETENB = 213,
		GUI_ID_TEXT = 214,

		// Help
		GUI_ID_HELP=215,
		GUI_ID_ABOUT=216,

		// View
		GUI_ID_SP_ALL=217,
		GUI_ID_SP_PER=218,
		GUI_ID_SP_TOP=219,
		GUI_ID_SP_FRT=220,
		GUI_ID_SP_RHT=221,

		// Edit
		GUI_ID_SNAP = 222,
		GUI_ID_LIMIT = 223,
		GUI_ID_TOOL_NNODE = 224,
		GUI_ID_TOOL_NODE = 225,
		GUI_ID_TOOL_NODEB = 226,
		GUI_ID_TOOL_TEXT = 227,

		// Windows
		GUI_ID_SUBMIT = 228,
		GUI_ID_BOX_NAME = 229
	};

	// Defines
	#define EDITOR_TEXT_VERSION "0.5.0 - Dirt"
	#define EDITOR_VERSION 1
	#define EDITOR_PARSER 1
	#define NODE_RES 16 // The resolution of the snapping (16)
	#define NODE_THIN 1/NODE_RES // The smallest a box can be (1/NODE_RES)
	#define NODEB_MAX 10 // Maximum amount of nodeboxes (50)

	// Structures
	struct ed_data
	{
		IGUIStaticText* d_nb;
		IGUIStaticText* d_pos;
		IGUIStaticText* d_rot;
		IGUIContextMenu* menu;

		bool snapping;
		bool limiting;
		TOOL_TYPE type;
	};

	struct CDR
	{
		CDR_TYPE type;
		irr::gui::IGUIImage* image;
		int camera;
	};

#include "NodeBox.h"

	// Functions: Convertors
	const wchar_t* convert(const char* input); //Convertor: char to wchar_t
	char* convert(wchar_t* input); //Convertor: wchar_t to char
#endif