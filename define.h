#include <irrlicht.h>

#ifndef _define_included_
#define _define_included_

	// Enums
	enum CDR_TYPE 
	{
		CDR_X_P = 1, 
		CDR_X_N = 2,
		CDR_Y_P = 3,
		CDR_Y_N = 4,
		CDR_Z_P = 5,
		CDR_Z_N = 6
	};
	// CDR_TYPE::CDR_<axis>_<positive/negitive-direction>

	enum BUILD_TYPE
	{
		NBT_NB = 1, // Just the node box table
		NBT_NBS = 2, // The node box table with draw type and parent holder (nodebox={})
		NBT_FULL = 3 // Whole node definition
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
	};

	// Defines
	#define NODE_THIN 0.05
	#define NODEB_MAX 50

	// Structures
	struct ed_data
	{
		irr::gui::IGUIStaticText* d_nb;
		irr::gui::IGUIStaticText* d_pos;
		irr::gui::IGUIStaticText* d_rot;
	};

	struct CDR
	{
		CDR_TYPE type;
		irr::gui::IGUIImage* image;
	};

	class sBox
	{
	public:
		~sBox(){
			if (model)
				model->remove();
		}
		irr::core::vector3df position;
		irr::core::vector3df size;
		irr::scene::IMeshSceneNode* model;
	};

	// Functions: Convertors
	const wchar_t* convert(const char* input); //Convertor: char to wchar_t
	char* convert(wchar_t* input); //Convertor: wchar_t to char

	// Functions: Misc
	irr::core::vector3df* calcVectors(irr::io::IrrXMLReader* xml,const char* prefix);
#endif