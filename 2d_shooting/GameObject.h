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
	bool	m_isEnemy		{false};
	int		m_movePattern	{};
	string	m_model;
public:
	int		Init(int movePattern, const T_KINETICS& kt, bool isEnemy);
	int		Update(const GameTimer& gt);
};

class EnemyDrone : public GameObject
{
public:
	int		m_movePattern	{};
	int		m_bullet		{1};
	bool	m_firedBullet	{};
	float	m_timeStore		{};
	float	m_timeFire		{};
public:
	EnemyDrone();
	int		Init(int movePattern, const T_KINETICS& kt);
	int		Update(const GameTimer& gt);
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
