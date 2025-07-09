
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

	EAPP_MODEL		m_model	{ EMODEL_NONE };
	PG2OBJECT		m_obj	{};
public:
	void		Position(XMFLOAT2 v);
	XMFLOAT2	Position() const;
	void		Direction(float v);
	float		Direction() const;
	void		Speed(float v);
	float		Speed() const;

	void		Move();
	void		MoveLeft();
	void		MoveRight();

	virtual	void		Model(EAPP_MODEL v);
	virtual	EAPP_MODEL	Model() const;
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
	GamePlayer*		m_player	{};
	int				m_gameScore	{};

public:
	GameInfo();
	void InitPlayer();
};

extern GameInfo* g_gameInfo;

#endif
