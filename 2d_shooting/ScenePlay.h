
#pragma once
#ifndef _ScenePlay_H_
#define _ScenePlay_H_

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
	GamePlayer*				m_mainPlayer	{};
	vector<GameMob*>		m_vecMob		;
	vector<uint8_t>			m_keyEvent		;
	class UiBase*			m_pUi			{};
	bool					m_stageComplete		{false};
	bool					m_stageChangeing	{false};

public:
	ScenePlay();
	virtual ~ScenePlay();

	// IG2Scene
	int		Type()						override { return (int)EAPP_SCENE::EAPP_SCENE_PLAY; }
	int		Init(const std::any& ={})	override;
	int		Destroy()					override;
	int		Update(const std::any& t)	override;
	int		Render()					override;
	int		Notify(const std::string& name, const std::any& t)	override;

	int		CreateMainPlayerModel();
	int		StageInit();
	int		StageChange(const GameTimer& gt);
	int		StageChangingUpdate(const GameTimer& gt);
	int		StageComplete();
	int		SetupMobMovemoent(GameMob*);
};

#endif
