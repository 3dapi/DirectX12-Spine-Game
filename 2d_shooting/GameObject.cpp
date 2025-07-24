//--------------------------------------------------------------------------------------------------------------------------------------------------------------
// MainApp

#include "MainApp.h"
#include "Common/G2.Util.h"
#include "Common/G2.FactoryMfAudio.h"
#include "GameInfo.h"
#include "GameObject.h"

void GameObject::HP(float v)
{
	m_hp = v;
	if(0>= m_hp)
		this->State(EAPP_CHAR_STATE::ESTATE_CHAR_DYING);
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
	m_kt.pos = v;
}

XMFLOAT2 GameObject::Position() const
{
	return m_kt.pos;
}
void GameObject::Velocity(const XMFLOAT2& v)
{
	m_kt.vlc = v;
}
XMFLOAT2 GameObject::Velocity() const
{
	return m_kt.vlc;
}
void GameObject::Acceleration(const XMFLOAT2& v)
{
	m_kt.acc = v;
}
XMFLOAT2 GameObject::Acceleration() const
{
	return m_kt.acc;
}
XMFLOAT2 GameObject::Direction() const
{
	auto len = this->Speed();
	if(0.0F == len)
		return {0.0F, 0.0F};
	return {m_kt.vlc.x/len, m_kt.vlc.y/len};
}
float GameObject::Speed() const
{
	return sqrtf(m_kt.vlc.x * m_kt.vlc.x + m_kt.vlc.y * m_kt.vlc.y);
}
void GameObject::Scale(float v)
{
	m_kt.scale = v;
}
float GameObject::Scale() const
{
	return m_kt.scale;
}
void GameObject::Rot(float v)
{
	m_kt.rot = v;
}
float GameObject::Rot() const
{
	return m_kt.rot;
}
void GameObject::Diffuse(const XMVECTORF32& v)
{
	m_kt.dif = v;
}
XMVECTORF32 GameObject::Diffuse() const
{
	return m_kt.dif;
}
void GameObject::Box(const XMFLOAT2& v)
{
	m_kt.box = v;
}
XMFLOAT2 GameObject::Box() const
{
	return m_kt.box;
}
void GameObject::Alive(bool v)
{
	m_kt.alive;
}
bool GameObject::Alive() const
{
	return m_kt.alive;
}

void GameObject::MoveForceSpeed(float v)
{
	m_spdForce = v;
}

float GameObject::MoveForceSpeed() const
{
	return m_spdForce;
}

void GameObject::Move(float dt)
{
	m_kt.vlc.x += m_kt.acc.x * dt;
	m_kt.vlc.y += m_kt.acc.y * dt;

	m_kt.pos.x += m_kt.vlc.x * dt;
	m_kt.pos.y += m_kt.vlc.y * dt;
}

void GameObject::MoveLeft(float dt)
{
	m_kt.pos.x += (-1.0F) * m_spdForce* dt;
	this->State(EAPP_CHAR_STATE::ESTATE_CHAR_MOVE);
}

void GameObject::MoveRight(float dt)
{
	m_kt.pos.x += (+1.0F) * m_spdForce* dt;
	this->State(EAPP_CHAR_STATE::ESTATE_CHAR_MOVE);
}

void GameObject::MoveUp(float dt)
{
	m_kt.pos.y += (+1.0F) * m_spdForce* dt;
	this->State(EAPP_CHAR_STATE::ESTATE_CHAR_MOVE);
}

void GameObject::MoveDown(float dt)
{
	m_kt.pos.y += (-1.0F) * m_spdForce* dt;
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
		m_kt.box = {128.0F,94.0F};
	}
}

int GamePlayer::Init(const string& model)
{
	m_hp    = 100;
	m_damage = 34.0F;
	m_spdForce = 250.0F;
	m_kt.pos   = XMFLOAT2{ 0.0F, -300.0F };
	m_kt.dif   = XMVECTORF32{{{ 1.0F, 1.0F, 1.0F, 1.0F }}};

	if(!model.empty())
		m_model    = model;

	if(m_model.empty())
	{
		m_model = EMODEL_SHIP[0];
	}

	this->State(EAPP_CHAR_STATE::ESTATE_CHAR_MOVE);
	return S_OK;
}
int GamePlayer::Update(const GameTimer& gt)
{
	return S_OK;
}

void GamePlayer::State(EAPP_CHAR_STATE v)
{
	GameObject::State(v);
}

EAPP_CHAR_STATE GamePlayer::State() const
{
	return GameObject::State();
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

EnemyDrone::EnemyDrone()
{
}

int EnemyDrone::Init(int movePattern, const T_KINETICS& kt)
{
	m_hp     = 100;
	m_damage = 0.5F;
	m_movePattern = movePattern;
	memcpy(&m_kt, &kt, sizeof(T_KINETICS));
	this->State(EAPP_CHAR_STATE::ESTATE_CHAR_MOVE);

	m_timeStore ={};
	m_timeFire = G2::randomRange(0.5F, 3.5F);

	return S_OK;
}

int EnemyDrone::Update(const GameTimer& t)
{
	GameTimer gt = std::any_cast<GameTimer>(t);
	auto dt = gt.DeltaTime();
	auto pGameInfo = GameInfo::instance();

	// 살아 있을 때만....
	if(!this->m_kt.alive)
		return S_OK;

	if(m_bullet)
	{
		m_timeStore += dt;
		if(m_timeFire<m_timeStore)
		{
			FireBullet();
			--m_bullet;
			m_timeStore -= m_timeFire;
		}
	}

	if (pGameInfo->IsCollisionPlayer(&m_kt) && pGameInfo->m_enablePlay)
	{
		this->m_kt.alive = false;
		auto hp = pGameInfo->MainPlayer()->HP();

		//hp -= this->Damage();
		//if(0>hp)
		//{
		//	hp = 0;
		//}
		//pGameInfo->MainPlayer()->HP(hp);
	}
	else
	{
		this->Move(dt);
	}

	return S_OK;
}

void EnemyDrone::FireBullet()
{
	m_firedBullet = true;
}

EnemyBoss::EnemyBoss()
{
}

int EnemyBoss::Init(int stage)
{
	return 0;
}

int EnemyBoss::Update(const GameTimer& gt)
{
	return 0;
}

int GameBullet::Init(int movePattern, const T_KINETICS& kt, bool isEnemy)
{
	m_isEnemy = isEnemy;
	m_movePattern = movePattern;
	memcpy(&this->pos, &kt, sizeof(T_KINETICS));
	return S_OK;
}

int GameBullet::Update(const GameTimer& gt)
{
	auto dt = gt.DeltaTime();
	auto pGameInfo = GameInfo::instance();

	// 살아 있을 때만....
	if(!this->alive)
		return S_OK;

	if(m_isEnemy && pGameInfo->IsCollisionPlayer(this) && pGameInfo->m_enablePlay)
	{
		this->alive = false;
		//pGameInfo->MainPlayer()->HP(0);
	}
	else
	{
		this->vlc.x += this->acc.x * dt;
		this->vlc.y += this->acc.y * dt;
		this->pos.x += this->vlc.x * dt;
		this->pos.y += this->vlc.y * dt;
	}

	return S_OK;
}
