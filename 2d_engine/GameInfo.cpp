//--------------------------------------------------------------------------------------------------------------------------------------------------------------
// MainApp

#include <string>
#include <tuple>
#include <d3d12.h>
#include "MainApp.h"
#include "Common/G2.Util.h"
#include "GameInfo.h"
#include "GameCharacter.h"

using namespace G2;

static GameInfo* _inst = new GameInfo;

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

bool GameInfo::IsCollisionPlayer(class GameCharacter* p)
{
	float p0_w = m_player->m_boundBox.x;
	float p0_h = m_player->m_boundBox.y;
	float p1_w = p->m_boundBox.x;
	float p1_h = p->m_boundBox.y;

	float p0_x = m_player->m_pos.x - p0_w * 0.5F;
	float p0_y = m_player->m_pos.y - p0_h * 0.5F;
	float p1_x = p->m_pos.x - p0_w * 0.5F;
	float p1_y = p->m_pos.y - p0_h * 0.5F;

	auto ret =	p0_x        <= p1_x + p1_w &&	// left   <= v_right
				p0_x + p0_w >= p1_x        &&	// right  >= v_left
				p0_y        <= p1_y + p1_h &&	// top    <= v_bottom
				p0_y + p0_h >= p1_y;			// bottom >= v_top
		
	return ret;
}

void GameInfo::IncreaseScore(int score)
{
	m_gameScore += score;
}

int GameInfo::StageInit()
{
	// 점수 초기화.
	this->m_gameScore = 0;
	this->m_enablePlay = true;
	this->m_stageCur = 0;

	// 스테이지 환수 조건
	m_killedMob.resize(MAX_STAGE, 0)	;
	m_stage    .resize(MAX_STAGE, {})	;

	std::fill(m_killedMob.begin(), m_killedMob.end(), 0);
	std::fill(m_stage.begin(), m_stage.end(), GAME_STAGE{});

	m_stage[0] =	{  5, {EAPP_MODEL::EMODEL_STMAN }, };
	m_stage[1] =	{  5, {EAPP_MODEL::EMODEL_RAPTOR}, };
	m_stage[2] =	{ 10, {EAPP_MODEL::EMODEL_RAPTOR, EAPP_MODEL::EMODEL_GOBLIN}, };
	m_stage[3] =	{ 13, {EAPP_MODEL::EMODEL_RAPTOR, EAPP_MODEL::EMODEL_GOBLIN, EAPP_MODEL::EMODEL_ALIEN}, };
	m_stage[4] =	{ 20, {EAPP_MODEL::EMODEL_RAPTOR, EAPP_MODEL::EMODEL_BOSS}, };

	return S_OK;
}

GAME_STAGE* GameInfo::CurrentState()
{
	if(0> m_stageCur || m_stageCur>= m_stage.size())
		return nullptr;
	return &m_stage[m_stageCur];
}

bool GameInfo::CurrentStateComplete()
{
	if(0 == m_stageCur)
	{
		if(5 <= m_killedMob[m_stageCur])
		{
			return true;
		}
	}

	return false;
}

void GameInfo::CurrentStateAdvancing(int v)
{
	if(0 == m_stageCur)
	{
		m_killedMob[m_stageCur] += v;
	}
}
