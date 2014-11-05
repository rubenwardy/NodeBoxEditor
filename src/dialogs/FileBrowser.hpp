#ifndef FILEBROWSER_HPP_INCLUDED
#define FILEBROWSER_HPP_INCLUDED
#include "../common.hpp"

class FileBrowser
{
public:
	FileBrowser():
		cur_dir(""),
		device(NULL),
		browser(NULL),
		path(NULL)
	{}
	void setup(IrrlichtDevice *idevice, IGUIListBox *ebrowser, IGUIEditBox *epath)
	{
		device = idevice;
		browser = ebrowser;
		path = epath;
	}
	void build_gui();
	void show_directory(std::string dir);
	bool OnEvent(const SEvent &event);
private:
	std::string cur_dir;
	IGUIListBox *browser;
	IGUIEditBox *path;
	IrrlichtDevice *device;
};

#endif
