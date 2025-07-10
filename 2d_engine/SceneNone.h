
#pragma once
#ifndef _SceneNone_H_
#define _SceneNone_H_

#include <map>
#include <vector>
#include <any>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <wrl/client.h>
#include "Common/G2.Constants.h"
#include "common/G2.ConstantsWin.h"
#include "common/G2.Util.h"
#include "AppCommon.h"
#include "AppCommonXTK.h"

using namespace std;
using namespace DirectX;
using Microsoft::WRL::ComPtr;
using namespace G2;


class SceneNone: public G2::IG2Scene
{
protected:
protected:
public:
	SceneNone();
	virtual ~SceneNone();

	// IG2Scene
	int		Type()						override { return (int)EAPP_SCENE::EAPP_SCENE_NONE; }
	int		Init(const std::any& ={})	override;
	int		Destroy()					override;
	int		Update(const std::any& t)	override;
	int		Render()					override;
	int		Notify(const std::string& name, const std::any& t)	override;
};

#endif
