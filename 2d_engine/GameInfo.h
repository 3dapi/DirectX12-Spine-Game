
#pragma once
#ifndef _GameInfo_H_
#define _GameInfo_H_

#include <string>
#include <DirectXMath.h>
#include <DirectXColors.h>
using namespace G2;

#include "Common/G2.Constants.h"
#include "common/G2.ConstantsWin.h"
#include "AppCommon.h"

using namespace std;
using namespace DirectX;


class GameCharacter
{
protected:
	EAPP_CHAR_STATE	m_state	{ EAPP_CHAR_STATE::ESTATE_CHAR_IDLE };
	int				m_hp	{100};
	XMFLOAT2		m_pos	{};			// position
	float			m_dir	{ 1.0F };	// direction: left: -1, right:1
	float			m_scale	{ 1.0F };	// model scale
	float			m_speed	{ 5.0F };	// move speed
	XMVECTOR 		m_dif	{1.0F, 1.0F, 1.0F, 1.0F};	// model color

	EAPP_MODEL		m_modelType	{ EAPP_MODEL::EMODEL_NONE };
	PG2OBJECT		m_modelObj	{};
public:
	virtual	void			State(EAPP_CHAR_STATE v);
	virtual	EAPP_CHAR_STATE	State() const;

	virtual	void		Position(XMFLOAT2 v);
	virtual	XMFLOAT2	Position() const;
	virtual	void		Direction(float v);
	virtual	float		Direction() const;
	virtual	void		Scale(float v);
	virtual	float		Scale() const;
	virtual	void		Speed(float v);
	virtual	float		Speed() const;

	virtual	void		Move();
	virtual	void		MoveLeft();
	virtual	void		MoveRight();

	virtual	void		ModelType(EAPP_MODEL v);
	virtual	EAPP_MODEL	ModelType() const;

	virtual	void		ModelObject(PG2OBJECT v);
	virtual	PG2OBJECT	ModelObject() const;
};

class GamePlayer : public GameCharacter
{
protected:
public:
	int		Init(EAPP_MODEL modelType, PG2OBJECT modelObj, EAPP_CHAR_STATE state= EAPP_CHAR_STATE::ESTATE_CHAR_IDLE);
	int		Update(const GameTimer& gt);
	int		Render();
};

class GameMob : public GameCharacter
{
public:
	GameMob();
};


class GameInfo
{
public:
	static const int	MAX_MOB = 32;

public:
	int				m_gameScore{};

protected:
	GamePlayer*		m_player	{};


public:
	GameInfo();
	GamePlayer*		MainPlayer();
};

extern GameInfo* g_gameInfo;

#endif
