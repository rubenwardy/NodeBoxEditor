#include "Configuration.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>

void Configuration::load(irr::core::stringc file){
	std::string line;
	std::ifstream myfile(file.c_str());
	if (myfile.is_open()){
		while (std::getline (myfile,line) )
		{
			doLine(irr::core::stringc(line.c_str()));
		}
		myfile.close();
	}else
		printf("Unable to open editor.conf\n"); 
	return;
}

void Configuration::doLine(irr::core::stringc line){
	irr::core::stringc l = line;

	// Remove comments
	irr::s32 first = l.findFirst(irr::c8('#'));
	if (first!=-1)
		l = l.subString(0,first);
	l = l.trim(" \t\n\r");

	// Check that string hash content
	if (l==""){
		return;
	}

	// Split string into hash and value per
	first = l.findFirst(irr::c8('='));
	irr::core::stringc hash = l.subString(0,first).trim();
	irr::core::stringc value = l.subString(first+1,l.size()).trim();

	#if IRRLICHT_VERSION_MAJOR == 1 && IRRLICHT_VERSION_MINOR < 8
		stringc lower = irr::core::stringc(value);
		lower.make_lower();		
	#else
		stringc lower = value.make_lower();
	#endif

	// convert boolean to lower case
	if (lower=="true"){
		value = "true";
	}else if (lower=="false"){
		value = "false";
	}

	// Create setting
	setStringSetting(hash,value);
}

void Configuration::save(irr::core::stringc file){
	std::ofstream myfile (file.c_str());
	if (myfile.is_open()){
		for (irr::core::list<setting*>::Iterator it=list->begin();it!=list->end();it++){
			printf("Looping...\n");
			setting* set = *it;
			printf("%s = ",set->hash.c_str());
			printf("%s\n",set->value.c_str());
			myfile << set->hash.c_str();
			myfile << " = ";
			myfile << set->value.c_str();
			myfile << "\n";
		}
		myfile.close();
	}else
		printf("Unable to write to file\n");
}

Configuration::setting* Configuration::getSetting(irr::core::stringc hash,bool warning_on_not_found) const{
	for (irr::core::list<setting*>::Iterator it=list->begin();it!=list->end();it++){
		setting* set = *it;
		if (set->hash == hash){
			return set;
		}
	}
	if (warning_on_not_found)
		printf("[Warning] Setting not found '%s'\n",hash.c_str());

	return NULL;
}

void Configuration::setStringSetting(irr::core::stringc hash,irr::core::stringc value){
	setting* set = getSetting(hash,false);

	if (set){
		set->value = value;
	}else{
		setting* newset = new setting();
		newset->hash = hash;
		newset->value = value;
		list->push_back(newset);
	}
}

irr::core::stringc* Configuration::getSettingAsString(irr::core::stringc hash) const{
	setting* set = getSetting(hash);

	if (set)
		return &set->value;

	return NULL;
}

bool Configuration::getSettingAsBool(irr::core::stringc hash) const{
	setting* set = getSetting(hash);

	if (set){
		if (set->value == "true" || set->value == "1"){
			return true;
		}else if (set->value == "false" || set->value == "0"){		
			return false;
		}
		printf("[Warning] Setting '%s' is not a boolean!\n",hash.c_str());
	}	

	return false;
}

int Configuration::getSettingAsInt(irr::core::stringc hash) const{
	setting* set = getSetting(hash);

	if (set){
		return atoi(set->value.c_str());
	}

	return false;
}
