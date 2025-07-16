
#pragma once
#ifndef _GameInfo_H_
#define _GameInfo_H_

#include <string>
#include <DirectXMath.h>
#include <DirectXColors.h>

#include "Common/G2.Constants.h"
#include "common/G2.ConstantsWin.h"
#include "AppCommon.h"
#include "GameCharacter.h"

struct GAME_STAGE
{
	int						mobMax	{};
	vector<EAPP_MODEL>		mobType	{};
};

class GameInfo
{
public:
	static GameInfo* instance();
	static void      deleteInstance();

	inline static const int	MAX_STAGE	{5};		// max stage
public:
	int						m_gameScore	{};
	// 스테이지 환수 조건
	vector<int>				m_killedMob		;
	bool					m_enablePlay	{ true };
	float					m_maxMobPos		{1000.0F};		// mob boundary

	vector<GAME_STAGE>		m_stage			;
	int						m_stageCur		{0};			// current state

protected:
	class GamePlayer*		m_player		{};

public:
	GameInfo();
	virtual ~GameInfo();

	GamePlayer*	MainPlayer();
	bool		IsCollisionPlayer(class GameCharacter* p);
	void		IncreaseScore(int score);

	int			StageInit();
	int			CurrentStateIndex() const { return m_stageCur; }
	GAME_STAGE*	CurrentState();
	bool		CurrentStateComplete();
	void		CurrentStateAdvancing(int v);
};

#endif
