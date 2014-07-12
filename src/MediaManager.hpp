#ifndef MEDIAMANAGER_HPP_INCLUDED
#define MEDIAMANAGER_HPP_INCLUDED
#include "common.hpp"
#include <assert.h>
#include <map>

class Media
{
public:
	
	class Image
	{
	public:
		Image(IImage *the_data):
			data(the_data)
		{}
		
		Image():
			data(NULL)
		{}

		IImage *get() const { return data; }
		void grab() { holders++; }
		void drop() { assert(holders > 0); holders--; }
		void dropAll() { holders = 0; }
		void deleteImage() { data->drop(); }
	private:
		IImage *data;
		unsigned int holders;
	};

	bool import(const char *file, IVideoDriver* driver)
	{		
		return add(file, driver->createImageFromFile(file));
	}

	bool add(const char *file, IImage *image)
	{
		if (!image)
			return false;
	
		images[file] = new Media::Image(image);
		return true;
	}

	Media::Image *get(const char* name)
	{
		if (!images[name]) {
			return NULL;
		}
		images[name]->grab();
		return images[name];
	}

	void clearGrabs()
	{
		for (std::map<std::string, Media::Image*>::const_iterator it = images.begin();
				it != images.end();
				++it) {
			it->second->dropAll();
		}
	}
private:
	std::map<std::string, Media::Image*> images;
};

#endif
