
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

using namespace G2;

class GameInfo
{
public:
	int					m_gameScore{};
	bool				m_enablePlay{ true };

protected:
	class GamePlayer*	m_player	{};

public:
	GameInfo();
	GamePlayer*		MainPlayer();

	bool	IsCollisionPlayer(class GameCharacter* p);
	void	IncreaseScore(int score);
};

extern GameInfo* g_gameInfo;

#endif
