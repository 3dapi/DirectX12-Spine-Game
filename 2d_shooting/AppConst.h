#pragma once
#ifndef __APPCONST_H__
#define __APPCONST_H__

#include <vector>
#include <string>
using std::vector;
using std::string;

enum EAPP_CONST
{
	EAPP_FRAME_RESOURCE_CHAIN_NUMBER	= 2,		// Cycle through the circular frame resource array.
	EAPP_DESC_HEAP_SIZE					= 1024,		// Descriptor heap size
};

enum EAPP_ATTRIB
{
	EAPP_ATT_WIN_HWND					= 0x0101,	// window handle
	EAPP_ATT_WIN_HINST					,			//
	EAPP_ATT_XTK_SPRITE					,			// Sprite Batch
	EAPP_ATT_XTK_GRAPHIC_MEM			,			//
	EAPP_ATT_XTK_DESC_HEAP				,			//
	EAPP_ATT_XTK_BATCH					,			//
	EAPP_ATT_CUR_CB						= 0x0301,	// current const buffer
};
enum EAPP_CMD
{
	EAPP_CMD_CHANGE_SCENE				= 0x4001,	// change scene
};

enum EAPP_SCENE
{
	EAPP_SCENE_NONE		= 0,
	EAPP_SCENE_BEGIN	= 1,
	EAPP_SCENE_LOBBY	,
	EAPP_SCENE_PLAY		,
	EAPP_SCENE_END		,
	EAPP_SCENE_COUNT	,

	EAPP_SCENE_MESH		= 0x1001,
	EAPP_SCENE_XTK		,
};

enum class EAPP_CHAR_STATE : int
{
	ESTATE_CHAR_NONE		= 0,
	ESTATE_CHAR_MOVE		,
	ESTATE_CHAR_DYING		,
	ESTATE_CHAR_DEATH		,
};

inline static const vector<string> EMODEL_SHIP
{
	"space_shooter/Ship_2_C_Small",		// ship1
	"space_shooter/Ship_2_D_Small",		// ship2
	"space_shooter/Ship_2_C_Medium",	// ship1 for ui
	"space_shooter/Ship_2_D_Medium",	// ship2 for ui
};

inline static const vector<string> EMODEL_DRONE
{
	"space_shooter/Pickup_1_A_Small",
	"space_shooter/Pickup_1_B_Small",
	"space_shooter/Pickup_1_C_Small",
	"space_shooter/Pickup_1_D_Small",
	"space_shooter/Pickup_1_E_Small",

	"space_shooter/Pickup_2_A_Small",
	"space_shooter/Pickup_2_B_Small",
	"space_shooter/Pickup_2_C_Small",
	"space_shooter/Pickup_2_D_Small",
	"space_shooter/Pickup_2_E_Small",
};

inline static const vector<string> EMODEL_MISSILE
{
	"space_shooter/Missile_A_Small",
	"space_shooter/Missile_B_Small",
	"space_shooter/Missile_C_Small",
	"space_shooter/Missile_D_Small",
	"space_shooter/Missile_E_Small",
};

inline static const vector<string> EMODEL_BULLET
{
	"bullet-red",
	"bullet-green",
	"bullet-yellow",
	"bullet-cyan",
};

inline static const vector<string> EMODEL_BOSS
{
	"space_shooter/Boss_1_A_Small",
	"space_shooter/Boss_1_B_Small",
};

inline static const vector<string> EMODEL_BG
{
	"vertical_shooting/Background Grid",
};

#endif __APPCONST_H__