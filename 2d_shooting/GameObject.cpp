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
	{
		this->Alive(false);
		this->State(EAPP_CHAR_STATE::ESTATE_CHAR_DYING);
	}
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
	m_kt.alive = v;
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
}

int GamePlayer::Init(const string& model)
{
	m_hp    = 500;
	m_damage = 35.0F;
	m_spdForce = 360.0F;
	m_kt       = {};
	m_kt.pos   = XMFLOAT2{ 0.0F, -300.0F };
	m_kt.dif   = XMVECTORF32{{{ 1.0F, 1.0F, 1.0F, 1.0F }}};
	m_kt.alive = true;

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

void GamePlayer::PositionScreenBoundary(const ::SIZE& screenSize)
{
	if(-screenSize.cx/2.0F > m_kt.pos.x)
	{
		m_kt.pos.x = -screenSize.cx/2.0F;
	}
	if(+screenSize.cx/2.0F < m_kt.pos.x)
	{
		m_kt.pos.x = +screenSize.cx/2.0F;
	}
	if(-screenSize.cy/2.0F > m_kt.pos.y)
	{
		m_kt.pos.y = -screenSize.cy/2.0F;
	}
	if(+screenSize.cy/2.0F < m_kt.pos.y)
	{
		m_kt.pos.y = +screenSize.cx/2.0F;
	}
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

EnemyDrone::EnemyDrone()
{
}

int EnemyDrone::Init(int movePattern, const T_KINETICS& kt)
{
	m_movePattern = movePattern;
	memcpy(&m_kt, &kt, sizeof(T_KINETICS));
	this->State(EAPP_CHAR_STATE::ESTATE_CHAR_MOVE);

	m_hp     = 50;
	m_damage = 70.0F;

	m_bullet		= movePattern;
	m_firedBullet	= {};
	m_timeStore		= {};
	m_timeFire		= {};
	m_flipLen		= {};
	m_flipAngle		= {};

	m_timeFire = G2::randomRange(0.5F, 3.5F);
	if(1>= m_movePattern)
	{
	}
	else if(2>= m_movePattern)
	{
		m_timeFire = G2::randomRange(0.2F, 1.5F);
		m_flipLen = G2::randomRange(200.0F, 500.0F);
		m_flipAngle = G2::randomRange(0.0F, float(2.0 * M_PI));
	}
	else if(3>= m_movePattern)
	{
		m_timeFire = G2::randomRange(0.1F, 1.0F);
		m_flipLen = G2::randomRange(100.0F, 400.0F);
		m_flipAngle = G2::randomRange(0.0F, float(2.0 * M_PI));
	}

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

	auto playerKt = pGameInfo->MainPlayer()->Kinetics();
	bool isCollision = IsCollision(   playerKt->pos.x, playerKt->pos.y
									, playerKt->box.x, playerKt->box.y, playerKt->scale
									, m_kt.pos.x, m_kt.pos.y
									, m_kt.box.x, m_kt.box.y, m_kt.scale);
	if (isCollision && pGameInfo->m_enablePlay)
	{
		this->m_kt.alive = false;
		auto hp = pGameInfo->MainPlayer()->HP();
		hp -= this->Damage();
		if(0>hp)
		{
			hp = 0;
		}
		if(!GameInfo::M_CHEAT)
		{
			pGameInfo->MainPlayer()->HP(hp);
		}
		pGameInfo->IncreaseScore(1000);
	}
	else
	{
		this->Move(dt);
	}

	return S_OK;
}

void EnemyDrone::Move(float dt)
{
	if(1>= m_movePattern)
	{
	}
	else if(2>= m_movePattern)
	{
		m_kt.acc.x = m_flipLen * sinf(m_timeStore + m_flipAngle);
	}
	else if(4>= m_movePattern)
	{
		m_kt.acc.x = m_flipLen * cosf(m_timeStore + m_flipAngle);
		m_kt.acc.y = m_flipLen * sinf(m_timeStore + m_flipAngle);
	}

	m_kt.vlc.x += m_kt.acc.x * dt;
	m_kt.vlc.y += m_kt.acc.y * dt;

	m_kt.pos.x += m_kt.vlc.x * dt;
	m_kt.pos.y += m_kt.vlc.y * dt;
}

void EnemyDrone::FireBullet()
{
	m_firedBullet = true;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

EnemyBoss::EnemyBoss()
{
}

int EnemyBoss::Init(int movePattern, const T_KINETICS& kt)
{
	m_movePattern = movePattern;
	memcpy(&m_kt, &kt, sizeof(T_KINETICS));
	this->State(EAPP_CHAR_STATE::ESTATE_CHAR_MOVE);

	m_hp     = 600;
	m_damage = 40.0F;
	m_model = movePattern;

	m_firedMissile	= {};
	m_timeStore		= {};
	m_timeFire		= 1.6F;	// 패턴 미사일 발사 간격.

	//m_timeFire		= G2::randomRange(0.5F, 3.5F);
	//if(1>= m_movePattern)
	//{
	//}
	//else if(2>= m_movePattern)
	//{
	//	m_timeFire = G2::randomRange(0.2F, 1.5F);
	//}
	//else if(3>= m_movePattern)
	//{
	//	m_timeFire = G2::randomRange(0.1F, 1.0F);
	//}
	m_mov_pattern	  = {};
	m_mov_pattern.phase = (float)M_PI;
	m_mov_pattern.center.y = 200.0F;
	m_mov_pattern.speed = 0.8F;
	m_mov_pattern.len   = 220;
	this->MovePatternCircle({});

	return S_OK;
}

int EnemyBoss::Update(const GameTimer& gt)
{
	auto dt = gt.DeltaTime();
	auto pGameInfo = GameInfo::instance();
	auto mainPlayer= pGameInfo->MainPlayer();

	if(this->m_kt.alive)
	{
		m_timeStore += dt;
		if(m_timeFire<m_timeStore)
		{
			FireBosMissle();
			m_timeStore -= m_timeFire;
		}
	}

	auto playerKt = mainPlayer->Kinetics();
	bool isCollision = IsCollision(playerKt->pos.x, playerKt->pos.y
									, playerKt->box.x, playerKt->box.y, playerKt->scale
									, m_kt.pos.x, m_kt.pos.y
									, m_kt.box.x, m_kt.box.y, m_kt.scale);
	// main player 와 충돌.
	if(this->m_kt.alive && pGameInfo->m_enablePlay && isCollision)
	{
		// setup main player damage
		auto hp = mainPlayer->HP();
		hp -= this->Damage();
		if(0>hp)
		{
			hp = 0;
		}
		if(!GameInfo::M_CHEAT)
		{
			pGameInfo->MainPlayer()->HP(hp);
		}
		pGameInfo->IncreaseScore(3000);

		// setup boss damage
		hp = this->HP();
		hp -= mainPlayer->Damage();
		if(0>hp)
		{
			hp = 0;
		}
		this->HP(hp);
	}

	this->MovePatternCircle(gt);
	return S_OK;
}

void EnemyBoss::MovePatternCircle(const GameTimer& gt)
{
	m_mov_pattern.angle += gt.DeltaTime() * m_mov_pattern.speed;
	float angle = m_mov_pattern.angle + m_mov_pattern.phase;
	float x = m_mov_pattern.len * cosf(angle);
	float y = m_mov_pattern.len * sinf(angle);
	this->m_kt.pos.x = m_mov_pattern.center.x + x;
	this->m_kt.pos.y = m_mov_pattern.center.y + y;
}

void EnemyBoss::FireBosMissle()
{
	m_firedMissile = true;
}

int GameBullet::Init(int movePattern, const T_KINETICS& kt, bool isPlayer)
{
	m_movePattern = movePattern;
	*((T_KINETICS*)this) = kt;
	m_isPlayer = isPlayer;	
	return S_OK;
}

int GameBullet::Update(const GameTimer& gt, const function<bool(GameBullet*)>& funcCollision)
{
	auto dt = gt.DeltaTime();
	// 살아 있을 때만....
	if(!this->alive)
		return S_OK;

	if(funcCollision && funcCollision(this))
	{
		this->alive = false;
		// 적 탄환
		if(!m_isPlayer)
		{
			// player hp 를 줄임.
			auto hp = GameInfo::instance()->MainPlayer()->HP();
			hp -= 15;
			if(0>hp)
			{
				hp = 0;
			}
			if(!GameInfo::M_CHEAT)
			{
				GameInfo::instance()->MainPlayer()->HP(hp);
			}
		}
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

int GameMissile::Init(int movePattern, const T_KINETICS& kt, bool isPlayer)
{
	m_movePattern = movePattern;
	*((T_KINETICS*)this) = kt;
	m_isPlayer = isPlayer;
	return S_OK;
}

int GameMissile::Update(const GameTimer& gt, const function<bool(GameMissile*)>& funcCollision)
{
	auto dt = gt.DeltaTime();
	// 살아 있을 때만....
	if(!this->alive)
		return S_OK;

	if(funcCollision && funcCollision(this))
	{
		this->alive = false;
		// 적 미사일
		if(!m_isPlayer)
		{
			// player hp 를 줄임.
			auto hp = GameInfo::instance()->MainPlayer()->HP();
			hp -= 20;
			if(0>hp)
			{
				hp = 0;
			}
			if(!GameInfo::M_CHEAT)
			{
				GameInfo::instance()->MainPlayer()->HP(hp);
			}
		}
	}
	else
	{
		this->vlc.x += this->acc.x * dt;
		this->vlc.y += this->acc.y * dt;

		float len = sqrtf(vlc.x * vlc.x + vlc.y * vlc.y);
		if(0.000001F < len)
		{
			vlc.x /= len;
			vlc.y /= len;
			if(1.0F < fabsf(vlc.x))
			{
				if(0.0F <= vlc.y)
					rot = 0.0F;
				else
					rot = -(float)M_PI;
			}
			else
			{
				rot = atan2f(vlc.x, vlc.y);
			}
		}
		vlc.x *= GameInfo::SPEED_MISSILE;
		vlc.y *= GameInfo::SPEED_MISSILE;

		this->pos.x += this->vlc.x * dt;
		this->pos.y += this->vlc.y * dt;
	}

	return S_OK;
}
