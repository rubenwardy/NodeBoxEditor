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

void checkScaling(irr::scene::ISceneNode* input){
irr::core::aabbox3d<f32> box = input->getTransformedBoundingBox();
irr::core::vector3df extent = box.getExtent();
 
float sx=extent.X;
float sy=extent.Y;
float sz=extent.Z;
bool tmp_change=false;

if (extent.X>1){
	std::cout << "--auto correct: x" << std::endl;
	sx=1/extent.X;
	tmp_change=true;
}

if (extent.Y>1){
	std::cout << "--auto correct: y" << std::endl;
	sy=1/extent.Y;
	tmp_change=true;
}

if (extent.Z>1){
	std::cout << "--auto correct: z" << std::endl;
	sz=1/extent.Z;
	tmp_change=true;
}

if (tmp_change==true){
	std::cout << "--changing" << std::endl;
	core::vector3df scale(sx, sy, sz);
	input->setScale(scale);
}
}

void resizeObject(irr::scene::ISceneNode* input,float px,float py,float pz){

std::cout << std::endl << "-----------------------" << std::endl << "Performing resize." << std::endl;

std::cout << std::endl << "Increase by: " << std::endl << px << " - " << py << " - " << pz << std::endl << std::endl;

irr::core::aabbox3d<f32> box = input->getTransformedBoundingBox();
irr::core::vector3df extent = box.getExtent();
 
float sx = px+extent.X;
float sy = py+extent.Y;
float sz = pz+extent.Z;

setsizeObject(input,sx,sy,sz);
}

void setsizeObject(irr::scene::ISceneNode* input,float px,float py,float pz){

std::cout << std::endl << "-----------------------" << std::endl << "Performing resize." << std::endl;

irr::core::aabbox3d<f32> box = input->getTransformedBoundingBox();
irr::core::vector3df extent = box.getExtent();
 
float sx = px/extent.X;
float sy = py/extent.Y;
float sz = pz/extent.Z;

std::cout << std::endl << "Before: " << std::endl << extent.X << " - " << extent.Y << " - " << extent.Z << std::endl;

std::cout << std::endl << "After: " << std::endl << px << " - " << py << " - " << pz << std::endl;

std::cout << std::endl << "Scaled by: " << std::endl << sx << " - " << sy << " - " << sz << std::endl;

if (sx<=0 || sy<=0 || sz<=0){
	std::cout << "--cancelled, scale <= 0" << std::endl;
	return;
}

if (px<=0 || py<=0 || pz<=0){
	std::cout << "--cancelled, set <= 0" << std::endl;
	return;
}

core::vector3df scale(sx, sy, sz);
input->setScale(scale);

checkScaling(input);

}