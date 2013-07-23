#include "common.h"

#ifndef _NODEBOX_H_INCLUDED_
#define _NODEBOX_H_INCLUDED_

class NodeBox
	{
	public:
		NodeBox(){};

		NodeBox(core::stringw _name, vector3df _one, vector3df _two)
		:name(_name),one(_one),two(_two),model(NULL){};

		~NodeBox(){
			if (model)
				model->remove();
		}
		irr::core::vector3df one;
		irr::core::vector3df two;
		core::stringw name;

		irr::scene::IMeshSceneNode* model;

		irr::core::vector3df GetCenter(){
			return vector3df(
				one.X + ((two.X - one.X)/2),
				one.Y + ((two.Y - one.Y)/2),
				one.Z + ((two.Z - one.Z)/2)
			);
		}

		irr::core::vector3df GetScale(){
			return vector3df(
				(two.X - one.X)/2,
				(two.Y - one.Y)/2,
				(two.Z - one.Z)/2
			);
		}
	};

#endif