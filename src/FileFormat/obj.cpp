#include "obj.hpp"
#include <string>
#include <sstream>
#include "../util/string.hpp"
#include <algorithm>

void nodeBoxObj(int order, NodeBox *box, std::string &vertices, std::string &faces)
{
	std::ostringstream vert;
	vert << "v " << -box->one.X << " " << box->one.Y << " " << box->one.Z << "\n";
	vert << "v " << -box->one.X << " " << box->one.Y << " " << box->two.Z << "\n";
	vert << "v " << -box->one.X << " " << box->two.Y << " " << box->two.Z << "\n";
	vert << "v " << -box->one.X << " " << box->two.Y << " " << box->one.Z << "\n";

	vert << "v " << -box->two.X << " " << box->one.Y << " " << box->one.Z << "\n";
	vert << "v " << -box->two.X << " " << box->one.Y << " " << box->two.Z << "\n";
	vert << "v " << -box->two.X << " " << box->two.Y << " " << box->two.Z << "\n";
	vert << "v " << -box->two.X << " " << box->two.Y << " " << box->one.Z << "\n";
	vertices = vert.str();

	static const int faces_val[] = {
		1, 2, 3, 4,
		5, 6, 7, 8,
		1, 4, 8, 5,
		2, 3, 7, 6,
		1, 2, 6, 5,
		4, 3, 7, 8
	};

	std::ostringstream fac;
	std::string name(box->name);
	std::transform(name.begin(), name.end(), name.begin(), ::tolower);
	str_replace(std::string(name.c_str(), name.size()), ' ', '_');
	fac << "g " << name << "\n";
	for (int i = 0; i < 6; i++) {
		fac << "f "
		    << faces_val[i*4]     + order*8 << " "
		    << faces_val[i*4 + 1] + order*8 << " "
		    << faces_val[i*4 + 2] + order*8 << " "
		    << faces_val[i*4 + 3] + order*8 << "\n";
	}
	faces = fac.str();
}

std::string nodeToObj(Node *node, std::string filenameNoExt)
{
	std::string vertices = "";
	std::string faces = "";
	int count = 0;
	for (std::vector<NodeBox*>::const_iterator it = node->boxes.begin();
			it != node->boxes.end();
			++it) {
		NodeBox* box = *it;
		std::string vert = "";
		std::string fac = "";
		nodeBoxObj(count, box, vert, fac);
		vertices += vert;
		faces += fac;
		count++;
	}

	std::ostringstream res;
	res << "mtllib " << filenameNoExt.c_str() << ".mtl" << std::endl;
	res << "o converted_out" << std::endl;
	res << vertices.c_str();
	res << "usemtl none" << std::endl;
	res << "s off" << std::endl;
	res << faces.c_str();
	return res.str();
}
