#include <irrlicht.h>

#ifndef _define_included_
#define _define_included_

	// Structures
	struct ed_data
	{
		irr::gui::IGUIStaticText* d_nb;
		irr::gui::IGUIStaticText* d_pos;
		irr::gui::IGUIStaticText* d_rot;
	};

	struct sBox
	{
		irr::core::vector3df size;
		irr::scene::IMeshSceneNode* model;
	};

	// Functions

	//Convertors
	const wchar_t* convert(const char* input); //Convertor: char to wchar_t
	char* convert(wchar_t* input); //Convertor: wchar_t to char

	//Other useful funcs
	irr::core::vector3df* calcVectors(irr::io::IrrXMLReader* xml,const char* prefix);
	void resizeObject(sBox* input,irr::f32 px,irr::f32 py,irr::f32 pz);
	void setsizeObject(sBox* input,irr::f32 px,irr::f32 py,irr::f32 pz);
	void checkScaling(sBox* input);

#define NODE_THIN 0.1

#endif