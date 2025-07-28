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
	friend	class UiPlay;
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
	inline static const int			MAX_SND_EFFECT		{15};
	inline static const int			MAX_DRONE			{1000};
	inline static const int			MAX_BULLET_PLAYER	{1000};
	inline static const int			MAX_BULLET_ENEMY	{1000};
	inline static const int			MAX_MISSLE_ENEMY	{2000};

	ComPtr<ID3D12DescriptorHeap>	m_srvHeap	{};
	map<string, struct UI_TEXTURE>	m_srvTex	;

	GamePlayer*				m_mainPlayer	{};
	vector<EnemyDrone*>		m_vecDrone		;
	EnemyBoss*				m_droneBoss		{};

	vector<GameBullet*>		m_vecBulletEnemy	;
	vector<GameBullet*>		m_vecBulletPlayer	;

	vector<GameMissile*>	m_vecMissileEnemy	;
	vector<PG2AUDIOPLAYER>	m_sndLaser		{};
	vector<PG2AUDIOPLAYER>	m_sndBoom		{};

	class UiDrawable*		m_pUi			{};
	class UiDrawable*		m_pUiBg			{};

	PLAY_STATE				m_playState		{};
	float					m_timeStored	{};
	float					m_timeDrone		{};

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

	void	SetStateStart();
	void	SetStateEnd();
	int		UpdateEnemy(const std::any& t);
	void	BulletFire(GameObject* obj, bool isPlayer);
	void	BulletUpdate(const std::any& t);
	void	MissileFire(GameObject* obj);
	void	MissileUpdate(const std::any& t);
	void	DroneBossSetup();
	void	DroneBossUpdate(const std::any& t);
	void	BulletSoundPlay();
	void	BoomSoundPlay();
	void	RenderDebugging(SpriteBatch* sprite, const XMFLOAT2& begin, const XMFLOAT2& end, const XMVECTORF32& color=XMVECTORF32{{{1.F, 0.F, 1.F, 0.6F}}});
};

#endif
