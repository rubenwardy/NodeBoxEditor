#include "define.h"
#include <iostream>

using namespace irr;
using namespace gui;
using namespace scene;
using namespace core;
using namespace video;

irr::core::vector3df* calcVectors(irr::io::IrrXMLReader* xml,const char* prefix){
	std::cout << "calcVectors::Start" << std::endl;
	char px[3];
	char py[3]; 
	char pz[3];

	std::cout << "calcVectors::setting" << std::endl;
	strcpy(px,prefix); // copy string one into the result.
	strcpy(py,prefix);
	strcpy(pz,prefix);

	std::cout << "calcVectors::appending" << std::endl;
	strcat(px,"x"); // append string two to the result.
	strcat(py,"y");
	strcat(pz,"z");

	std::cout << "calcVectors::declaring v3df" << std::endl;
	irr::core::vector3df* tmp;

	std::cout << "calcVectors::reading xml" << std::endl;
	tmp=new irr::core::vector3df(xml->getAttributeValueAsInt(px),xml->getAttributeValueAsInt(py),xml->getAttributeValueAsInt(pz));

	std::cout << "calcVectors::End" << std::endl;
	return tmp;
}

const wchar_t* convert(const char* input){
	char* orig=(char*)input;
	size_t origsize = strlen(orig) + 1;
    const size_t newsize = 100;
    size_t convertedChars = 0;
    wchar_t wcstring[newsize];
    mbstowcs_s(&convertedChars, wcstring, origsize, orig, _TRUNCATE);
    wcscat_s(wcstring, L"");
	//outp=wcstring;
	return wcstring;
}

char* convert(wchar_t* input){
	char* tmp=new char();
	wcstombs(tmp,input,0);
	return tmp;
}

void resizeObject(irr::scene::ISceneNode* input,float px,float py,float pz){

std::cout << std::endl << "-----------------------" << std::endl << "Performing resize." << std::endl;

std::cout << std::endl << "Increase by: " << std::endl << px << " - " << py << " - " << pz << std::endl << std::endl;

irr::core::aabbox3d<f32> box = input->getTransformedBoundingBox();
irr::core::vector3df extent = box.getExtent();
 
float sx = (px+extent.X)/extent.X;
float sy = (py+extent.Y)/extent.Y;
float sz = (pz+extent.Z)/extent.Z;

std::cout << std::endl << "Before: " << std::endl << extent.X << " - " << extent.Y << " - " << extent.Z << std::endl;

std::cout << std::endl << "After: " << std::endl << (px+extent.X) << " - " << (py+extent.Y) << " - " << (pz+extent.Z) << std::endl;

std::cout << std::endl << "Scaled by: " << std::endl << sx << " - " << sy << " - " << sz << std::endl;

if (sx<=0 || sy<=0 || sz<=0){
	std::cout << "--cancelled" << std::endl;
	return;
}

if (extent.X>2){
	std::cout << "--auto correct: x" << std::endl;
	sx=2/extent.X;
}

if (extent.Y>2){
	std::cout << "--auto correct: y" << std::endl;
	sy=2/extent.Y;
}

if (extent.Z>2){
	std::cout << "--auto correct: z" << std::endl;
	sz=2/extent.Z;
}

core::vector3df scale(sx, sy, sz);
input->setScale(scale);

}