
#pragma once
#ifndef _GameCharacter_H_
#define _GameCharacter_H_

#include <map>
#include <string>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include "Common/G2.Constants.h"
#include "common/G2.ConstantsWin.h"
#include "AppCommon.h"

using namespace std;
using namespace G2;

class GameInfo;
class GameCharacter
{
	friend GameInfo;
protected:
	EAPP_CHAR_STATE	m_state	{ EAPP_CHAR_STATE::ESTATE_CHAR_IDLE };
	float			m_hp		{100};
	float			m_damage	{50};
	XMFLOAT2		m_pos		{};			// position
	float			m_dir		{ 1.0F };	// direction: left: -1, right:1
	float			m_scale		{ 1.0F };	// model scale
	float			m_speed		{ 400.0F };	// move speed
	XMFLOAT4		m_dif		{1.0F, 1.0F, 1.0F, 1.0F};	// model color

	XMFLOAT2		m_boundBox	{ 160.0F, 230.0F };

	EAPP_MODEL		m_modelType	{ EAPP_MODEL::EMODEL_NONE };
	PG2OBJECT		m_modelObj	{};
public:
	virtual ~GameCharacter();

	virtual	void			State(EAPP_CHAR_STATE v);
	virtual	EAPP_CHAR_STATE	State() const;

	virtual	void		HP(float v);
	virtual	float		HP() const;
	virtual	void		Damage(float v);
	virtual	float		Damage() const;

	virtual	void		Position(XMFLOAT2 v);
	virtual	XMFLOAT2	Position() const;
	virtual	void		Direction(float v);
	virtual	float		Direction() const;
	virtual	void		Scale(float v);
	virtual	float		Scale() const;
	virtual	void		Speed(float v);
	virtual	float		Speed() const;

	virtual	void		Move(float dt);
	virtual	void		MoveLeft(float dt);
	virtual	void		MoveRight(float dt);
	virtual	void		MoveUp(float dt);
	virtual	void		MoveDown(float dt);

	virtual	void		ModelType(EAPP_MODEL v);
	virtual	EAPP_MODEL	ModelType() const;

	virtual	void		ModelObject(PG2OBJECT v);
	virtual	PG2OBJECT	ModelObject() const;
};

class GamePlayer : public GameCharacter, public IG2Listener
{
protected:
	int		m_attackRepeat = 3;
	map<string, bool>	m_aniComplete;
public:
	GamePlayer();
	int		Init(EAPP_MODEL modelType, PG2OBJECT modelObj, EAPP_CHAR_STATE state= EAPP_CHAR_STATE::ESTATE_CHAR_IDLE);
	int		Update(const GameTimer& gt);
	int		Render();
	int		Notify(const std::string&, const std::any&) override;
	void	State(EAPP_CHAR_STATE v) override;
	EAPP_CHAR_STATE	State() const override;

	bool	AnimationComplete(const string& aniName)
	{
		auto itr = m_aniComplete.find(aniName);
		if( itr != m_aniComplete.end())
			return itr->second;
		return false;
	}
};

class GameMob : public GameCharacter, public IG2Listener
{
protected:
public:
	GameMob();
	int		Init(EAPP_MODEL modelType = EAPP_MODEL::EMODEL_NONE, PG2OBJECT modelObj=nullptr, EAPP_CHAR_STATE state = EAPP_CHAR_STATE::ESTATE_CHAR_MOVE);
	int		Update(const GameTimer& gt);
	int		Render();
	int		Notify(const std::string&, const std::any&) override;
};

#endif
