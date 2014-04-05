#include "EditorState.h"

EditorState::EditorState(irr::IrrlichtDevice* dev,Project* proj,Configuration* settings)
:_device(dev),project(proj),currentmode(0),
plane_tri(NULL),mousedown(false), _settings(settings),
close_requested(false),_modeCount(0),_menu(NULL)
{
	for (int i = 0; i<5; i++){
		_modes[i] = NULL;
	}

	for (int i = 0; i<NUMBER_OF_KEYS; i++){
		keys[i] = EKS_UP;
	}
}

void EditorState::AddMode(EditorMode* value){
	_modes[_modeCount] = value;
	value->setId(_modeCount);
	_modeCount ++;
}

void EditorState::SelectMode(int id){
	if (!_modes[id])
		return;

	if (Mode())
		Mode()->unload();

	currentmode = id;

	Menu()->init();

	if (Mode())
		Mode()->load();
}
