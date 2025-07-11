
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

class GameInfo
{
public:
	static GameInfo* instance();
	static void      deleteInstance();

public:
	int					m_gameScore	{};
	bool				m_enablePlay{ true };
	float				m_maxMobPos	{1000.0F};		// mob boundary
	int					m_maxMob	{16};			// play max mob count

protected:
	class GamePlayer*	m_player	{};

public:
	GameInfo();
	virtual ~GameInfo();

	GamePlayer*	MainPlayer();
	bool		IsCollisionPlayer(class GameCharacter* p);
	void		IncreaseScore(int score);
};

#endif
