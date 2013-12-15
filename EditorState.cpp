#include "EditorState.h"

EditorState::EditorState(irr::IrrlichtDevice* dev,Project* proj,Configuration* settings)
:_device(dev),project(proj),currentmode(0),
plane_tri(NULL),mousedown(false), _settings(settings),
close_requested(false),
_menu(NULL)
{
	_modes = new list<EditorMode*>();

	for (int i = 0; i<NUMBER_OF_KEYS; i++){
		keys[i] = EKS_UP;
	}
}

EditorMode* EditorState::Mode() const{
	int i = 0;
	for(list<EditorMode*>::ConstIterator Iterator = _modes->begin(); Iterator != _modes->end(); ++Iterator)
	{
		EditorMode* n = *Iterator;
		if (n){
			if (i==currentmode)
				return n;
			i++;
		}
	}
	return NULL;
}

void EditorState::AddMode(EditorMode* value){
	_modes->push_back(value);
}