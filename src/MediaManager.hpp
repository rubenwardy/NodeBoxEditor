#ifndef MEDIAMANAGER_HPP_INCLUDED
#define MEDIAMANAGER_HPP_INCLUDED
#include "common.hpp"

class Media
{
public:
	~Media()
	{
		for (std::map<std::string, Media::Image*>::const_iterator it = images.begin();
				it != settings.end();
				++it) {
			delete it->second;
		}
	}

	class Image
	{
	public:
		Image(IImage *the_data):
			data(the_data)
		{}

		~Image()
		{
			data->drop();
		}

		IImage *get() const { return data; }
		void grab() { holders++; }
		void drop() { assert(holders > 0); holders--; }
		void dropAll() { holders = 0; }
	private:
		IImage *data;
		unsigned int holders;
	};

	Media::Image *get(const char* name)
	{
		images[name]->grab();
		return images[name];
	}

	void clearGrabs()
	{
		for (std::map<std::string, Media::Image*>::const_iterator it = images.begin();
				it != settings.end();
				++it) {
			it->second->dropAll();
		}
	}
private:
	std::map<std::string, Media::Image*> images;
};

#endif
