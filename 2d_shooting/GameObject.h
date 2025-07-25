#pragma once
#ifndef _GameObject_H_
#define _GameObject_H_

#include "AppCommon.h"

using namespace std;
using namespace G2;

struct T_KINETICS
{
	XMFLOAT2		pos		{0.0F, 0.0F};					// position
	XMFLOAT2		vlc		{0.0F, 0.0F};					// velocity
	XMFLOAT2		acc		{0.0F, 0.0F};					// acceleration
	float			scale	{1.0F};							// scale
	float			rot		{0.0F};							// 회전
	XMVECTORF32		dif		{{{1.0F, 0.0F, 1.0F, 1.0F}}};	// model color
	XMFLOAT2		box		{0.0F, 0.0F };
	bool			alive	{false};
};

inline bool IsCollision(const struct T_KINETICS* p0, const struct T_KINETICS* p1)
{
	float p0_w = p0->box.x * p0->scale;
	float p0_h = p0->box.y * p0->scale;
	float p0_x = p0->pos.x - p0_w * 0.5F;
	float p0_y = p0->pos.y - p0_h * 0.5F;

	float p1_w = p1->box.x * p1->scale;
	float p1_h = p1->box.y * p1->scale;
	float p1_x = p1->pos.x - p1_w * 0.5F;
	float p1_y = p1->pos.y - p1_h * 0.5F;

	auto ret =	p0_x        <= p1_x + p1_w &&	// left   <= v_right
				p0_x + p0_w >= p1_x        &&	// right  >= v_left
				p0_y        <= p1_y + p1_h &&	// top    <= v_bottom
				p0_y + p0_h >= p1_y;			// bottom >= v_top
	if(ret)
	{
		int c;
		c = 0;
	}
	return ret;
}

class GameInfo;
class GameObject
{
	friend GameInfo;
protected:
	EAPP_CHAR_STATE	m_state	{ EAPP_CHAR_STATE::ESTATE_CHAR_MOVE };
	T_KINETICS		m_kt		{};	// kinetics
	float			m_spdForce	{};
	float			m_hp		{100};
	float			m_damage	{50};
	string			m_model		;

public:
	virtual ~GameObject();

	virtual	void			State(EAPP_CHAR_STATE v);
	virtual	EAPP_CHAR_STATE	State() const;

	virtual	void		HP(float v);
	virtual	float		HP() const;
	virtual	void		Damage(float v);
	virtual	float		Damage() const;

	virtual	const T_KINETICS* Kinetics() const { return &m_kt; }
	virtual	void		Position(XMFLOAT2 v);
	virtual	XMFLOAT2	Position() const;
	virtual	void		Velocity(const XMFLOAT2& v);
	virtual	XMFLOAT2	Velocity() const;
	virtual	void		Acceleration(const XMFLOAT2& v);
	virtual	XMFLOAT2	Acceleration() const;
	virtual	XMFLOAT2	Direction() const;
	virtual	float		Speed() const;
	virtual	void		Scale(float v);
	virtual	float		Scale() const;
	virtual	void		Rot(float v);
	virtual	float		Rot() const;
	virtual	void		Diffuse(const XMVECTORF32& v);
	virtual	XMVECTORF32	Diffuse() const;
	virtual	void		Box(const XMFLOAT2& v);
	virtual	XMFLOAT2	Box() const;
	virtual	void		Alive(bool v);
	virtual	bool		Alive() const;

	virtual	void		MoveForceSpeed(float v);
	virtual	float		MoveForceSpeed() const;

	virtual	void		Model(const string& v) { m_model = v;}
	virtual	string		Model() const { return m_model;}

	virtual	void		Move(float dt);
	virtual	void		MoveLeft(float dt);
	virtual	void		MoveRight(float dt);
	virtual	void		MoveUp(float dt);
	virtual	void		MoveDown(float dt);
};

class GamePlayer : public GameObject
{
public:
	GamePlayer();
	int		Init(const string& m_model="");
	int		Update(const GameTimer& gt);
	void	State(EAPP_CHAR_STATE v) override;
	EAPP_CHAR_STATE	State() const override;
};

class GameBullet : public T_KINETICS
{
public:
	bool	m_isPlayer		{false};
	int		m_movePattern	{};
	string	m_model;
public:
	int		Init(int movePattern, const T_KINETICS& kt, bool isPlayer);
	int		Update(const GameTimer& gt, const function<bool(GameBullet* obj)>& funcCollision);
};

class EnemyDrone : public GameObject
{
public:
	int		m_movePattern	{};
	int		m_bullet		{1};
	bool	m_firedBullet	{};
	float	m_timeStore		{};
	float	m_timeFire		{};
	float	m_flipLen		{};
	float	m_flipAngle		{};
public:
	EnemyDrone();
	int		Init(int movePattern, const T_KINETICS& kt);
	int		Update(const GameTimer& gt);
	void	Move(float dt) override;
	void	FireBullet();
};

class EnemyBoss : public GameObject
{
protected:
	int		m_movePattern{};
	string	m_model;
public:
	EnemyBoss();
	int		Init(int stage);
	int		Update(const GameTimer& gt);
};

#endif
