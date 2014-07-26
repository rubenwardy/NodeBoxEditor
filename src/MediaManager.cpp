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

bool Media::import(const char *file, IVideoDriver* driver)
{		
	return add(filenameWithExt(file).c_str(), driver->createImageFromFile(file));
}

bool Media::add(const char *file, IImage *image)
{
	if (!image)
		return false;

	if (images.find(file) != images.end()) {
		std::cerr << "Image '" << file << "' already exists in file manager!" << std::endl;
		return false;
	}
	images[file] = new Media::Image(file, image);
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
