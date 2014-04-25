#include "NBEditor.h"
#include "guihelpers.h"

// The gui id numbers for this mode
// NOTE: the maximum that can be here is 20
//       see in MenuState.h to raise limit
enum ENB_GUI{
	ENB_GUI_MAIN_LISTBOX = GUI_SIDEBAR + 1,	
	ENB_GUI_MAIN_MSG = GUI_SIDEBAR + 2,
	ENB_GUI_PROP = GUI_SIDEBAR + 3,
	ENB_GUI_PROP_X1 = GUI_SIDEBAR + 4,
	ENB_GUI_PROP_Y1 = GUI_SIDEBAR + 5,
	ENB_GUI_PROP_Z1 = GUI_SIDEBAR + 6,
	ENB_GUI_PROP_X2 = GUI_SIDEBAR + 7,
	ENB_GUI_PROP_Y2 = GUI_SIDEBAR + 8,
	ENB_GUI_PROP_Z2 = GUI_SIDEBAR + 9,
	ENB_GUI_PROP_NAME = GUI_SIDEBAR + 19,
	ENB_GUI_PROP_UPDATE = GUI_SIDEBAR + 11,
	ENB_GUI_PROP_REVERT = GUI_SIDEBAR + 12
};

NBEditor::NBEditor(EditorState* st)
:EditorMode(st),current(-1),prop_needs_update(false)
{
	// Top Window
	cdrs[0] = CDR(EVIEW_XZ,CDR_X_P);
	cdrs[1] = CDR(EVIEW_XZ,CDR_X_N);
	cdrs[2] = CDR(EVIEW_XZ,CDR_Z_P);
	cdrs[3] = CDR(EVIEW_XZ,CDR_Z_N);
	cdrs[4] = CDR(EVIEW_XZ,CDR_XZ);

	// Front window
	cdrs[5] = CDR(EVIEW_XY,CDR_X_P);
	cdrs[6] = CDR(EVIEW_XY,CDR_X_N);
	cdrs[7] = CDR(EVIEW_XY,CDR_Y_P);
	cdrs[8] = CDR(EVIEW_XY,CDR_Y_N);
	cdrs[9] = CDR(EVIEW_XY,CDR_XY);

	// Side window
	cdrs[10] = CDR(EVIEW_ZY,CDR_Y_P);
	cdrs[11] = CDR(EVIEW_ZY,CDR_Y_N);
	cdrs[12] = CDR(EVIEW_ZY,CDR_Z_P);
	cdrs[13] = CDR(EVIEW_ZY,CDR_Z_N);
	cdrs[14] = CDR(EVIEW_ZY,CDR_ZY);

	// Snappers
	for (int i=0;i<17;i++){
		snappers[i]=NULL;
	}

	printf("Making 16 pixel snap grid: \n");

	for (int a=-(NODE_RES/2);a<(NODE_RES/2)+1;a++){
		snappers[a+(NODE_RES/2)] = a*((float)1/(float)NODE_RES);
		printf(">> %f\n",snappers[a+8]);
	}
}

void NBEditor::load(){
	IGUIStaticText* sidebar = GetState()->Menu()->GetSideBar();
	IGUIEnvironment* guienv = GetState()->GetDevice()->getGUIEnvironment();

	if (sidebar){
		sidebar->setText(L"Node Box Tool");
		IGUIStaticText* t = guienv->addStaticText(L"No node selected",rect<s32>(20,30,140,100),false,true,sidebar,ENB_GUI_MAIN_MSG);
		
		IGUIListBox* lb = guienv->addListBox(rect<s32>(20,30,230,128),sidebar,ENB_GUI_MAIN_LISTBOX,true);

		if (lb){
			lb->setVisible(false);
			IGUIButton* b = guienv->addButton(rect<s32>(0,100,50,125),lb,GUI_PROJ_NEW_BOX,L"+",L"Add a node box");
			IGUIButton* c = guienv->addButton(rect<s32>(60,100,110,125),lb,GUI_PROJ_DELETE_BOX,L"-",L"Delete node box");
			b->setNotClipped(true);
			c->setNotClipped(true);
		}

		// Create nodebox properties
		{
			// Create properties
			t = guienv->addStaticText(L"Properties",rect<s32>(0,170,120,190),false,true,sidebar,ENB_GUI_PROP);
			t->setVisible(false);

			// Add name properties box
			guienv->addStaticText(L"Name:",rect<s32>(10,30,50,50),false,true,t)->setNotClipped(true);
			guienv->addEditBox(L"",rect<s32>(60,30,210,50),true,t,ENB_GUI_PROP_NAME)->setNotClipped(true);		

			// Add positioning
			addXYZ(t,guienv,vector2di(10,60),ENB_GUI_PROP_X1);
			addXYZ(t,guienv,vector2di(10,160),ENB_GUI_PROP_X2); // 60

			// Add buttons
			guienv->addButton(rect<s32>(30,250,100,280),t,ENB_GUI_PROP_UPDATE,L"Update",L"")->setNotClipped(true);
			guienv->addButton(rect<s32>(110,250,180,280),t,ENB_GUI_PROP_REVERT,L"Revert",L"")->setNotClipped(true);
		}
	}
	load_ui();
}

void NBEditor::load_ui(){
	IGUIStaticText* sidebar = GetState()->Menu()->GetSideBar();

	if (!sidebar)
		return;

	IGUIEnvironment* guienv = GetState()->GetDevice()->getGUIEnvironment();
	Node* node = GetState()->project->GetCurrentNode();
	if (!node){
		sidebar->getElementFromId(ENB_GUI_MAIN_MSG)->setVisible(true);
		sidebar->getElementFromId(ENB_GUI_MAIN_LISTBOX)->setVisible(false);
		sidebar->getElementFromId(ENB_GUI_PROP)->setVisible(false);
	}else{
		IGUIListBox* lb = (IGUIListBox*) sidebar->getElementFromId(ENB_GUI_MAIN_LISTBOX);		
		sidebar->getElementFromId(ENB_GUI_MAIN_MSG)->setVisible(false);
		sidebar->getElementFromId(ENB_GUI_PROP)->setVisible(false);

		if (lb){
			lb->clear();
			lb->setVisible(true);
			std::vector<NodeBox*> boxes = node->GetBoxes();
			for (std::vector<NodeBox*>::const_iterator it = boxes.begin();
					it != boxes.end();
					++it) {
				lb->addItem((*it)->name.c_str());
			}
			lb->setSelected(lb->getListItem(node->GetId()));
		}

		fillProperties();
	}
}

void NBEditor::fillProperties(){
	IGUIStaticText* sidebar = GetState()->Menu()->GetSideBar();

	if (!sidebar)
		return;

	Node* node = GetState()->project->GetCurrentNode();

	if (!node)
		return;

	NodeBox* nb = node->GetCurrentNodeBox();

	if (!nb)	
		return;

	sidebar->getElementFromId(ENB_GUI_PROP)->setVisible(true);
	fillTB(sidebar,ENB_GUI_PROP,ENB_GUI_PROP_X1,nb->one.X);
	fillTB(sidebar,ENB_GUI_PROP,ENB_GUI_PROP_Y1,nb->one.Y);
	fillTB(sidebar,ENB_GUI_PROP,ENB_GUI_PROP_Z1,nb->one.Z);
	fillTB(sidebar,ENB_GUI_PROP,ENB_GUI_PROP_X2,nb->two.X);
	fillTB(sidebar,ENB_GUI_PROP,ENB_GUI_PROP_Y2,nb->two.Y);
	fillTB(sidebar,ENB_GUI_PROP,ENB_GUI_PROP_Z2,nb->two.Z);

	IGUIElement* e = sidebar->getElementFromId(ENB_GUI_PROP)->getElementFromId(ENB_GUI_PROP_NAME);

	if (e){
		IGUIEditBox* b = static_cast<IGUIEditBox*>(e);

		if (!b)
			return;

		b->setText(stringw(nb->name).c_str());
	}
}

void NBEditor::unload(){}

void NBEditor::update(double dtime){
}

void NBEditor::draw(irr::video::IVideoDriver* driver){
	if (wasmd && !GetState()->mousedown){
		current = -1;
	}

	static irr::video::ITexture* scale = driver->getTexture("media/gui_scale.png");

	for (int i=0;i<15;i++){
		if (cdrs[i].visible){
			rect<s32> drawarea = rect<s32>(cdrs[i].position.X-5,cdrs[i].position.Y-5,cdrs[i].position.X+5,cdrs[i].position.Y+5);
			
			if (!wasmd && GetState()->mousedown && drawarea.isPointInside(GetState()->mouse_position)){
				current = i;
			}

			SColor color = SColor(255,255,255,255);

			if (current == i){
				color = SColor(255,0,0,255);
			}else if (GetState()->keys[KEY_LCONTROL]==EKS_DOWN){
				color = SColor(255,255,255,0);				
			}

			driver->draw2DImage(
				scale,
				drawarea.UpperLeftCorner,
				rect<s32>(0,0,10,10),NULL,color,true
			);
			
			
			cdrs[i].visible = false;
		}
	}

	if (!wasmd && GetState()->mousedown){
		wasmd = true;		
	}else if (wasmd && !GetState()->mousedown){
		wasmd = false;
	}
}

void NBEditor::viewportTick(VIEWPORT window,irr::video::IVideoDriver* driver,rect<s32> offset){
	for (int i=0;i<15;i++){
		if (cdrs[i].window == window){
			cdrs[i].update(this,(current == i),offset);
		}
	}
}

void CDR::update(NBEditor* editor,bool drag,rect<s32> offset){
	if (!editor->GetState()->project){
		printf("Project for NBEditor::updatePoint() not found!\n");
		return;
	}

	if (!editor->GetState()->Settings()->getSettingAsBool("always_show_position_handle")){
		if (editor->GetState()->keys[KEY_LSHIFT]==EKS_UP){
			if (type == CDR_XY || type == CDR_XZ || type == CDR_ZY){
				return;
			}
		}else{
			if (!(type == CDR_XY || type == CDR_XZ || type == CDR_ZY)){
				return;
			}
		}
	}

	Node* node = editor->GetState()->project->GetCurrentNode();

	if (!node)
		return;

	NodeBox* box = node->GetCurrentNodeBox();

	if (!box)
		return;

	if (drag){
			// get mouse position
			position2di target = editor->GetState()->mouse_position;
			target.X -= 5;
			target.Y -= 5;
			target -= offset.UpperLeftCorner;

			// get the ray
			line3d<irr::f32> ray = editor->GetState()->GetDevice()->getSceneManager()->getSceneCollisionManager()->getRayFromScreenCoordinates(target,editor->GetState()->GetDevice()->getSceneManager()->getActiveCamera());

			// contains the output values
			vector3df wpos = vector3df(0,0,0); // the collision position
			#if IRRLICHT_VERSION_MAJOR == 1 && IRRLICHT_VERSION_MINOR < 8
				const ISceneNode* tmpNode; // not needed, but required for function
			#else
				ISceneNode* tmpNode; // not needed, but required for function
			#endif
			triangle3df tmpTri; // not needed, but required for function

			// Execute ray
			editor->GetState()->GetDevice()->getSceneManager()->getSceneCollisionManager()->getCollisionPoint(ray,editor->GetState()->plane_tri,wpos,tmpTri,tmpNode);

			// Snapping
			wpos -= vector3df(node->getPosition().X,node->getPosition().Y,node->getPosition().Z);

			if (editor->GetState()->Settings()->getSettingAsBool("snapping")==true){
				for (int i=0;i<15;i++){
					if (wpos.X > editor->snappers[i]-0.0313 && wpos.X < editor->snappers[i]+0.0313){
						wpos.X = editor->snappers[i];
					}
					if (wpos.Y > editor->snappers[i]-0.0313 && wpos.Y < editor->snappers[i]+0.0313){
						wpos.Y = editor->snappers[i];
					}
					if (wpos.Z > editor->snappers[i]-0.0313 && wpos.Z < editor->snappers[i]+0.0313){
						wpos.Z = editor->snappers[i];
					}
				}
			}

			// Do node limiting
			if (editor->GetState()->Settings()->getSettingAsBool("limiting")==true){
				// X Axis
				if (wpos.X < -0.5)
					wpos.X = -0.5;
				else if (wpos.X > 0.5)
					wpos.X = 0.5;

				// Y Axis
				if (wpos.Y < -0.5)
					wpos.Y = -0.5;
				else if (wpos.Y > 0.5)
					wpos.Y = 0.5;

				// Z Axis
				if (wpos.Z < -0.5)
					wpos.Z = -0.5;
				else if (wpos.Z > 0.5)
					wpos.Z = 0.5;
			}
			
			// Call required function
			if (type < CDR_XZ){
				box->resizeNodeBoxFace(editor->GetState(),type,wpos,editor->GetState()->keys[KEY_LCONTROL]==EKS_DOWN);
			}else{
				box->moveNodeBox(editor->GetState(),type,wpos);
			}
			node->remesh();

			editor->triggerCDRmoved();
	}

	vector3df pos;
	vector3df center = box->GetCenter();

	switch (type){
	case CDR_X_P:
		pos = center;
		pos.X = box->two.X;
		break;
	case CDR_X_N:
		pos = center;
		pos.X = box->one.X;
		break;
	case CDR_Y_P:
		pos = center;
		pos.Y = box->two.Y;
		break;
	case CDR_Y_N:
		pos = center;
		pos.Y = box->one.Y;
		break;
	case CDR_Z_P:
		pos = center;
		pos.Z = box->two.Z;
		break;
	case CDR_Z_N:
		pos = center;
		pos.Z = box->one.Z;
		break;
	case CDR_XZ:
		pos = center;
		break;
	case CDR_XY:
		pos = center;
		break;
	case CDR_ZY:
		pos = center;
		break;
	}

	pos.X+=node->getPosition().X;
	pos.Y+=node->getPosition().Y;
	pos.Z+=node->getPosition().Z;

	#if IRRLICHT_VERSION_MAJOR == 1 && IRRLICHT_VERSION_MINOR < 8
		vector2d<irr::s32> cpos = editor->GetState()->GetDevice()->getSceneManager()->getSceneCollisionManager()->getScreenCoordinatesFrom3DPosition(pos,editor->GetState()->GetDevice()->getSceneManager()->getActiveCamera());
	#else
		vector2d<irr::s32> cpos = editor->GetState()->GetDevice()->getSceneManager()->getSceneCollisionManager()->getScreenCoordinatesFrom3DPosition(pos,editor->GetState()->GetDevice()->getSceneManager()->getActiveCamera(),true);
	#endif

	position = cpos += offset.UpperLeftCorner;
	visible = true;
	return;
}

bool NBEditor::OnEvent(const irr::SEvent &event){
	if (event.EventType == EET_GUI_EVENT){
		if (event.GUIEvent.EventType == EGET_BUTTON_CLICKED){
			switch (event.GUIEvent.Caller->getID()){
			case GUI_PROJ_NEW_BOX:
				{
					Node* node = GetState()->project->GetCurrentNode();
					if (node){
						printf("Clicked add nb!\n");
						node->addNodeBox();
						load_ui();
					}
				}
				break;
			case GUI_PROJ_DELETE_BOX:
				{
					Node* node = GetState()->project->GetCurrentNode();
					IGUIListBox* lb = (IGUIListBox*) GetState()->Menu()->GetSideBar()->getElementFromId(ENB_GUI_MAIN_LISTBOX);	
					if (node && node->GetNodeBox(lb->getSelected())){
						printf("Clicked delete nb!\n");
						node->deleteNodebox(lb->getSelected());
						load_ui();
					}
				}
				break;
			case ENB_GUI_PROP_REVERT:
				{
					fillProperties();
				}
				break;
			case ENB_GUI_PROP_UPDATE:
				{

					IGUIElement* prop = GetState()->Menu()->GetSideBar()->getElementFromId(ENB_GUI_PROP);

					if (!prop)
						return false;

					Node* node = GetState()->project->GetCurrentNode();

					if (!node)
						return false;

					NodeBox* nb = node->GetCurrentNodeBox();

					if (!nb)	
						return false;

					try{
						nb->name = prop->getElementFromId(ENB_GUI_PROP_NAME)->getText();
						#if IRRLICHT_VERSION_MAJOR == 1 && IRRLICHT_VERSION_MINOR < 8
						nb->name.replace(' ','_');
						#else
						nb->name = nb->name.replace(' ','_');
						#endif
						nb->one.X = wcstod(prop->getElementFromId(ENB_GUI_PROP_X1)->getText(),NULL);
						nb->one.Y = wcstod(prop->getElementFromId(ENB_GUI_PROP_Y1)->getText(),NULL);
						nb->one.Z = wcstod(prop->getElementFromId(ENB_GUI_PROP_Z1)->getText(),NULL);
						nb->two.X = wcstod(prop->getElementFromId(ENB_GUI_PROP_X2)->getText(),NULL);
						nb->two.Y = wcstod(prop->getElementFromId(ENB_GUI_PROP_Y2)->getText(),NULL);
						nb->two.Z = wcstod(prop->getElementFromId(ENB_GUI_PROP_Z2)->getText(),NULL);
						node->remesh();
						load_ui();
					}catch(void* e){
						GetState()->GetDevice()->getGUIEnvironment()->addMessageBox(L"Update failed",L"Please check that the properties contain only numbers.");
					}
				}
				break;
			}
		}else if (event.GUIEvent.EventType == EGET_LISTBOX_CHANGED){
			Node* node = GetState()->project->GetCurrentNode();
			IGUIListBox* lb = (IGUIListBox*) GetState()->Menu()->GetSideBar()->getElementFromId(ENB_GUI_MAIN_LISTBOX);	
			if (node && lb && node->GetNodeBox(lb->getSelected())){
				node->select(lb->getSelected());
				load_ui();
			}
					
		}
	}

	return false;
}
