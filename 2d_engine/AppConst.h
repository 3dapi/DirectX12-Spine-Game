#pragma once
#ifndef __APPCONST_H__
#define __APPCONST_H__

#include <any>
#include <array>
#include <memory>
#include <vector>
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12/d3dx12.h>

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace G2;

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
	EAPP_SCENE_SPINE	,
};

enum class EAPP_CHAR_STATE : int
{
	ESTATE_CHAR_NONE		= 0,
	ESTATE_CHAR_IDLE		= 1,
	ESTATE_CHAR_MOVE		,
	ESTATE_CHAR_ATTACK		,
	ESTATE_CHAR_DYING		,
	ESTATE_CHAR_DEATH		,
};

enum class EAPP_MODEL : int
{
	EMODEL_NONE		= 0,		// nothing
	EMODEL_KNIGHT	= 1,		// hero
	EMODEL_BOY		,			// spine boy
	EMODEL_RAPTOR	,			// raptor
	EMODEL_GOBLIN	,			// goblin
	EMODEL_ALIEN	,			// alien
	EMODEL_STMAN	,			// stretchyman
	EMODEL_COUNT	,
};

#endif __APPCONST_H__