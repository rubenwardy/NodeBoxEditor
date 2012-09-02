#include <irrlicht.h>

//Convertors
const wchar_t* convert(const char* input); //Convertor: char to wchar_t
char* convert(wchar_t* input); //Convertor: wchar_t to char

//Other useful funcs
irr::core::vector3df* calcVectors(irr::io::IrrXMLReader* xml,const char* prefix);