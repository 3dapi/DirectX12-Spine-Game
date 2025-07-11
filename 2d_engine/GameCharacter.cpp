//--------------------------------------------------------------------------------------------------------------------------------------------------------------
// MainApp

#include <string>
#include <tuple>
#include <d3d12.h>
#include "MainApp.h"
#include "Common/G2.Util.h"
#include "SpineFactory.h"
#include "GameInfo.h"
#include "GameCharacter.h"

void GameCharacter::HP(float v)
{
	m_hp = v;
}

float GameCharacter::HP() const
{
	return m_hp;
}

void GameCharacter::Damage(float v)
{
	m_damage = v;
}

float GameCharacter::Damage() const
{
	return m_damage;
}

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

void GameCharacter::Move(float dt)
{
	m_pos.x += m_speed * dt;
	m_pos.y += m_speed * dt;

	auto spineObj = dynamic_cast<SpineRender*>(m_modelObj);
	if (spineObj)
	{
		spineObj->Position(m_pos);
	}
}

void GameCharacter::MoveLeft(float dt)
{
	this->Direction(-1.0F);
	m_pos.x += m_speed * dt * m_dir;

	this->State(EAPP_CHAR_STATE::ESTATE_CHAR_MOVE);

	auto spineObj = dynamic_cast<SpineRender*>(m_modelObj);
	if (spineObj)
	{
		spineObj->Position(m_pos);
	}
}

void GameCharacter::MoveRight(float dt)
{
	this->Direction(+1.0F);
	m_pos.x += m_speed * dt * m_dir;

	this->State(EAPP_CHAR_STATE::ESTATE_CHAR_MOVE);

	auto spineObj = dynamic_cast<SpineRender*>(m_modelObj);
	if (spineObj)
	{
		spineObj->Position(m_pos);
	}
}

void GameCharacter::MoveUp(float dt)
{
	m_pos.y += m_speed * dt * (+1.0F);

	this->State(EAPP_CHAR_STATE::ESTATE_CHAR_MOVE);

	auto spineObj = dynamic_cast<SpineRender*>(m_modelObj);
	if (spineObj)
	{
		spineObj->Position(m_pos);
	}
}

void GameCharacter::MoveDown(float dt)
{
	m_pos.y += m_speed * dt * (-1.0F);

	this->State(EAPP_CHAR_STATE::ESTATE_CHAR_MOVE);

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

GameCharacter::~GameCharacter()
{
	SAFE_DELETE(m_modelObj);
}

void GameCharacter::State(EAPP_CHAR_STATE v)
{
	// state 가 같으면 변경을 안하다.
	if (m_state == v)
	{
		return;
	}
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

GamePlayer::GamePlayer()
{
}

int GamePlayer::Init(EAPP_MODEL modelType, PG2OBJECT modelObj, EAPP_CHAR_STATE state)
{
	m_hp    = 100;
	m_damage = 5.0F;
	m_speed = 400.0F;
	m_pos   = XMFLOAT2{ 0.0F, 0.0F };
	m_dir   = 1.0F;
	m_dif   = XMFLOAT4{ 1.0F, 1.0F, 1.0F, 1.0F };
	m_modelType = modelType;

	if(m_modelObj)
	{
		SAFE_DELETE(m_modelObj);
	}
	m_modelObj  = modelObj;

	this->State(state);
	auto spineObj = dynamic_cast<SpineRender*>(m_modelObj);
	if (spineObj)
	{
		if (m_state == EAPP_CHAR_STATE::ESTATE_CHAR_IDLE)
		{
			auto cur_ani = spineObj->Animation();
			if (cur_ani != "idle")
			{
				spineObj->Animation("idle");
			}
		}
	}
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

int GameMob::Init(EAPP_MODEL modelType, PG2OBJECT modelObj, EAPP_CHAR_STATE state)
{
	m_hp     = 100;
	m_damage = 0.7F;
	m_speed  = 70.0F * G2::randomRange(1.5F, 2.5F);
	m_pos    = XMFLOAT2{ 0.0F, 0.0F };
	m_dir    = 1.0F;
	m_dif    = XMFLOAT4{ 1.0F, 1.0F, 1.0F, 1.0F };

	// model object가 있는 경우에만 교체.
	if (modelObj)
	{
		SAFE_DELETE(m_modelObj);

		m_modelType = modelType;
		m_modelObj = modelObj;
	}

	this->State(state);
	auto spineObj = dynamic_cast<SpineRender*>(m_modelObj);
	if (spineObj)
	{
		spineObj->Color(m_dif);
		if (m_state == EAPP_CHAR_STATE::ESTATE_CHAR_MOVE)
		{
			auto cur_ani = spineObj->Animation();
			if (cur_ani != "walk")
			{
				spineObj->Animation("walk");
			}
		}
	}
	return S_OK;
}

int GameMob::Update(const GameTimer& t)
{
	GameTimer gt = std::any_cast<GameTimer>(t);
	auto dt = gt.DeltaTime();
	auto pGameInfo = GameInfo::instance();

	if (!pGameInfo->IsCollisionPlayer(this) || !pGameInfo->m_enablePlay)
	{
		if (-pGameInfo->m_maxMobPos > this->m_pos.x)
		{
			m_dir = +1.0;
		}
		else if (pGameInfo->m_maxMobPos < this->m_pos.x)
		{
			m_dir = -1.0;
		}

		if (0 > m_dir)
			MoveLeft(dt);
		else
			MoveRight(dt);
	}
	auto spineObj = dynamic_cast<SpineRender*>(m_modelObj);
	if (spineObj)
	{
		if (30 > m_hp)
		{
			spineObj->Color({ 1.0F, 0.4F, 0.4F, 0.5F });
		}
		else if (60 > m_hp)
		{
			spineObj->Color({ 1.0F, 0.6F, 0.6F, 0.8F });
		}
		m_modelObj->Update(gt);
	}
	return S_OK;
}

int GameMob::Render()
{
	if (m_modelObj)
		return m_modelObj->Render();
	return S_OK;
}
