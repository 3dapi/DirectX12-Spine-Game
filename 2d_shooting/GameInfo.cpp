//--------------------------------------------------------------------------------------------------------------------------------------------------------------
// MainApp

#include "MainApp.h"
#include "Common/G2.Util.h"
#include "GameInfo.h"
#include "GameObject.h"

using namespace G2;

static GameInfo* _inst = new GameInfo;
bool GameInfo::M_CHEAT = true;

GameInfo* GameInfo::instance()
{
	return _inst;
}
void GameInfo::deleteInstance()
{
	SAFE_DELETE(_inst);
}

GameInfo::GameInfo()
{
	m_player = new GamePlayer;
}

GameInfo::~GameInfo()
{
	SAFE_DELETE(m_player);
}

GamePlayer* GameInfo::MainPlayer()
{
	return m_player;
}

void GameInfo::IncreaseScore(int score)
{
	m_gameScore += score;
}


