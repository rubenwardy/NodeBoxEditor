#ifndef OBJ_HPP_INCLUDED
#define OBJ_HPP_INCLUDED

#include "../Node.hpp"

void nodeBoxObj(int order, NodeBox *box, std::string &vertices, std::string &faces);
std::string nodeToObj(Node *node, std::string filenameNoExt = "out");

#endif
