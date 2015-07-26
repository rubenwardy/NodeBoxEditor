#ifndef NODEBOX_HPP_INCLUDED
#define NODEBOX_HPP_INCLUDED

#include <string>
#include "../common.hpp"
#include "../EditorState.hpp"
#include "media.hpp"

class EditorState;
class NodeBox
{
public:
	NodeBox() {};

	NodeBox(const std::string & name, const vector3df & one, const vector3df & two) :
		name(name), one(one), two(two), model(NULL), rebuild_needed(true)
	{}

	void removeMesh(IVideoDriver *driver);

	irr::core::vector3df one;
	irr::core::vector3df two;
	bool rebuild_needed;
	std::string name;
	irr::scene::IMeshSceneNode* model;

	irr::core::vector3df GetCenter()
	{
		return vector3df(
			one.X + ((two.X - one.X) / 2),
			one.Y + ((two.Y - one.Y) / 2),
			one.Z + ((two.Z - one.Z) / 2)
		);
	}

	irr::core::vector3df GetScale()
	{
		return vector3df(
			(two.X - one.X) / 2,
			(two.Y - one.Y) / 2,
			(two.Z - one.Z) / 2
		);
	}

	// Transformations
	void moveFace(EditorState* editor, ECDR_DIR type,
			vector3df position, bool both);
	void move(EditorState* editor, ECDR_DIR type, vector3df position,
			const int snap_res=0);
	void rotate(EAxis axis);
	void flip(EAxis axis);

	// Create the mesh for the nodebox, store is in this->model.
	//
	// Only runs if rebuild_needed is true.
	void buildMesh(EditorState* editor, vector3di nd_position,
			IrrlichtDevice* device, Media::Image* images[6], bool force = false);
};

#endif
