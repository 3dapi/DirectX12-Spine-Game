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
public:
	enum class PLAY_STATE: int
	{
		BEGIN	= 0,
		LOW		,
		MIDDLE	,
		HI		,
		BOSS	,
		END		,
	};
protected:
	inline static const int			MAX_DRONE			{1000};
	inline static const int			MAX_BULLET_PLAYER	{1000};
	inline static const int			MAX_BULLET_ENEMY	{1000};

	ComPtr<ID3D12DescriptorHeap>	m_srvHeap	{};
	map<string, struct UI_TEXTURE>	m_srvTex	;

	GamePlayer*				m_mainPlayer	{};
	vector<EnemyDrone*>		m_vecDrone		;
	EnemyDrone*				m_vecMobBoss	;

	vector<GameBullet*>		m_vecBulletEnemy	;
	vector<GameBullet*>		m_vecBulletPlayer	;

	class UiDrawable*		m_pUi			{};
	class UiDrawable*		m_pUiBg			{};

	PLAY_STATE				m_playState		{};
	float					m_timeStored	{};
	float					m_timeDrone		{};
	float					m_speedBullet	{700};

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

	void	SetStateEnd();
	int		UpdateEnemy(const std::any& t);
	void	BulletFire(GameObject* obj, bool isPlayer);
	void	BulletUpdate(const std::any& t);
	void	RenderDebugging(SpriteBatch* sprite, const XMFLOAT2& begin, const XMFLOAT2& end, const XMVECTORF32& color=XMVECTORF32{{{1.F, 0.F, 1.F, 0.6F}}});
};

#endif
