#include "MediaManager.hpp"

Media::~Media()
{
	for (std::map<std::string, Media::Image*>::const_iterator it = images.begin();
			it != settings.end();
			++it) {
		delete it->second;
	}
}

bool Media::import(const char *file, IVideoDriver* driver)
{		
	return add(file, driver->createImageFromFile(file));
}

bool Media::add(const char *file, IImage *image)
{
	if (!image)
		return false;

	images[file] = new Media::Image(image);
	return true;
}

Media::Image *Media::get(const char* name)
{
	if (!images[name]) {
		return NULL;
	}
	images[name]->grab();
	return images[name];
}

void Media::clearGrabs()
{
	for (std::map<std::string, Media::Image*>::const_iterator it = images.begin();
			it != images.end();
			++it) {
		it->second->dropAll();
	}
}
