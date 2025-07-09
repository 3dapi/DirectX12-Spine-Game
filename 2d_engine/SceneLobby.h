#pragma once
#ifndef _SceneLobby_H_
#define _SceneLobby_H_

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
#include "RenderSpine.h"
#include "UiLobby.h"


using namespace std;
using namespace DirectX;
using Microsoft::WRL::ComPtr;
using namespace G2;

class SceneLobby: public G2::IG2Scene
{
protected:
	vector<unique_ptr<RenderSpine> >	m_char	{ 2 };
	UiLobby*							m_pUi	{};

public:
	SceneLobby();
	virtual ~SceneLobby();

	// IG2Scene
	int		Type()						override { return EAPP_SCENE::EAPP_SCENE_LOBBY; }
	int		Init(const std::any& ={})	override;
	int		Destroy()					override;
	int		Update(const std::any& t)	override;
	int		Render()					override;
	int		Notify(const std::string& name, const std::any& t)	override;

protected:
	void	CheckSelectCharacter(const ::POINT& mousePos);

};

#endif
