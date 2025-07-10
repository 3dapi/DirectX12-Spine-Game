//--------------------------------------------------------------------------------------------------------------------------------------------------------------
// MainApp

#include <string>
#include <tuple>
#include <d3d12.h>
#include "MainApp.h"
#include "Common/G2.Util.h"
#include "GameInfo.h"
#include "SpineFactory.h"

void GameCharacter::Position(XMFLOAT2 v)
{
	m_pos = v;
	auto spineObj = dynamic_cast<SpineRender*>(m_modelObj);
	if(spineObj)
	{
		spineObj->Position(m_pos);
	}
}
XMFLOAT2 GameCharacter::Position() const
{
	return m_pos;
}
void GameCharacter::Direction(float v)
{
	m_dir = v;
	auto spineObj = dynamic_cast<SpineRender*>(m_modelObj);
	if (spineObj)
	{
		spineObj->Direction(m_dir);
	}
}
float GameCharacter::Direction() const
{
	return m_dir;
}
void GameCharacter::Scale(float v)
{
	m_scale = v;
	auto spineObj = dynamic_cast<SpineRender*>(m_modelObj);
	if (spineObj)
	{
		spineObj->Scale(m_scale);
	}
}
float GameCharacter::Scale() const
{
	return m_scale;
}
void GameCharacter::Speed(float v)
{
	m_speed = v;
}
float GameCharacter::Speed() const
{
	return m_speed;
}

void GameCharacter::Move()
{
	m_pos.x += m_speed;
	m_pos.y += m_speed;

	auto spineObj = dynamic_cast<SpineRender*>(m_modelObj);
	if (spineObj)
	{
		spineObj->Position(m_pos);
	}
}

void GameCharacter::MoveLeft()
{
	m_pos.x += m_speed * (-1.0F);
	m_pos.y += m_speed * (-1.0F);
	auto spineObj = dynamic_cast<SpineRender*>(m_modelObj);
	if (spineObj)
	{
		spineObj->Position(m_pos);
	}
}

void GameCharacter::MoveRight()
{
	m_pos.x += m_speed * (+1.0F);
	m_pos.y += m_speed * (+1.0F);
	auto spineObj = dynamic_cast<SpineRender*>(m_modelObj);
	if (spineObj)
	{
		spineObj->Position(m_pos);
	}
}

void GameCharacter::ModelType(EAPP_MODEL v)
{
	// find model 
	m_modelType = v;
}
EAPP_MODEL GameCharacter::ModelType() const
{
	return m_modelType;
}
void GameCharacter::ModelObject(PG2OBJECT v)
{
	m_modelObj = v;
}

PG2OBJECT GameCharacter::ModelObject() const
{
	return m_modelObj;
}

void GameCharacter::State(EAPP_CHAR_STATE v)
{
	m_state = v;
	string aniName = "idle";
	switch (m_state)
	{
		case EAPP_CHAR_STATE::ESTATE_CHAR_MOVE:
			aniName = "walk";
			break;
		case EAPP_CHAR_STATE::ESTATE_CHAR_ATTACK:
			aniName = "attack";
			break;
	}
	auto spineObj = dynamic_cast<SpineRender*>(m_modelObj);
	if (spineObj)
	{
		spineObj->Animation(aniName);
	}
}

EAPP_CHAR_STATE	GameCharacter::State() const 
{
	return m_state;
}

int GamePlayer::Init(EAPP_MODEL modelType, PG2OBJECT modelObj, EAPP_CHAR_STATE state)
{
	m_hp    = 100;
	m_speed = 5.0F;
	m_pos   = XMFLOAT2{ 0.0F, 0.0F };
	m_dir   = 1.0F;
	m_dif   = XMVectorSet( 1.0F, 1.0F, 1.0F, 1.0F );
	m_modelType = modelType;
	m_modelObj  = modelObj;

	this->State(state);
	return S_OK;
}
int GamePlayer::Update(const GameTimer& gt)
{
	if (m_modelObj)
		return m_modelObj->Update(gt);
	
	return S_OK;
}
int GamePlayer::Render()
{
	if (m_modelObj)
		return m_modelObj->Render();
	return S_OK;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

GameMob::GameMob()
{
	m_speed = 3.0F;
}

GameInfo* g_gameInfo = new GameInfo;

GameInfo::GameInfo()
{
	m_player = new GamePlayer;
}

GamePlayer* GameInfo::MainPlayer()
{
	return m_player;
}
