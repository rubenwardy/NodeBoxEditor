#include <irrlicht.h>

#ifndef _define_included_
#define _define_included_
//Classes
class ed_data
{
public:
	irr::gui::IGUIStaticText* d_nb;
	irr::gui::IGUIStaticText* d_pos;
	irr::gui::IGUIStaticText* d_rot;
};

//Convertors
const wchar_t* convert(const char* input); //Convertor: char to wchar_t
char* convert(wchar_t* input); //Convertor: wchar_t to char

//Other useful funcs
irr::core::vector3df* calcVectors(irr::io::IrrXMLReader* xml,const char* prefix);
void resizeObject(irr::scene::ISceneNode* input,float px,float py,float pz);

#endif