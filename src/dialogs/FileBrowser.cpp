#include "FileBrowser.hpp"
#include "../util/filesys.hpp"
#include "../util/string.hpp"

void FileBrowser::build_gui()
{}

void FileBrowser::show_directory(std::string dir)
{
	std::vector<std::string> files = filesInDirectory(dir);
	browser->clear();
	for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it) {
		if ((*it)[0] == '.')
			continue;
		browser->addItem(narrow_to_wide(*it).c_str());
	}
}

bool FileBrowser::OnEvent(const SEvent &event)
{
	if (event.EventType == EET_GUI_EVENT &&
			event.GUIEvent.EventType == EGET_LISTBOX_CHANGED &&
			event.GUIEvent.Caller == browser) {
		if (browser->getSelected() >= 0)
			path->setText(browser->getListItem(browser->getSelected()));
	}
	return false;
}
