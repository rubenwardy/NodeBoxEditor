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

bool Media::import(std::string filepath, std::string filename, IVideoDriver* driver)
{
	return add(filepath, filename, driver->createImageFromFile(filepath.c_str()));
}

bool Media::add(std::string filepath, std::string filename, IImage *image, bool overwrite)
{
	if (!image)
		return false;

	filename = trim(filename);
	const char *shortpath = filename.c_str();
	if (images[shortpath] != NULL) {
		if (overwrite) {
			std::cerr << "Overwriting '" << shortpath << "'" << std::endl;
			images[shortpath]->update(image);
			images[shortpath]->origpath = filepath;
		} else {
			std::cerr << "Failed to add image '" << shortpath
					<< "', it already exists (and overwrite was not authorised)"
					<< std::endl;
			return false;
		}
	} else {
		std::cerr << "Adding '" << shortpath << "'" << std::endl;
		images[shortpath] = new Media::Image(shortpath, image);
		images[shortpath]->origpath = filepath;
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
