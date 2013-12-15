#ifndef _CONFIGURATION_H_INCLUDED_
#define _CONFIGURATION_H_INCLUDED_
#include "common.h"

class Configuration
{
public:
	struct setting{
		irr::core::stringc hash;
		irr::core::stringc value;		
	};

	Configuration(){list=new irr::core::list<setting*>();}
	void load(irr::core::stringc file);
	void save(irr::core::stringc file);	

	// Get Functions
	setting* getSetting(irr::core::stringc hash,bool warning_on_not_found = true) const;
	irr::core::stringc* getSettingAsString(irr::core::stringc hash) const;
	bool getSettingAsBool(irr::core::stringc hash) const;
	int getSettingAsInt(irr::core::stringc hash) const;

	// Set functions
	void setStringSetting(irr::core::stringc hash,irr::core::stringc value);
private:
	irr::core::list<setting*>* list;
	void doLine(irr::core::stringc line);
};

#endif