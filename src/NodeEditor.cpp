#include "NodeEditor.h"
#include "guihelpers.h"

NodeEditor::NodeEditor(EditorState* st):EditorMode(st){}

void NodeEditor::load(){
	IGUIStaticText* sidebar = GetState()->Menu()->GetSideBar();
	IGUIEnvironment* guienv = GetState()->GetDevice()->getGUIEnvironment();

	if (!sidebar || !guienv)
		return;

	sidebar->setText(L"Node Tool");
	IGUIListBox* lb = guienv->addListBox(rect<s32>(20,30,230,128),sidebar,ENG_GUI_MAIN_LISTBOX,true);

	if (lb){
		//lb->setVisible(false);
		IGUIButton* b = guienv->addButton(rect<s32>(0,100,50,125),lb,ENG_GUI_MAIN_ADD,L"+",L"Add a node");
		IGUIButton* c = guienv->addButton(rect<s32>(60,100,110,125),lb,ENG_GUI_MAIN_DEL,L"-",L"Delete node");
		IGUIButton* d = guienv->addButton(rect<s32>(120,100,170,125),lb,ENG_GUI_MAIN_EDIT,L"Edit",L"Edit node boxes");
		b->setNotClipped(true);
		c->setNotClipped(true);
		d->setNotClipped(true);
	}

	// Create node properties
	{
		// Create properties
		IGUIStaticText* t = guienv->addStaticText(L"Properties",rect<s32>(0,170,120,190),false,true,sidebar,ENG_GUI_PROP);
		t->setVisible(false);

		// Add name properties box
		guienv->addStaticText(L"Name:",rect<s32>(10,30,50,50),false,true,t)->setNotClipped(true);
		guienv->addEditBox(L"",rect<s32>(60,30,210,50),true,t,ENG_GUI_PROP_NAME)->setNotClipped(true);		

		// Add positioning
		addXYZ(t,guienv,vector2di(10,60),ENG_GUI_PROP_X);

		// Add buttons
		guienv->addButton(rect<s32>(30,250,100,280),t,ENG_GUI_PROP_UPDATE,L"Update",L"")->setNotClipped(true);
		guienv->addButton(rect<s32>(110,250,180,280),t,ENG_GUI_PROP_REVERT,L"Revert",L"")->setNotClipped(true);
	}
	load_ui();
}

void NodeEditor::load_ui(){
	IGUIStaticText* sidebar = GetState()->Menu()->GetSideBar();
	IGUIEnvironment* guienv = GetState()->GetDevice()->getGUIEnvironment();

	if (!sidebar || !guienv)
		return;

	IGUIListBox* lb = (IGUIListBox*) sidebar->getElementFromId(ENG_GUI_MAIN_LISTBOX);

	if (lb){
		lb->clear();
		sidebar->getElementFromId(ENG_GUI_PROP)->setVisible(false);

		list<Node*>* nodes = GetState()->project->GetList();
		for (irr::core::list<Node*>::Iterator it=nodes->begin();it!=nodes->end();it++){
			Node* node = *it;
			if (node){
				size_t origsize = strlen(node->name.c_str()) + 1;
				static wchar_t wcstring[1024];
				mbstowcs(wcstring, node->name.c_str(), origsize);
				wcscat(wcstring, L"");
				lb->addItem(wcstring);
			}
		}
		lb->setSelected(lb->getListItem(GetState()->project->GetSelectedNodeId()));

		fillProperties();
	}

	
}

void NodeEditor::fillProperties(){
	IGUIStaticText* sidebar = GetState()->Menu()->GetSideBar();

	if (!sidebar)
		return;

	Node* node = GetState()->project->GetCurrentNode();

	if (!node)
		return;

	sidebar->getElementFromId(ENG_GUI_PROP)->setVisible(true);
	fillTB(sidebar,ENG_GUI_PROP,ENG_GUI_PROP_X,node->getPosition().X);
	fillTB(sidebar,ENG_GUI_PROP,ENG_GUI_PROP_Y,node->getPosition().Y);
	fillTB(sidebar,ENG_GUI_PROP,ENG_GUI_PROP_Z,node->getPosition().Z);

	IGUIElement* e = sidebar->getElementFromId(ENG_GUI_PROP)->getElementFromId(ENG_GUI_PROP_NAME);

	if (e){
		IGUIEditBox* b = static_cast<IGUIEditBox*>(e);

		if (!b)
			return;

		b->setText(stringw(node->name).c_str());
	}
}

void NodeEditor::unload(){}

void NodeEditor::update(double dtime){}

void NodeEditor::draw(irr::video::IVideoDriver* driver){}

void NodeEditor::viewportTick(VIEWPORT window,irr::video::IVideoDriver* driver,rect<s32> offset){}

bool NodeEditor::OnEvent(const irr::SEvent &event){
	if (event.EventType == EET_GUI_EVENT){
		if (event.GUIEvent.EventType == EGET_BUTTON_CLICKED){
			switch (event.GUIEvent.Caller->getID()){
			case ENG_GUI_MAIN_EDIT:
				{
					if (GetState()->project->GetCurrentNode()){
						// TODO: This line is terrible
						GetState()->SelectMode(0);
						return true;
					}
				}
				break;
			case ENG_GUI_MAIN_ADD:
				{
					GetState()->project->AddNode(new Node(GetState()->GetDevice(),GetState()));
					load_ui();
				}
				break;
			case ENG_GUI_MAIN_DEL:
				{
					if (GetState()->project->GetCurrentNode()){
						//GetState()->project->Remove
						return true;
					}
				}
				break;
			case ENG_GUI_PROP_REVERT:
				{
					fillProperties();
				}
				break;
			case ENG_GUI_PROP_UPDATE:
				{

					IGUIElement* prop = GetState()->Menu()->GetSideBar()->getElementFromId(ENG_GUI_PROP);

					if (!prop)
						return false;

					Node* node = GetState()->project->GetCurrentNode();

					if (!node)
						return false;

					try{
						node->name = prop->getElementFromId(ENG_GUI_PROP_NAME)->getText();
						#if IRRLICHT_VERSION_MAJOR == 1 && IRRLICHT_VERSION_MINOR < 8
						node->name.replace(' ','_');
						#else
						node->name = node->name.replace(' ','_');
						#endif
						node->setPosition(vector3di(
							wcstod(prop->getElementFromId(ENG_GUI_PROP_X)->getText(),NULL),
							wcstod(prop->getElementFromId(ENG_GUI_PROP_Y)->getText(),NULL),
							wcstod(prop->getElementFromId(ENG_GUI_PROP_Z)->getText(),NULL)
						));
						node->remesh();
						load_ui();
					}catch(void* e){
						GetState()->GetDevice()->getGUIEnvironment()->addMessageBox(L"Update failed",L"Please check that the properties contain only numbers.");
					}
				}
				break;
			}
		}else if (event.GUIEvent.EventType == EGET_LISTBOX_CHANGED){
			IGUIListBox* lb = (IGUIListBox*) GetState()->Menu()->GetSideBar()->getElementFromId(ENG_GUI_MAIN_LISTBOX);	
			if (lb && GetState()->project->GetNode(lb->getSelected())){
				GetState()->project->SelectNode(lb->getSelected());
				load_ui();
			}
					
		}
	}	
	return false;
}