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

	// Defines
	#define NODE_THIN 0.01
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

	struct sBox
	{
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