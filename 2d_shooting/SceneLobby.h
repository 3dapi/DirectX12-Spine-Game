#pragma once
#ifndef _SceneLobby_H_
#define _SceneLobby_H_

#include "Common/G2.Geometry.h"
#include "common/G2.Util.h"
#include "AppCommon.h"
#include "AppCommonXTK.h"

using namespace std;
using namespace DirectX;
using Microsoft::WRL::ComPtr;
using namespace G2;

class SceneLobby: public G2::IG2Scene
{
protected:
	class UiDrawable*						m_pUi	{};

public:
	SceneLobby();
	virtual ~SceneLobby();

	// IG2Scene
	int		Type()						override { return (int)EAPP_SCENE::EAPP_SCENE_LOBBY; }
	int		Init(const std::any& ={})	override;
	int		Destroy()					override;
	int		Update(const std::any& t)	override;
	int		Render()					override;
	int		Notify(const std::string& name, const std::any& t)	override;

protected:
	void	CheckChooseShip(const ::POINT& mousePos);

};

#endif
