#ifndef MEDIAMANAGER_HPP_INCLUDED
#define MEDIAMANAGER_HPP_INCLUDED
#include "../common.hpp"
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
			holders(0),
			origpath("")
		{}

		Image():
			data(NULL)
		{}

		std::string name;
		std::string origpath;

		IImage *get() const { return data; }
		void grab() { holders++; }
		void drop() { assert(holders > 0); holders--; }
		void dropAll() { holders = 0; }
		void deleteImage() { data->drop(); }
		unsigned int getHolders() const { return holders; }
		void update(IImage *ndata) { data->drop(); data = ndata; }
	private:
		IImage *data;
		unsigned int holders;
	};

	Media() { std::cerr << "Media Manager created!" << std::endl; }
	Media(const Media &old) { std::cerr << "Media Manager copied! (This shouldn't happen)" << std::endl; }
	~Media();
	bool import(std::string filepath, std::string filename, IVideoDriver* driver);
	bool add(std::string filepath, std::string filename, IImage *image, bool overwrite = false);
	Media::Image *get(const char* name);
	void clearGrabs();
	void debug();
	std::map<std::string, Media::Image*>& getList() const {return (std::map<std::string, Media::Image*>&)images;};
private:
	std::map<std::string, Media::Image*> images;
};

#endif
