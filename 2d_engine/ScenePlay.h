
#pragma once
#ifndef _ScenePlay_H_
#define _ScenePlay_H_

#include <map>
#include <vector>
#include <any>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <wrl/client.h>
#include <spine/spine.h>

#include "Common/G2.Constants.h"
#include "Common/G2.Geometry.h"
#include "common/G2.ConstantsWin.h"
#include "common/G2.FactorySpine.h"
#include "common/G2.Util.h"
#include "AppCommon.h"
#include "AppCommonXTK.h"
#include "GameInfo.h"

using namespace std;
using namespace DirectX;
using Microsoft::WRL::ComPtr;
using namespace G2;


class ScenePlay: public G2::IG2Scene
{
protected:
	std::vector<GameMob*>		m_vecMob{ GameInfo::MAX_MOB };
public:
	ScenePlay();
	virtual ~ScenePlay();

	// IG2Scene
	int		Type()						override { return EAPP_SCENE::EAPP_SCENE_PLAY; }
	int		Init(const std::any& ={})	override;
	int		Destroy()					override;
	int		Update(const std::any& t)	override;
	int		Render()					override;
	int		Notify(const std::string& name, const std::any& t)	override;

	void	CreatePlayerCharModel();
	void	CreateMobCharModel();
	void	DeletePlayCharModel();
	void	DeleteMobCharModel();
};

#endif
