#include "NBEditor.h"

NBEditor::NBEditor(EditorState* st)
:EditorMode(st),current(-1)
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
	sidebar->setText(L"Node boxes");
	IGUIStaticText* t = guienv->addStaticText(L"No node selected",rect<s32>(20,30,140,100),false,true,sidebar,GUI_SIDEBAR_LABEL);
	
	if (t)
		t->setVisible(false);
	
	IGUIListBox* lb = guienv->addListBox(rect<s32>(20,30,230,128),sidebar,GUI_SIDEBAR_LISTBOX,true);

	if (lb){
		lb->setVisible(false);
		IGUIButton* b = guienv->addButton(rect<s32>(20-20,130-30,70-20,155-30),lb,GUI_PROJ_NEW_BOX,L"+",L"Add a node box");
		IGUIButton* c = guienv->addButton(rect<s32>(80-20,130-30,130-20,155-30),lb,GUI_PROJ_DELETE_BOX,L"-",L"Delete node box");
		b->setNotClipped(true);
		c->setNotClipped(true);
	}

	load_ui();
}

void NBEditor::load_ui(){
	IGUIStaticText* sidebar = GetState()->Menu()->GetSideBar();
	IGUIEnvironment* guienv = GetState()->GetDevice()->getGUIEnvironment();
	Node* node = GetState()->project->GetCurrentNode();
	if (!node){
		sidebar->getElementFromId(GUI_SIDEBAR_LABEL)->setVisible(true);
		sidebar->getElementFromId(GUI_SIDEBAR_LISTBOX)->setVisible(false);
	}else{
		IGUIListBox* lb = (IGUIListBox*) sidebar->getElementFromId(GUI_SIDEBAR_LISTBOX);		
		sidebar->getElementFromId(GUI_SIDEBAR_LABEL)->setVisible(false);

		if (lb){
			lb->clear();
			lb->setVisible(true);
			for (int i = 0;i<NODEB_MAX;i++){
				NodeBox* box = node->GetNodeBox(i);
				if (box){
					size_t origsize = strlen(box->name.c_str()) + 1;
					static wchar_t wcstring[1024];
					mbstowcs(wcstring, box->name.c_str(), origsize);
					wcscat(wcstring, L"");
					lb->addItem(wcstring);
				}
			}
			lb->setSelected(lb->getListItem(node->GetId()));
		}
	}
}

void NBEditor::unload(){
}

void NBEditor::update(){
}

void NBEditor::draw(irr::video::IVideoDriver* driver){
	if (wasmd && !GetState()->mousedown){
		current = -1;		
	}

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
				driver->getTexture("media/gui_scale.png"),
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
					IGUIListBox* lb = (IGUIListBox*) GetState()->Menu()->GetSideBar()->getElementFromId(GUI_SIDEBAR_LISTBOX);	
					if (node && node->GetNodeBox(lb->getSelected())){
						printf("Clicked delete nb!\n");
						node->deleteNodebox(lb->getSelected());
						load_ui();
					}
				}
				break;
			}
		}else if (event.GUIEvent.EventType == EGET_LISTBOX_CHANGED){
			Node* node = GetState()->project->GetCurrentNode();
			IGUIListBox* lb = (IGUIListBox*) GetState()->Menu()->GetSideBar()->getElementFromId(GUI_SIDEBAR_LISTBOX);	
			if (node && lb && node->GetNodeBox(lb->getSelected())){
				node->select(lb->getSelected());
				load_ui();
			}
					
		}
	}

	return false;
}