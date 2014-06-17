// GENERATED CODE
// Node Box Editor, version 0.6.4 - Stone
// Project name: test
// Copy and paste the following code snippets
//   into the correct place in src/content_nodebox.cpp

void content_nodebox_Node1(ContentFeatures *f)
{
	// NodeBox1
	f->setNodeBox(core::aabbox3d<f32>(
		-0.5*BS,
		-0.5*BS,
		-0.5*BS,
		-0.125*BS,
		0.5*BS,
		0.5*BS
	));

	// NodeBox2
	f->addNodeBox(core::aabbox3d<f32>(
		0.125*BS,
		-0.5*BS,
		-0.5*BS,
		0.5*BS,
		0.5*BS,
		0.5*BS
	));

}

