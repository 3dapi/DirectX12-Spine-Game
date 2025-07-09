//--------------------------------------------------------------------------------------------------------------------------------------------------------------
// MainApp

#include <string>
#include <tuple>
#include <d3d12.h>
#include "MainApp.h"
#include "ResourceUploadBatch.h"
#include "Common/G2.FactoryTexture.h"
#include "Common/G2.FactoryShader.h"
#include "Common/G2.FactorySIgnature.h"
#include "Common/G2.FactoryPipelineState.h"
#include "Common/G2.FactorySpine.h"
#include "Common/G2.Geometry.h"
#include "Common/G2.Util.h"
#include "SceneGameMesh.h"
#include "SceneXtkGame.h"
#include "SceneSpine.h"
#include "SceneSample2D.h"
#include "SceneBegin.h"
#include "SceneLobby.h"
#include "ScenePlay.h"
#include "GameInfo.h"

void GameCharacter::Model(EAPP_MODEL v)
{
	// find model 
	m_model = v;
}
EAPP_MODEL GameCharacter::Model() const
{
	return m_model;
}

void GameCharacter::Position(XMFLOAT2 v)
{
	m_pos = v;
}
XMFLOAT2 GameCharacter::Position() const
{
	return m_pos;
}

void GameCharacter::Direction(float v)
{
	m_dir = v;
}
float GameCharacter::Direction() const
{
	return m_dir;
}
void GameCharacter::Speed(float v)
{
	m_speed = v;
}
float GameCharacter::Speed() const
{
	return m_speed;
}

void GameCharacter::Move()
{
	m_pos.x += m_speed;
	m_pos.y += m_speed;
}

void GameCharacter::MoveLeft()
{
	m_pos.x += m_speed * (-1.0F);
	m_pos.y += m_speed * (-1.0F);
}

void GameCharacter::MoveRight()
{
	m_pos.x += m_speed * (+1.0F);
	m_pos.y += m_speed * (+1.0F);
}

void GamePlayer::Init()
{
	m_model = EMODEL_NONE;
	m_hp    = 100;
	m_speed = 5.0F;
	m_pos   = XMFLOAT2{ 0.0F, 0.0F };
	m_dir   = 1.0F;
	m_dif   = XMVectorSet( 1.0F, 1.0F, 1.0F, 1.0F );
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

GameMob::GameMob()
{
	m_speed = 3.0F;
}

GameInfo* g_gameInfo = new GameInfo;

GameInfo::GameInfo()
{
	m_player = new GamePlayer;
}

void GameInfo::InitPlayer()
{
	m_player->Init();
}
