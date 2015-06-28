#include "EditorState.hpp"

EditorState::EditorState(irr::IrrlichtDevice* dev, Project* proj, Configuration* settings) :
	device(dev),
	project(proj),
	currentmode(0),
	plane_tri(NULL),
	mousedown(false),
	settings(settings),
	close_requested(false),
	modeCount(0),
	menu(NULL)
{
	for (int i = 0; i < 5; i++) {
		modes[i] = NULL;
	}

	for (int i = 0; i < NUMBER_OF_KEYS; i++) {
		keys[i] = EKS_UP;
	}
}

void EditorState::AddMode(EditorMode* value)
{
	modes[modeCount] = value;
	value->id = modeCount;
	modeCount++;
}

void EditorState::SelectMode(int id)
{
	if (menu->dialog)
		if (!menu->dialog->canClose() || !menu->dialog->close())
			return;

	if (!modes[id]) {
		return;
	}

	if (Mode()) {
		Mode()->unload();
	}

	currentmode = id;

	menu->init();

	if (Mode()) {
		Mode()->load();
	}
}

EViewportType stringToType(std::string input, EViewportType def)
{
	if (input == "pers")
		return VIEWT_PERS;
	else if (input == "front")
		return VIEWT_FRONT;
	else if (input == "back")
		return VIEWT_BACK;
	else if (input == "left")
		return VIEWT_LEFT;
	else if (input == "right")
		return VIEWT_RIGHT;
	else if (input == "top")
		return VIEWT_TOP;
	else if (input == "bottom")
		return VIEWT_BOTTOM;
	else
		return def;
}

EViewportType EditorState::getEViewportType(EViewport id)
{
	switch (id) {
	case VIEW_TL:
		return stringToType(settings->get("viewport_top_left"), VIEWT_PERS);
	case VIEW_TR:
		return stringToType(settings->get("viewport_top_right"), VIEWT_TOP);
	case VIEW_BL:
		return stringToType(settings->get("viewport_bottom_left"), VIEWT_FRONT);
	case VIEW_BR:
		return stringToType(settings->get("viewport_bottom_right"), VIEWT_PERS);
	}
}
