#include "helpers.hpp"
#include "../util/string.hpp"
#include "../util/filesys.hpp"

void save_file(FileFormat *writer, EditorState *state, std::string file)
{
	if (!writer || !state)
		return;

	std::string after(file);

	if (after.find('.') == std::string::npos) {
		after += '.';
		after += writer->getExtension();
	}

	std::cerr << "Saving to " << after << std::endl;

	if (!writer->write(state->project, after)) {
		if (writer->error_code == EFFE_IO_ERROR) {
			state->device->getGUIEnvironment()->addMessageBox(L"Unable to Save",
					L"Unable to open file to save to");
		} else {
			state->device->getGUIEnvironment()->addMessageBox(L"Unable to Save",
					L"Unknown reason");
		}
	}

	delete writer;
}

void export_textures(std::string dir, EditorState *state)
{
	if (dir == "")
		return;

	std::cerr << "Exporting Images to " << dir.c_str() << std::endl;
	CreateDir(dir.c_str());
	Media *media = &state->project->media;
	std::map<std::string, Media::Image*>& images = media->getList();
	for (std::map<std::string, Media::Image*>::const_iterator it = images.begin();
			it != images.end();
			++it) {
		Media::Image *image = it->second;
		if (!image->get()) {
			std::cerr << "Image->get() is NULL!" << std::endl;
			continue;
		}
		state->device->getVideoDriver()->writeImageToFile(image->get(), (dir + image->name).c_str());
	}
}
