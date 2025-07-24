//--------------------------------------------------------------------------------------------------------------------------------------------------------------
// MainApp

#include <string>
#include <tuple>
#include <d3d12.h>
#include "MainApp.h"
#include "Common/G2.Util.h"
#include "Common/G2.FactoryMfAudio.h"
#include "GameInfo.h"
#include "GameObject.h"

void GameObject::HP(float v)
{
	m_hp = v;
}

float GameObject::HP() const
{
	return m_hp;
}

void GameObject::Damage(float v)
{
	m_damage = v;
}

float GameObject::Damage() const
{
	return m_damage;
}

void GameObject::Position(XMFLOAT2 v)
{
	m_pos = v;
}

XMFLOAT2 GameObject::Position() const
{
	return m_pos;
}
void GameObject::Direction(float v)
{
	m_dir = v;
}
float GameObject::Direction() const
{
	return m_dir;
}
void GameObject::Scale(float v)
{
	m_scale = v;
}
float GameObject::Scale() const
{
	return m_scale;
}
void GameObject::Speed(float v)
{
	m_speed = v;
}
float GameObject::Speed() const
{
	return m_speed;
}

void GameObject::Move(float dt)
{
	m_pos.x += m_speed * dt;
	m_pos.y += m_speed * dt;
}

void GameObject::MoveLeft(float dt)
{
	this->Direction(-1.0F);
	m_pos.x += m_speed * dt * m_dir;

	this->State(EAPP_CHAR_STATE::ESTATE_CHAR_MOVE);
}

void GameObject::MoveRight(float dt)
{
	this->Direction(+1.0F);
	m_pos.x += m_speed * dt * m_dir;

	this->State(EAPP_CHAR_STATE::ESTATE_CHAR_MOVE);
}

void GameObject::MoveUp(float dt)
{
	m_pos.y += m_speed * dt * (+1.0F);

	this->State(EAPP_CHAR_STATE::ESTATE_CHAR_MOVE);
}

void GameObject::MoveDown(float dt)
{
	m_pos.y += m_speed * dt * (-1.0F);

	this->State(EAPP_CHAR_STATE::ESTATE_CHAR_MOVE);
}

GameObject::~GameObject()
{
}

void GameObject::State(EAPP_CHAR_STATE v)
{
	// state 가 같으면 변경을 안하다.
	if (m_state == v)
	{
		return;
	}
	m_state = v;
}

EAPP_CHAR_STATE	GameObject::State() const 
{
	return m_state;
}

GamePlayer::GamePlayer()
{
	//AFEW::C
	if(GameInfo::M_CHEAT)
	{
		m_boundBox = {160.0F,80.0F};
	}
}

int GamePlayer::Init(EAPP_MODEL modelType, PG2OBJECT modelObj, EAPP_CHAR_STATE state)
{
	m_hp    = 100;
	m_damage = 34.0F;
	m_speed = 250.0F;
	m_pos   = XMFLOAT2{ 0.0F, 0.0F };
	m_dir   = 1.0F;
	m_dif   = XMFLOAT4{ 1.0F, 1.0F, 1.0F, 1.0F };
	m_modelType = modelType;

	this->State(state);
	return S_OK;
}
int GamePlayer::Update(const GameTimer& gt)
{
	m_aniComplete.clear();

	return S_OK;
}
int GamePlayer::Render()
{
	return S_OK;
}

int GamePlayer::Notify(const std::string& aniname, const std::any& val)
{
	string eventType;
	if(val.has_value())
	{
		try
		{
			eventType = any_cast<const string&>(val);
		}
		catch(const bad_any_cast&)
		{
			return E_FAIL;
		}
	}
	if(aniname == "attack" && eventType == "start")
	{
		//printf("GamePlayer::Notify:: %s %s\n", aniname.c_str(), eventType.c_str());
		m_audio->Play(false);
	}

	// attack complete 3번오면 idle 로 변경
	if(aniname == "attack" && eventType == "complete")
	{
		--m_attackRepeat;
		if( 0 == m_attackRepeat)
		{
			this->State(EAPP_CHAR_STATE::ESTATE_CHAR_IDLE);
		}
		m_aniComplete["attack"] = true;

		if(2<m_attackRepeat)
			m_audio->Play(false);
	}

	return S_OK;
}

void GamePlayer::State(EAPP_CHAR_STATE v)
{
	if(m_state != v && v == EAPP_CHAR_STATE::ESTATE_CHAR_ATTACK)
	{
		// 3회 애니메이션
		m_attackRepeat = 3;
	}
	GameObject::State(v);
}

EAPP_CHAR_STATE GamePlayer::State() const
{
	return GameObject::State();
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
	m_damage = 0.5F;
	m_speed  = 70.0F * G2::randomRange(1.5F, 2.5F);
	m_pos    = XMFLOAT2{ 0.0F, 0.0F };
	m_dir    = 1.0F;
	m_dif    = XMFLOAT4{ 1.0F, 1.0F, 1.0F, 1.0F };

	this->State(state);

	return S_OK;
}

int GameMob::Update(const GameTimer& t)
{
	GameTimer gt = std::any_cast<GameTimer>(t);
	auto dt = gt.DeltaTime();
	auto pGameInfo = GameInfo::instance();

	// 살아 있을 때만....
	if(0< this->m_hp)
	{
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

			if(EAPP_CHAR_STATE::ESTATE_CHAR_IDLE != this->m_state)
			{
				if (0 > m_dir)
					MoveLeft(dt);
				else
					MoveRight(dt);
			}
		}
	}

	return S_OK;
}

int GameMob::Render()
{
	return S_OK;
}

int GameMob::Notify(const std::string& aniName, const std::any& val)
{
	string eventType;
	if(val.has_value())
	{
		try
		{
			eventType = any_cast<const string&>(val);
		}
		catch(const bad_any_cast&)
		{
			return E_FAIL;
		}
	}
	return S_OK;
}
