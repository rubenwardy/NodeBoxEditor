#include "FileDialog.hpp"
#include "../util/string.hpp"
#include "../util/filesys.hpp"
#include "../FileFormat/FileFormat.hpp"
#include "../util/tinyfiledialogs.h"
#include "../FileFormat/helpers.hpp"

void FileDialog_open_project(EditorState *state)
{
	std::string path = getSaveLoadDirectory(state->settings->get("save_directory"),
			state->isInstalled);

	const char* filters[] = {"*.nbe"};
	const char *cfile = tinyfd_openFileDialog("Open Project",
			path.c_str(), 1, filters, 0);

	if (!cfile)
		return;

	std::string file = cfile;

	if (file == "")
		return;


	std::cerr << file.c_str() << std::endl;

	// Get file parser
	FileFormat *parser = getFromType(FILE_FORMAT_NBE, state);
	if (!parser) {
		state->device->getGUIEnvironment()->addMessageBox(L"Unable to open",
		L"File format does not exist. (It is based on file extensions)");
		return;
	}

	// Get directory, and load#
	std::cerr << "Reading from " << file << std::endl;
	Project *tmp = parser->read(file);
	if (tmp) {
		if (state->project)
			delete state->project;
			state->project = tmp;
			state->project->SelectNode(0);
			state->Mode()->unload();
			state->menu->init();
			state->Mode()->load();
			delete parser;
			parser = NULL;
			return;
	} else {
		switch(parser->error_code) {
		case EFFE_IO_ERROR:
			state->device->getGUIEnvironment()->addMessageBox(L"Unable to open",
				L"Failed to open the file\n\t(Does it not exist, or is it readonly?)");
			break;
		case EFFE_READ_OLD_VERSION:
			state->device->getGUIEnvironment()->addMessageBox(L"Unable to open",
				L"This file is outdated and is not supported");
			break;
		case EFFE_READ_NEW_VERSION:
			state->device->getGUIEnvironment()->addMessageBox(L"Unable to open",
				L"This file was created with a new version of NBE\n\t(Update your copy)");
			break;
		case EFFE_READ_PARSE_ERROR:
			state->device->getGUIEnvironment()->addMessageBox(L"Unable to open",
				L"An error occurred while reading the file - it may be corrupted\n\t(This should never happen)");
			break;
		case EFFE_READ_WRONG_TYPE:
			state->device->getGUIEnvironment()->addMessageBox(L"Unable to open",
				L"The file is not in the correct format\n\t(Are you opening the wrong type of file?)");
			break;
		default:
			state->device->getGUIEnvironment()->addMessageBox(L"Unable to open",
				L"Unknown error");
			break;
		}
	}
}

void FileDialog_import(EditorState *state)
{
	std::string path = getSaveLoadDirectory(state->settings->get("save_directory"),
			state->isInstalled);

	const char* filters[] = {"*.nbe"};
	const char *cfile = tinyfd_openFileDialog("Import Nodes",
			path.c_str(), 1, filters, 0);

	if (!cfile)
		return;

	std::string file = cfile;

	if (file == "")
		return;


	std::cerr << file.c_str() << std::endl;

	// Get file parser
	FileFormat *parser = getFromType(FILE_FORMAT_NBE, state);
	if (!parser) {
		state->device->getGUIEnvironment()->addMessageBox(L"Unable to open",
		L"File format does not exist.");
		return;
	}

	// Get directory, and load
	std::cerr << "Reading from " << file << std::endl;
	Project *tmp = parser->read(file, state->project);
	if (tmp) {
		state->project->remesh();
	} else {
		switch(parser->error_code) {
		case EFFE_IO_ERROR:
			state->device->getGUIEnvironment()->addMessageBox(L"Unable to open",
					L"Failed to open the file\n\t(Does it not exist, or is it readonly?)");
			break;
		case EFFE_READ_OLD_VERSION:
			state->device->getGUIEnvironment()->addMessageBox(L"Unable to open",
				L"This file is outdated and is not supported");
			break;
		case EFFE_READ_NEW_VERSION:
			state->device->getGUIEnvironment()->addMessageBox(L"Unable to open",
				L"This file was created with a new version of NBE\n\t(Update your copy)");
			break;
		case EFFE_READ_PARSE_ERROR:
			state->device->getGUIEnvironment()->addMessageBox(L"Unable to open",
				L"An error occurred while reading the file - it may be corrupted\n\t(This should never happen)");
			break;
		case EFFE_READ_WRONG_TYPE:
			state->device->getGUIEnvironment()->addMessageBox(L"Unable to open",
				L"The file is not in the correct format\n\t(Are you opening the wrong type of file?)");
			break;
		default:
			state->device->getGUIEnvironment()->addMessageBox(L"Unable to open",
				L"Unknown error");
			break;
		}
		delete parser;
		parser = NULL;
	}
}

void FileDialog_save_project(EditorState *state)
{
	// Get path
	std::string path = getSaveLoadDirectory(state->settings->get("save_directory"),
			state->isInstalled);

	const char* filters[] = {"*.nbe"};
	const char *cfile = tinyfd_saveFileDialog("Save Project", path.c_str(),
			1, filters);

	if (!cfile)
		return;

	std::string file = cfile;

	if (file == "")
		return;
	
	std::cerr << file << std::endl;

	FileFormat *writer = getFromType(FILE_FORMAT_NBE, state);
	save_file(writer, state, file);
}

void FileDialog_export(EditorState *state, int parser)
{
	// Get path
	std::string path = getSaveLoadDirectory(state->settings->get("save_directory"),
			state->isInstalled);

	const char* filters[] = {""};

	if (parser == (int)FILE_FORMAT_LUA)
		filters[0] = "*.lua";
	else if (parser == (int)FILE_FORMAT_CPP)
		filters[0] = "*.cpp";

	const char *cfile = tinyfd_saveFileDialog("Save Project", path.c_str(),
			1, filters);

	if (!cfile)
		return;

	std::string file = cfile;

	if (file == "")
		return;

	std::cerr << file << std::endl;

	FileFormat *writer = getFromType((FileFormatType)parser, state);
	save_file(writer, state, file);
}

#include "../FileFormat/obj.hpp"
#include <fstream>

void FileDialog_export_obj(EditorState *state, Node *node)
{
	// Get path
	std::string path = getSaveLoadDirectory(state->settings->get("save_directory"),
			state->isInstalled);

	const char* filters[] = {"*.obj"};

	const char *cfile = tinyfd_saveFileDialog("Export Node to Mesh", path.c_str(),
			1, filters);	

	if (!cfile)
		return;

	std::string filename = cfile;

	if (filename == "")
		return;

	std::string res = nodeToObj(node, filenameWithoutExt(filename));
	std::ofstream file(filename.c_str());
	if (!file)
		return;
	file << res.c_str();
	file.close();
}

void FileDialog_export_mod(EditorState *state)
{
	std::string path = getSaveLoadDirectory(state->settings->get("save_directory"),
			state->isInstalled);

	const char *cdir = tinyfd_selectFolderDialog ("Select Folder", path.c_str());

	if (!cdir)
		return;

	std::string dir = trim(cdir);

	if (dir == "")
		return;

	dir = cleanDirectoryPath(dir);
	export_textures(dir + "textures/", state);

	FileFormat *writer = getFromType(FILE_FORMAT_LUA, state);
	save_file(writer, state, dir + "init.lua");
}

void FileDialog_export_textures(EditorState *state)
{
	std::string path = getSaveLoadDirectory(state->settings->get("save_directory"),
			state->isInstalled);

	const char *cdir = tinyfd_selectFolderDialog ("Select Folder", path.c_str());

	if (!cdir)
		return;

	std::string dir = trim(cdir);

	if (dir == "")
		return;

	dir = cleanDirectoryPath(dir);
	export_textures(dir, state);
}
