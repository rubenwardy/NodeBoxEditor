#include "EditorState.h"

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
	if (!modes[id]) {
		return;
	}

	if (Mode()) {
		Mode()->unload();
	}

	currentmode = id;

	Menu()->init();

	if (Mode()) {
		Mode()->load();
	}
}

