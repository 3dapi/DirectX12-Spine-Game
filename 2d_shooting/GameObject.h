
#pragma once
#ifndef _GameObject_H_
#define _GameObject_H_

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
class GameObject
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

	XMFLOAT2		m_boundBox	{ 200.0F, 45.0F };
	EAPP_MODEL		m_modelType	{ EAPP_MODEL::EMODEL_NONE };

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
	virtual	void		Direction(float v);
	virtual	float		Direction() const;
	virtual	void		Scale(float v);
	virtual	float		Scale() const;
	virtual	void		Speed(float v);
	virtual	float		Speed() const;

	virtual	void		Model(EAPP_MODEL v) { m_modelType =v;}
	virtual	EAPP_MODEL	Model() const { return m_modelType;}

	virtual	void		Move(float dt);
	virtual	void		MoveLeft(float dt);
	virtual	void		MoveRight(float dt);
	virtual	void		MoveUp(float dt);
	virtual	void		MoveDown(float dt);
};

class GamePlayer : public GameObject, public IG2Listener
{
protected:
	int		m_attackRepeat = 3;
	map<string, bool>	m_aniComplete;

	PG2AUDIOPLAYER		m_audio	{};
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

class GameMob : public GameObject, public IG2Listener
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
