#include "MediaManager.hpp"
#include "util/filesys.hpp"

Media::~Media()
{
	for (std::map<std::string, Media::Image*>::const_iterator it = images.begin();
			it != images.end();
			++it) {
		delete it->second;
	}
}

bool Media::import(std::string file, IVideoDriver* driver)
{		
	return add(file, driver->createImageFromFile(file.c_str()));
}

bool Media::add(std::string file, IImage *image)
{
	if (!image)
		return false;

	const char *shortname = trim(filenameWithExt(file)).c_str();
	if (images.find(shortname) != images.end()) {
		std::cerr << "Overwriting '" << shortname << "'" << std::endl;
		images[shortname]->update(image);
		images[shortname]->origpath = file;
	} else {
		std::cerr << "Adding '" << shortname << "'" << std::endl;
		images[shortname] = new Media::Image(shortname, image);
		images[shortname]->origpath = file;
	}
	return true;
}

Media::Image *Media::get(const char* name)
{
	if (!images[name]) {
		return NULL;
	}
	return images[name];
}

void Media::debug()
{
	std::cerr << "Media Manager:" << std::endl;
	for (std::map<std::string, Media::Image*>::const_iterator it = images.begin();
			it != images.end();
			++it) {
		std::cerr << it->second->name.c_str() << " (" << it->second->getHolders() << ")" << std::endl;
	}
}

void Media::clearGrabs()
{
	for (std::map<std::string, Media::Image*>::const_iterator it = images.begin();
			it != images.end();
			++it) {
		it->second->dropAll();
	}
}
