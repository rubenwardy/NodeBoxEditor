#include "define.h"
#include <iostream>

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
