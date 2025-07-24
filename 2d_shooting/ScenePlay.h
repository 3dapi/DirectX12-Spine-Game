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
	ComPtr<ID3D12DescriptorHeap>	m_srvHeap	{};
	map<string, struct UI_TEXTURE>	m_srvTex	;

	GamePlayer*				m_mainPlayer	{};
	vector<EnemyDrone*>		m_vecMob		;
	EnemyDrone*				m_vecMobBoss	;

	class UiDrawable*		m_pUi			{};
	class UiDrawable*		m_pUiBg			{};
	bool					m_stageComplete	{false};
	bool					m_stageChanging	{false};

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
	int		SetupMobMovemoent(EnemyDrone*);

	void	PositionToOtho(XMFLOAT2& pos);
};

#endif
