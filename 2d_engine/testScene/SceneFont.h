
#pragma once
#ifndef _SceneFont_H_
#define _SceneFont_H_

#include <any>
#include <map>
#include <memory>
#include <tuple>
#include <vector>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <wrl/client.h>

#include "Common/G2.Constants.h"
#include "Common/G2.Geometry.h"
#include "common/G2.ConstantsWin.h"
#include "common/G2.FactoryTexture.h"
#include "common/G2.Util.h"
#include "AppCommon.h"
#include "AppCommonXTK.h"
#include "UiBase.h"

using namespace std;
using namespace DirectX;
using Microsoft::WRL::ComPtr;
using namespace G2;


class SceneFont: public G2::IG2Scene
{
protected:
	ComPtr<ID3D12DescriptorHeap>	m_srvHeapUI	{};
	map<string, UI_TEXTURE>			m_uiTex		;
protected:
public:
	SceneFont();
	virtual ~SceneFont();

	// IG2Scene
	int		Type()						override { return (int)EAPP_SCENE::EAPP_SCENE_SPINE; }
	int		Init(const std::any& ={})	override;
	int		Destroy()					override;
	int		Update(const std::any& t)	override;
	int		Render()					override;
	int		Notify(const std::string&, const std::any&)	override;
};

#endif
