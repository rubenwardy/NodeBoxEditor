#include "NodeEditor.hpp"
#include <list>
#include "../project/node.hpp"
#include "../GUIHelpers.hpp"
#include "../util/string.hpp"

NodeEditor::NodeEditor(EditorState* st) :
	EditorMode(st)
{
}

void NodeEditor::load()
{
	state->project->remesh();
	IGUIStaticText* sidebar = state->menu->sidebar;
	IGUIEnvironment* guienv = state->device->getGUIEnvironment();

	if (!sidebar || !guienv)
		return;

	sidebar->setText(L"Node Tool");
	IGUIListBox* lb = guienv->addListBox(rect<s32>(20, 30, 230, 128), sidebar, ENG_GUI_MAIN_LISTBOX, true);

	if (lb) {
		//lb->setVisible(false);
		IGUIButton* b1 = guienv->addButton(rect<s32>(0, 100, 50, 125),
				lb, ENG_GUI_MAIN_ADD, L"+", L"Add a node");
		IGUIButton* b2 = guienv->addButton(rect<s32>(60, 100, 110, 125),
				lb, ENG_GUI_MAIN_DEL, L"-", L"Delete node");
		IGUIButton* b3 = guienv->addButton(rect<s32>(120, 100, 170, 125),
				lb, ENG_GUI_MAIN_EDIT, L"Edit", L"Edit node boxes");
		b1->setNotClipped(true);
		b2->setNotClipped(true);
		b3->setNotClipped(true);
	}

	// Create node properties
	{
		// Create properties
		IGUIStaticText* t = guienv->addStaticText(L"Properties",
				rect<s32>(0, 170, 120, 190),
				false, true, sidebar, ENG_GUI_PROP);
		t->setVisible(false);

		// Add name properties box
		guienv->addStaticText(L"Name:", rect<s32>(10, 30, 50, 50),
				false, true, t)->setNotClipped(true);
		guienv->addEditBox(L"", rect<s32>(60, 30, 210, 50), true,
				t, ENG_GUI_PROP_NAME)->setNotClipped(true);

		// Add positioning
		addXYZ(t, guienv, vector2di(10, 60), ENG_GUI_PROP_X);

		// Add buttons
		guienv->addButton(rect<s32>(30, 250, 100, 280), t,
				ENG_GUI_PROP_UPDATE, L"Update", L"")
			->setNotClipped(true);
		guienv->addButton(rect<s32>(110, 250, 180, 280), t,
				ENG_GUI_PROP_REVERT, L"Revert", L"")
			->setNotClipped(true);
	}
	load_ui();
}

void NodeEditor::load_ui(){
	IGUIStaticText* sidebar = state->menu->sidebar;
	IGUIEnvironment* guienv = state->device->getGUIEnvironment();

	if (!sidebar || !guienv) {
		return;
	}

	IGUIListBox* lb = (IGUIListBox*) sidebar->getElementFromId(ENG_GUI_MAIN_LISTBOX);

	if (lb) {
		lb->clear();
		sidebar->getElementFromId(ENG_GUI_PROP)->setVisible(false);

		std::list<Node*> & nodes = state->project->nodes;
		for (std::list<Node*>::const_iterator it = nodes.begin();
				it != nodes.end();
				++it) {
			std::wstring wide = narrow_to_wide((*it)->name);
			lb->addItem(wide.c_str());
		}
		lb->setSelected(lb->getListItem(state->project->GetSelectedNodeId()));

		fillProperties();
	}
}

void NodeEditor::fillProperties(){
	IGUIStaticText* sidebar = state->menu->sidebar;

	if (!sidebar) {
		return;
	}

	Node* node = state->project->GetCurrentNode();

	if (!node) {
		return;
	}

	sidebar->getElementFromId(ENG_GUI_PROP)->setVisible(true);
	fillTB(sidebar, ENG_GUI_PROP, ENG_GUI_PROP_X, node->position.X);
	fillTB(sidebar, ENG_GUI_PROP, ENG_GUI_PROP_Y, node->position.Y);
	fillTB(sidebar, ENG_GUI_PROP, ENG_GUI_PROP_Z, node->position.Z);

	IGUIElement* element = sidebar->getElementFromId(ENG_GUI_PROP)
			->getElementFromId(ENG_GUI_PROP_NAME);

	if (element) {
		IGUIEditBox* editbox = static_cast<IGUIEditBox *>(element);

		std::wstring wide = narrow_to_wide(node->name);
		editbox->setText(wide.c_str());
	}
}


void NodeEditor::unload()
{}


void NodeEditor::update(double dtime)
{}


void NodeEditor::draw(irr::video::IVideoDriver* driver)
{}


void NodeEditor::viewportTick(EViewport window, irr::video::IVideoDriver* driver, rect<s32> offset)
{}


bool NodeEditor::OnEvent(const irr::SEvent &event)
{
	if (event.EventType == EET_GUI_EVENT){
		if (event.GUIEvent.EventType == EGET_BUTTON_CLICKED){
			switch (event.GUIEvent.Caller->getID()){
			case ENG_GUI_MAIN_EDIT:
				if (state->project->GetCurrentNode()){
					// TODO: This line is terrible
					state->SelectMode(0);
					return true;
				}
				break;
			case ENG_GUI_MAIN_ADD:
				state->project->AddNode(state);
				load_ui();
				return true;
				break;
			case ENG_GUI_MAIN_DEL:
				if (state->project->GetCurrentNode()){
					state->project->DeleteNode(state->project->GetSelectedNodeId());
					load_ui();
					return true;
				}
				break;
			case ENG_GUI_PROP_REVERT:
				fillProperties();
				break;
			case ENG_GUI_PROP_UPDATE:
				updateProperties();
				break;
			}
		} else if (event.GUIEvent.EventType == EGET_LISTBOX_CHANGED) {
			IGUIListBox* lb = (IGUIListBox*) state->menu->sidebar->getElementFromId(ENG_GUI_MAIN_LISTBOX);
			if (lb && state->project->GetNode(lb->getSelected())){
				state->project->SelectNode(lb->getSelected());
				load_ui();
			}
		}
	} else if (event.EventType == EET_KEY_INPUT_EVENT && !event.KeyInput.PressedDown) {
		if (event.KeyInput.Key == KEY_RETURN){
			updateProperties();
			return false;
		}

		irr::gui::IGUIEnvironment *guienv = state->device->getGUIEnvironment();
		if (guienv->getFocus() && guienv->getFocus()->getType() == EGUIET_EDIT_BOX) {
			return false;
		}

		if (event.KeyInput.Key == KEY_INSERT){
			state->project->AddNode(state);
			load_ui();
		} else if (event.KeyInput.Key == KEY_DELETE){
			if (state->project->GetCurrentNode()){
				state->project->DeleteNode(state->project->GetSelectedNodeId());
				load_ui();
				return true;
			}
		} else if (event.KeyInput.Key == KEY_DOWN){
			IGUIListBox* lb = (IGUIListBox*) state->menu->sidebar->getElementFromId(ENG_GUI_MAIN_LISTBOX);
			int idx = state->project->GetSelectedNodeId();
			if (lb && idx < (int)state->project->GetNodeCount() - 1){
				state->project->SelectNode(idx + 1);
				load_ui();
			}
		} else if (event.KeyInput.Key == KEY_UP){
			IGUIListBox* lb = (IGUIListBox*) state->menu->sidebar->getElementFromId(ENG_GUI_MAIN_LISTBOX);
			int idx = state->project->GetSelectedNodeId();
			if (lb && idx > 0){
				state->project->SelectNode(idx - 1);
				load_ui();
			}
		}
	}
	return false;
}

void NodeEditor::updateProperties() {
	IGUIElement* prop = state->menu->sidebar->getElementFromId(ENG_GUI_PROP);
	Node* node = state->project->GetCurrentNode();
	if (!prop || !node) {
		return;
	}

	try {
		irr::core::stringc name = prop->getElementFromId(ENG_GUI_PROP_NAME)->getText();
		node->name = str_replace(std::string(name.c_str(), name.size()), ' ', '_');
		int y = (int)wcstod(prop->getElementFromId(ENG_GUI_PROP_Y)->getText(), NULL);
		if (state->settings->getBool("no_negative_node_y") && y < 0) {
			std::list<Node*> & nodes = state->project->nodes;
			for (std::list<Node*>::const_iterator it = nodes.begin();
					it != nodes.end();
					++it) {
				(*it)->position.Y -= y; // Remember, y is negative
			}
			state->project->remesh();
			y = 0;
		}

		node->position = vector3di(
			wcstod(prop->getElementFromId(ENG_GUI_PROP_X)->getText(), NULL),
			y,
			wcstod(prop->getElementFromId(ENG_GUI_PROP_Z)->getText(), NULL)
		);
		node->remesh();
		load_ui();
	} catch(void* e) {
		state->device->getGUIEnvironment()->addMessageBox(L"Update failed",
				L"Please check that the properties contain only numbers.");
	}
}

irr::video::ITexture* NodeEditor::icon()
{
	static irr::video::ITexture* icon = state->device->
			getVideoDriver()->getTexture("media/icon_mode_node.png");
	return icon;
}
