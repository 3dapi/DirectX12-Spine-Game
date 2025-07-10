
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
	EAPP_CHAR_STATE	m_st	{ ESTATE_CHAR_IDLE };
	int				m_hp	{100};
	float			m_speed	{5.0F};
	XMFLOAT2		m_pos	{};			// position
	float			m_dir	{ 1.0F };	// direction: left: -1, right:1
	XMVECTOR 		m_dif	{1.0F, 1.0F, 1.0F, 1.0F};	// color

	EAPP_MODEL		m_modelType	{ EMODEL_NONE };
	PG2OBJECT		m_modelObj	{};
public:
	virtual	void		Position(XMFLOAT2 v);
	virtual	XMFLOAT2	Position() const;
	virtual	void		Direction(float v);
	virtual	float		Direction() const;
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
	void		Init();
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
	void			InitPlayer();
	GamePlayer*		MainPlayer();
};

extern GameInfo* g_gameInfo;

#endif
