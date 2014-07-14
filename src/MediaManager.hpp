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
		Image(const char *the_name, IImage *the_data):
			name(the_name),
			data(the_data),
			holders(0)
		{}
		
		Image():
			data(NULL)
		{}

		std::string name;

		IImage *get() const { return data; }
		void grab() { holders++; }
		void drop() { assert(holders > 0); holders--; }
		void dropAll() { holders = 0; }
		void deleteImage() { data->drop(); }
		unsigned int getHolders() const { return holders; }
	private:
		IImage *data;
		unsigned int holders;
	};

	~Media();
	bool import(const char *file, IVideoDriver* driver);
	bool add(const char *file, IImage *image);
	Media::Image *get(const char* name);
	void clearGrabs();
	void debug();
private:
	std::map<std::string, Media::Image*> images;
};

#endif
