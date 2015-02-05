#ifndef COMMON_HPP_INCLUDED
#define COMMON_HPP_INCLUDED
#include <irrlicht.h>
#include <iostream>
using namespace irr;
using namespace core;
using namespace scene;
using namespace gui;
using namespace video;

// Enums
enum FileParserType
{
	EFPT_SAVE_PROJ,
	EFPT_LOAD_PROJ,
	EFPT_IMPORT
};

enum CubeSide
{
	ECS_TOP = 0,
	ECS_BOTTOM,
	ECS_RIGHT,
	ECS_LEFT,
	ECS_BACK,
	ECS_FRONT
};

enum EAxis
{
	EAX_X,
	EAX_Y,
	EAX_Z
};

enum Viewport
{
	VIEW_NONE = -1,
	VIEW_TL,
	VIEW_TR,
	VIEW_BL,
	VIEW_BR
};

enum ViewportType
{
	VIEWT_PERS = 0,
	VIEWT_FRONT,
	VIEWT_LEFT,
	VIEWT_TOP,
	VIEWT_BACK,
	VIEWT_RIGHT,
	VIEWT_BOTTOM
};

enum CDR_TYPE
{
	CDR_L = 0,
	CDR_R,
	CDR_U,
	CDR_D,
	CDR_M
};

enum CDRType
{
	CDR_NONE = 0,
	CDR_X_P,
	CDR_X_N,
	CDR_Y_P,
	CDR_Y_N,
	CDR_Z_P,
	CDR_Z_N,
	CDR_XZ,
	CDR_XY,
	CDR_ZY
};

// Defines
#include "conf_cmake.hpp"
#define EDITOR_VERSION 1
#define EDITOR_PARSER 1

#endif
