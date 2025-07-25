#pragma once
#ifndef _GameInfo_H_
#define _GameInfo_H_

#include "AppCommon.h"
#include "GameObject.h"

struct GAME_STAGE
{
	int						mobMax	{};
	vector<string>			mobType	{};
};

class GameInfo
{
public:
	static GameInfo* instance();
	static void      deleteInstance();
public:
	static bool				M_CHEAT		;

	int						m_gameScore	{};
	bool					m_enablePlay	{ true };
	float					m_maxMobPos		{1000.0F};		// mob boundary
protected:
	class GamePlayer*		m_player		{};

public:
	GameInfo();
	virtual ~GameInfo();

	GamePlayer*	MainPlayer();
	void		IncreaseScore(int score);
};

#endif
