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
    #define MAXSIZE 1024
	size_t origsize = strlen(input) + 1;
    static wchar_t wcstring[MAXSIZE];
    mbstowcs(wcstring, input, origsize);
    wcscat(wcstring, L"");
	return wcstring;
	#undef MAXSIZE
}

char* convert(wchar_t* input){
	char* tmp=new char();
	wcstombs(tmp,input,0);
	return tmp;
}

void checkScaling(sBox* input){
	//irr::core::aabbox3d<f32> box = input->getTransformedBoundingBox();
	irr::core::vector3df extent = input->size; //box.getExtent();
 
	f32 sx=extent.X;
	f32 sy=extent.Y;
	f32 sz=extent.Z;
	bool tmp_change=false;

	if (extent.X>1){
		std::cout << "--auto correct: x" << std::endl;
		input->size.X = 1;
		tmp_change=true;
	}else if(extent.X<0.1){
		std::cout << "--auto correct: x" << std::endl;
		input->size.X = 0.1;
		tmp_change=true;
	}

	if (extent.Y>1){
		std::cout << "--auto correct: y" << std::endl;
		input->size.Y = 1;
		tmp_change=true;
	}else if(extent.Y<0.1){
		std::cout << "--auto correct: y" << std::endl;
		input->size.Y = 0.1;
		tmp_change=true;
	}

	if (extent.Z>1){
		std::cout << "--auto correct: z" << std::endl;
		input->size.Z = 1;
		tmp_change=true;
	}else if(extent.Z<0.1){
		std::cout << "--auto correct: z" << std::endl;
		input->size.Z = 0.1;
		tmp_change=true;
	}

	//if (tmp_change==true){
		setsizeObject(input,input->size.X,input->size.Y,input->size.Z);
	//}
}

void resizeObject(sBox* input,f32 px,f32 py,f32 pz){
	std::cout << std::endl << "-----------------------" << std::endl << "Performing resize." << std::endl;

	std::cout << std::endl << "Increase by: " << std::endl << px << " - " << py << " - " << pz << std::endl << std::endl;

	//irr::core::aabbox3d<f32> box = input->getTransformedBoundingBox();
	irr::core::vector3df extent = input->size; //box.getExtent();
 
	f32 sx = px+extent.X;
	f32 sy = py+extent.Y;
	f32 sz = pz+extent.Z;

	setsizeObject(input,sx,sy,sz);
}

void setsizeObject(sBox* input,f32 px,f32 py,f32 pz){

	std::cout << std::endl << "-----------------------" << std::endl << "Performing resize." << std::endl;

	//irr::core::aabbox3d<f32> box = input->getTransformedBoundingBox();
	irr::core::vector3df extent = input->size; //box.getExtent();

	input->size=vector3df(px,py,pz);
 
	float sx = px/extent.X;
	float sy = py/extent.Y;
	float sz = pz/extent.Z;

	std::cout << std::endl << "Before: " << std::endl << extent.X << " - " << extent.Y << " - " << extent.Z << std::endl;

	std::cout << std::endl << "After: " << std::endl << px << " - " << py << " - " << pz << std::endl;

	std::cout << std::endl << "Scaled by: " << std::endl << sx << " - " << sy << " - " << sz << std::endl;

	core::vector3df scale(sx, sy, sz);
	input->model->setScale(scale);
}