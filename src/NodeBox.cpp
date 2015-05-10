#include "NodeBox.hpp"

void NodeBox::resizeNodeBoxFace(EditorState* editor, CDRType type, vector3df position, bool both)
{
	vector3df before_one = one;
	vector3df before_two = two;
	switch(type) {
	case CDR_X_P:
		if (both) {
			f32 new_opp = one.X - (position.X - two.X);

			if (editor->settings->getBool("limiting")==true){
				if (new_opp > 0.5 || new_opp < -0.5)
					return;
			}

			one.X = new_opp;
		}
		two.X = position.X;
		break;
	case CDR_X_N:
		if (both) {
			f32 new_opp = two.X - (position.X - one.X);

			if (editor->settings->getBool("limiting")==true){
				if (new_opp > 0.5 || new_opp < -0.5)
					return;
			}

			two.X = new_opp;
		}
		one.X = position.X;
		break;
	case CDR_Y_P:
		if (both) {
			f32 new_opp = one.Y - (position.Y - two.Y);

			if (editor->settings->getBool("limiting")==true){
				if (new_opp > 0.5 || new_opp < -0.5)
					return;
			}

			one.Y = new_opp;
		}
		two.Y = position.Y;
		break;
	case CDR_Y_N:
		if (both) {
			f32 new_opp = two.Y - (position.Y - one.Y);

			if (editor->settings->getBool("limiting")==true){
				if (new_opp > 0.5 || new_opp < -0.5)
					return;
			}

			two.Y = new_opp;
		}
		one.Y = position.Y;
		break;
	case CDR_Z_P:
		if (both) {
			f32 new_opp = one.Z - (position.Z - two.Z);

			if (editor->settings->getBool("limiting")==true){
				if (new_opp > 0.5 || new_opp < -0.5)
					return;
			}

			one.Z = new_opp;
		}
		two.Z = position.Z;
		break;
	case CDR_Z_N:
		if (both) {
			f32 new_opp = two.Z - (position.Z - one.Z);

			if (editor->settings->getBool("limiting")==true){
				if (new_opp > 0.5 || new_opp < -0.5)
					return;
			}

			two.Z = new_opp;
		}
		one.Z = position.Z;
		break;
	}

	if (before_one != one || before_two != two)
		rebuild_needed = true;
}

void NodeBox::moveNodeBox(EditorState* editor, CDRType type, vector3df position)
{
	vector3df new_one = one;
	vector3df new_two = two;
	vector3df move_dist = vector3df(0,0,0);

	switch(type) {
	case CDR_XZ:
		move_dist = vector3df(
			position.X - GetCenter().X,
			0,
			position.Z - GetCenter().Z
		);
		break;
	case CDR_XY:
		move_dist = vector3df(
			position.X - GetCenter().X,
			position.Y - GetCenter().Y,
			0
		);
		break;
	case CDR_ZY:
		move_dist = vector3df(
			0,
			position.Y - GetCenter().Y,
			position.Z - GetCenter().Z
		);
		break;
	}

	if (move_dist.X != 0){
		if (
				(new_one.X + move_dist.X <= 0.5 &&
				new_one.X + move_dist.X >= -0.5 &&
				new_two.X + move_dist.X <= 0.5 &&
				new_two.X + move_dist.X >= -0.5) ||
				!editor->settings->getBool("limiting")) {
			new_one.X += move_dist.X;
			new_two.X += move_dist.X;
		}
	}

	if (move_dist.Y != 0) {
		if (
				(new_one.Y + move_dist.Y <= 0.5 &&
				new_one.Y + move_dist.Y >= -0.5 &&
				new_two.Y + move_dist.Y <= 0.5 &&
				new_two.Y + move_dist.Y >= -0.5) ||
				!editor->settings->getBool("limiting")) {
			new_one.Y += move_dist.Y;
			new_two.Y += move_dist.Y;
		}
	}

	if (move_dist.Z != 0) {
		if (
				(new_one.Z + move_dist.Z <= 0.5 &&
				new_one.Z + move_dist.Z >= -0.5 &&
				new_two.Z + move_dist.Z <= 0.5 &&
				new_two.Z + move_dist.Z >= -0.5) ||
				!editor->settings->getBool("limiting")) {
			new_one.Z += move_dist.Z;
			new_two.Z += move_dist.Z;
		}
	}

	one = new_one;
	two = new_two;

	if (move_dist != vector3df(0, 0, 0))
		rebuild_needed = true;
}

ITexture* darken(IVideoDriver* driver, IImage* image, f32 amt, const char *name)
{
	if (image == NULL)
		return NULL;

	core::dimension2d<u32> dim = image->getDimension();
	IImage* image2 = driver->createImage(image->getColorFormat(), dim);
	image->copyTo(image2);

	for(u32 y=0; y<dim.Height; y++) {
		for(u32 x=0; x<dim.Width; x++) {
			video::SColor c = image2->getPixel(x,y);
			c.setRed((u32)(amt * c.getRed() + 0.5f));
			c.setGreen((u32)(amt * c.getGreen() + 0.5f));
			c.setBlue((u32)(amt * c.getBlue() + 0.5f));
			image2->setPixel(x, y, c);
		}
	}

	ITexture *retval = driver->addTexture(name, image2);
	image2->drop();
	return retval;
}

void NodeBox::buildNode(EditorState* editor, vector3di nd_position, IrrlichtDevice* device, Media::Image* images[6])
{
	if (!rebuild_needed)
		return;

	rebuild_needed = false;

	video::IVideoDriver* driver = device->getVideoDriver();
	static Media::Image *def = new Media::Image("default", driver->createImageFromFile("media/texture_box.png"));

	Media::Image *copied[6];
	for (int i = 0; i < 6; i++) {
		copied[i] = images[i];
		if (!copied[i])
			copied[i] = def;
	}
	ISceneManager* smgr = device->getSceneManager();
	if (model) {
		for (int i = 0; i < model->getMaterialCount(); i++)
			driver->removeTexture(model->getMaterial(i).getTexture(0));

		model->remove();
		model = NULL;
	}
	vector3df position = vector3df(
			nd_position.X + one.X + ((two.X - one.X) / 2),
			nd_position.Y + one.Y + ((two.Y - one.Y) / 2),
			nd_position.Z + one.Z + ((two.Z - one.Z) / 2)
		);
	vector3df size = vector3df(
			two.X - one.X,
			two.Y - one.Y,
			two.Z - one.Z
		);

	// init variables
	f32 cubeSize = 1.f;
	video::SColor cubeColour(255, 255, 255, 255);

	// Initialise buffers
	SMeshBuffer *buffer = new SMeshBuffer();
	SMeshBuffer *buffer2 = new SMeshBuffer();
	SMeshBuffer *buffer3 = new SMeshBuffer();
	SMeshBuffer *buffer4 = new SMeshBuffer();
	SMeshBuffer *buffer5 = new SMeshBuffer();
	SMeshBuffer *buffer6 = new SMeshBuffer();
	buffer->Indices.set_used(6);
	buffer2->Indices.set_used(6);
	buffer3->Indices.set_used(6);
	buffer4->Indices.set_used(6);
	buffer5->Indices.set_used(6);
	buffer6->Indices.set_used(6);
	u16 u[6] = {0,2,1, 0,3,2};
	for (s32 i=0; i<6; ++i) {
		buffer->Indices[i] = u[i];
		buffer2->Indices[i] = u[i];
		buffer3->Indices[i] = u[i];
		buffer4->Indices[i] = u[i];
		buffer5->Indices[i] = u[i];
		buffer6->Indices[i] = u[i];
	}

	// Init mesh
	SMesh * cubeMesh = new SMesh();

#define x0 -0.5f
#define x1 0.5f

	std::string lighting = editor->settings->get("lighting");

	// Front face
	vector2df topl((one.X + 0.5f), (-two.Y + 0.5f));
	vector2df btmr((two.X + 0.5f), (-one.Y + 0.5f));
	buffer->Vertices.set_used(4);
	buffer->Vertices[0] = video::S3DVertex(x0,x0,x0, -1,-1,-1, cubeColour, topl.X, btmr.Y);
	buffer->Vertices[1] = video::S3DVertex(x1,x0,x0, 1,-1,-1, cubeColour, btmr.X, btmr.Y);
	buffer->Vertices[2] = video::S3DVertex(x1,x1,x0, 1, 1,-1, cubeColour, btmr.X, topl.Y);
	buffer->Vertices[3] = video::S3DVertex(x0,x1,x0, -1, 1,-1, cubeColour, topl.X, topl.Y);
	buffer->BoundingBox.reset(0,0,0);
	ITexture *texture = NULL;
	if (lighting == "1" || lighting == "2")
		texture = darken(driver, copied[ECS_FRONT]->get(), 0.5f, copied[ECS_RIGHT]->name.c_str());
	else
		texture = driver->addTexture(copied[ECS_FRONT]->name.c_str(), copied[ECS_FRONT]->get());
	SMaterial mat = SMaterial();
	mat.setTexture(0, texture);
	buffer->Material = mat;
	cubeMesh->addMeshBuffer(buffer);
	buffer->drop();


	// Back face
	topl = vector2df((-two.X + 0.5f), (-two.Y + 0.5f));
	btmr = vector2df((-one.X + 0.5f), (-one.Y + 0.5f));
	buffer2->Vertices.set_used(4);
	buffer2->Vertices[0] = video::S3DVertex(x1,x0,x1, 1, -1, 1, cubeColour, topl.X, btmr.Y);
	buffer2->Vertices[1] = video::S3DVertex(x0,x0,x1, -1,-1, 1, cubeColour, btmr.X, btmr.Y);
	buffer2->Vertices[2] = video::S3DVertex(x0,x1,x1, -1, 1, 1, cubeColour, btmr.X, topl.Y);
	buffer2->Vertices[3] = video::S3DVertex(x1,x1,x1, 1, 1, 1, cubeColour, topl.X, topl.Y);
	buffer2->BoundingBox.reset(0,0,0);
	texture = NULL;
	if (lighting == "1" || lighting == "2")
		texture = darken(driver, copied[ECS_BACK]->get(), 0.5f, copied[ECS_BACK]->name.c_str());
	else
		texture = driver->addTexture(copied[ECS_BACK]->name.c_str(), copied[ECS_BACK]->get());
	mat = SMaterial();
	mat.setTexture(0, texture);
	buffer2->Material = mat;
	cubeMesh->addMeshBuffer(buffer2);
	buffer2->drop();


	// Left face
	topl = vector2df((-two.Z + 0.5f), (-two.Y + 0.5f));
	btmr = vector2df((-one.Z + 0.5f), (-one.Y + 0.5f));
	buffer3->Vertices.set_used(4);
	buffer3->Vertices[0] = video::S3DVertex(x0,x0,x1, -1,-1, 1, cubeColour, topl.X, btmr.Y);
	buffer3->Vertices[1] = video::S3DVertex(x0,x0,x0, -1,-1,-1, cubeColour, btmr.X, btmr.Y);
	buffer3->Vertices[2] = video::S3DVertex(x0,x1,x0, -1, 1,-1, cubeColour, btmr.X, topl.Y);
	buffer3->Vertices[3] = video::S3DVertex(x0,x1,x1, -1, 1, 1, cubeColour, topl.X, topl.Y);
	buffer3->BoundingBox.reset(0,0,0);
	texture = NULL;
	if (lighting == "1" || lighting == "2")
		texture = darken(driver, copied[ECS_LEFT]->get(), 0.7f, copied[ECS_LEFT]->name.c_str());
	else
		texture = driver->addTexture(copied[ECS_LEFT]->name.c_str(), copied[ECS_LEFT]->get());
	mat = SMaterial();
	mat.setTexture(0, texture);
	buffer3->Material = mat;
	cubeMesh->addMeshBuffer(buffer3);
	buffer3->drop();


	// Right face
	topl = vector2df((one.Z + 0.5f), (-two.Y + 0.5f));
	btmr = vector2df((two.Z + 0.5f), (-one.Y + 0.5f));
	buffer4->Vertices.set_used(4);
	buffer4->Vertices[0] = video::S3DVertex(x1,x0,x0,  1,-1,-1, cubeColour, topl.X, btmr.Y);
	buffer4->Vertices[1] = video::S3DVertex(x1,x0,x1,  1,-1, 1, cubeColour, btmr.X, btmr.Y);
	buffer4->Vertices[2] = video::S3DVertex(x1,x1,x1,  1, 1, 1, cubeColour, btmr.X, topl.Y);
	buffer4->Vertices[3] = video::S3DVertex(x1,x1,x0,  1, 1,-1, cubeColour, topl.X, topl.Y);
	buffer4->BoundingBox.reset(0,0,0);
	texture = NULL;
	if (lighting == "1" || lighting == "2")
		texture = darken(driver, copied[ECS_RIGHT]->get(), 0.7f, copied[ECS_RIGHT]->name.c_str());
	else
		texture = driver->addTexture(copied[ECS_RIGHT]->name.c_str(), copied[ECS_RIGHT]->get());
	mat = SMaterial();
	mat.setTexture(0, texture);
	buffer4->Material = mat;
	cubeMesh->addMeshBuffer(buffer4);
	buffer4->drop();


	// Top face
	topl = vector2df((one.X + 0.5f), (-two.Z + 0.5f));
	btmr = vector2df((two.X + 0.5f), (-one.Z + 0.5f));
	buffer5->Vertices.set_used(4);
	buffer5->Vertices[0] = video::S3DVertex(x0,x1,x0, -1, 1,-1, cubeColour, topl.X, btmr.Y);
	buffer5->Vertices[1] = video::S3DVertex(x1,x1,x0,  1, 1,-1, cubeColour, btmr.X, btmr.Y);
	buffer5->Vertices[2] = video::S3DVertex(x1,x1,x1,  1, 1, 1, cubeColour, btmr.X, topl.Y);
	buffer5->Vertices[3] = video::S3DVertex(x0,x1,x1, -1, 1, 1, cubeColour, topl.X, topl.Y);
	buffer5->BoundingBox.reset(0,0,0);
	texture = NULL;
	if (lighting == "1")
		texture = darken(driver, copied[ECS_TOP]->get(), 0.7f, copied[ECS_TOP]->name.c_str());
	else
		texture = driver->addTexture(copied[ECS_TOP]->name.c_str(), copied[ECS_TOP]->get());
	mat = SMaterial();
	mat.setTexture(0, texture);
	buffer5->Material = mat;
	cubeMesh->addMeshBuffer(buffer5);
	buffer5->drop();


	// Bottom face
	topl = vector2df((-one.X + 0.5f), (-one.Z + 0.5f));
	btmr = vector2df((-two.X + 0.5f), (-two.Z + 0.5f));
	buffer6->Vertices.set_used(4);
	buffer6->Vertices[0] = video::S3DVertex(x0,x0,x1, -1,-1, 1, cubeColour, topl.X, btmr.Y);
	buffer6->Vertices[1] = video::S3DVertex(x1,x0,x1,  1,-1, 1, cubeColour, btmr.X, btmr.Y);
	buffer6->Vertices[2] = video::S3DVertex(x1,x0,x0,  1,-1,-1, cubeColour, btmr.X, topl.Y);
	buffer6->Vertices[3] = video::S3DVertex(x0,x0,x0, -1,-1,-1, cubeColour, topl.X, topl.Y);
	buffer6->BoundingBox.reset(0,0,0);
	if (lighting == "1" || lighting == "2")
		texture = darken(driver, copied[ECS_BOTTOM]->get(), 0.4f, copied[ECS_BOTTOM]->name.c_str());
	else
		texture = driver->addTexture(copied[ECS_BOTTOM]->name.c_str(), copied[ECS_BOTTOM]->get());
	mat = SMaterial();
	mat.setTexture(0, texture);
	buffer6->Material = mat;
	cubeMesh->addMeshBuffer(buffer6);
	buffer6->drop();


	// Create scene node from mesh
	model = smgr->addMeshSceneNode(cubeMesh);
	cubeMesh->drop();
	model->setPosition(position);
	model->setScale(size);
	model->setMaterialFlag(EMF_BILINEAR_FILTER, false);
	model->setMaterialFlag(EMF_LIGHTING, false);
}

void NodeBox::rotate(EAxis axis)
{
	switch (axis) {
	case EAX_X: {
		f32 tmp = one.X;
		one.X = one.Y;
		one.Y = -tmp;
		tmp = two.X;
		two.X = two.Y;
		two.Y = -tmp;
		break;
	}
	case EAX_Y: {
		f32 tmp = one.X;
		one.X = one.Z;
		one.Z = -tmp;
		tmp = two.X;
		two.X = two.Z;
		two.Z = -tmp;
		break;
	}
	case EAX_Z: {
		f32 tmp = one.Z;
		one.Z = one.Y;
		one.Y = -tmp;
		tmp = two.Z;
		two.Z = two.Y;
		two.Y = -tmp;
		break;
	}};

	// Check relative sizes
	if (one.X > two.X) {
		f32 tmp = one.X;
		one.X = two.X;
		two.X = tmp;
	}
	if (one.Y > two.Y) {
		f32 tmp = one.Y;
		one.Y = two.Y;
		two.Y = tmp;
	}
	if (one.Z > two.Z) {
		f32 tmp = one.Z;
		one.Z = two.Z;
		two.Z = tmp;
	}
}

void NodeBox::flip(EAxis axis)
{
	switch (axis) {
	case EAX_X: {
		f32 tmp = one.X;
		one.X = -two.X;
		two.X = -tmp;
		break;
	}
	case EAX_Y: {
		f32 tmp = one.Y;
		one.Y = -two.Y;
		two.Y = -tmp;
		break;
	}
	case EAX_Z: {
		f32 tmp = one.Z;
		one.Z = -two.Z;
		two.Z = -tmp;
		break;
	}};

	// Check relative sizes
	if (one.X > two.X) {
		std::cerr << "This shouldn't happen! (X)" << std::endl;
		f32 tmp = one.X;
		one.X = two.X;
		two.X = tmp;
	}
	if (one.Y > two.Y) {
		std::cerr << "This shouldn't happen! (Y)" << std::endl;
		f32 tmp = one.Y;
		one.Y = two.Y;
		two.Y = tmp;
	}
	if (one.Z > two.Z) {
		std::cerr << "This shouldn't happen! (Z)" << std::endl;
		f32 tmp = one.Z;
		one.Z = two.Z;
		two.Z = tmp;
	}
}
