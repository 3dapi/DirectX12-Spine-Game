
#include <algorithm>
#include <any>
#include <filesystem>
#include <functional>
#include <tuple>
#include <d3d12.h>
#include "Common/G2.FactoryCamera.h"
#include "Common/G2.FactoryTexture.h"
#include "Common/G2.FactoryShader.h"
#include "Common/G2.FactorySIgnature.h"
#include "Common/G2.FactoryPipelineState.h"
#include "Common/G2.Geometry.h"
#include "Common/G2.Util.h"
#include "Common/G2.FactorySpine.h"
#include "Common/GameTimer.h"
#include <pix.h>
#include "CommonStates.h"
#include "ScenePlay.h"
#include "DirectXHelpers.h"
#include "ResourceUploadBatch.h"
#include "GraphicsMemory.h"
#include "ScenePlay.h"
#include "SpineFactory.h"
#include "UiPlay.h"

using namespace std;
using std::any_cast;
using namespace DirectX;
using namespace spine;
using namespace G2;


ScenePlay::ScenePlay()
	: m_keyEvent(EAPP_MAX_KEY, 0)
{
}

ScenePlay::~ScenePlay()
{
	Destroy();
}

int ScenePlay::Init(const std::any& initial_value)
{
	int hr = S_OK;

	auto cameraSpine = FactoryCamera::instance()->FindRes(IG2Camera::SPINE_2D);
	if(cameraSpine)
	{
		cameraSpine->Position({0.0f, +200.0f,-700.0f});
		cameraSpine->LookAt  ({0.0f, +200.0f,   0.0f});
		cameraSpine->Update  ();
	}

	auto pGameInfo = GameInfo::instance();

	pGameInfo->StageInit();

	SAFE_DELETE(m_pUi);
	m_pUi = new UiPlay;
	if (!m_pUi)
	{
		return E_FAIL;
	}
	m_pUi->Init();

	hr = CreateMainPlayerModel();
	if (FAILED(hr))
		return hr;
	hr = StageInit();
	if (FAILED(hr))
		return hr;

	return S_OK;
}

int ScenePlay::Destroy()
{
	SAFE_DELETE_VECTOR(m_vecMob);
	SAFE_DELETE(m_pUi);

	return S_OK;
}

int ScenePlay::Update(const std::any& t)
{
	auto pGameInfo    = GameInfo::instance();
	auto playerState  = m_mainPlayer->State();
	auto playerPos    = m_mainPlayer->Position();
	auto curStageIndex = pGameInfo->CurrentStateIndex();

	GameTimer gt = std::any_cast<GameTimer>(t);
	auto dt = gt.DeltaTime();

	if(pGameInfo->m_stageIncrease)
	{
		pGameInfo->m_stageCur++;
		StageInit();
	}

	//------------------------------------------------------------------------------
	// 스테이지 변경 체크
	// 각각의 스테이별로 목표를 완수 했는가?
	if(!m_stageComplete && !m_stageChangeing)
	{
		if(pGameInfo->CurrentStateComplete())
		{
			m_stageComplete= true;
		}
	}

	//------------------------------------------------------------------------------
	// 스테이지 교체 시작
	if(m_stageComplete)
	{
		m_mainPlayer->State(EAPP_CHAR_STATE::ESTATE_CHAR_IDLE);
		//장면 전환
		StageChange(gt);
		return S_OK;
	}

	//------------------------------------------------------------------------------
	// 스테이지 교체 업데이트
	if(m_stageChangeing)
	{
		StageChangingUpdate(gt);
		return S_OK;
	}
	

	//------------------------------------------------------------------------------
	// 게임 종료 체크. 유저 HP == 0
	if (pGameInfo->m_enablePlay && 0 >= m_mainPlayer->HP())
	{
		pGameInfo->m_enablePlay = false;
		return S_OK;
	}

	//------------------------------------------------------------------------------
	// 지난 시간만큼 HP를 채운다.
	if (pGameInfo->m_enablePlay)
	{
		auto newHp = m_mainPlayer->HP() + dt * 100;
		if (100 < newHp)
			newHp = 100;
		m_mainPlayer->HP(newHp);
	}

	vector<function<void(void)> > StageUpdate
	{
		// state 0~3
		[&]()
		{
			// clear
			for(auto it = m_vecMob.begin(); it != m_vecMob.end(); )
			{
				if(nullptr == *it || 0 == (*it)->HP())
				{
					if(*it)
						delete (*it);
					it = m_vecMob.erase(it);
					continue;
				}
				++it;
			}
		},
		// state 4
		[&]() {
			// regen Mob
			for(size_t i = 0; i < m_vecMob.size(); ++i)
			{
				auto* mob = m_vecMob[i];
				if(!mob)
					continue;
				auto mob_hp = mob->HP();
				if(0< mob_hp)
					continue;

				SetupMobMovemoent(mob);
			}
		},
	};

	if(curStageIndex<4)
		StageUpdate[0]();
	else
		StageUpdate[1]();


	// 전투
	for (size_t i = 0; i < m_vecMob.size(); ++i)
	{
		auto* mob = m_vecMob[i];
		if(!mob)
			continue;

		auto mobPosition = mob->Position();
		if (EAPP_CHAR_STATE::ESTATE_CHAR_ATTACK == playerState && pGameInfo->IsCollisionPlayer(mob))
		{
			bool isTarget = false;
			// 플레이어 앞쪽만 타겟팅.
			if(0< m_mainPlayer->Direction())
			{
				if(mob->Position().x >= m_mainPlayer->Position().x)
				{
					isTarget = true;
				}
			}
			else
			{
				if (mob->Position().x <= m_mainPlayer->Position().x)
				{
					isTarget = true;
				}
			}

			if (isTarget)
			{
				if(m_mainPlayer->AnimationComplete("attack"))
				{
					pGameInfo->IncreaseScore(100);

					auto newHp = mob->HP() - m_mainPlayer->Damage();
					if (0 >= newHp)
					{
						newHp = 0;
						// mob kill 수를 올린다.
						pGameInfo->CurrentStateAdvancing(1);
					}
					mob->HP(newHp);
				}
			}
		}

		if (EAPP_CHAR_STATE::ESTATE_CHAR_ATTACK != playerState &&
			EAPP_CHAR_STATE::ESTATE_CHAR_IDLE != mob->State() &&
			0< mob->HP() &&
			pGameInfo->IsCollisionPlayer(mob)
			)
		{
			auto newHp = m_mainPlayer->HP() - mob->Damage();
			if (0 > newHp)
				newHp = 0;
			m_mainPlayer->HP(newHp);
		}

		mob->Update(gt);
	}

	if (pGameInfo->m_enablePlay)
	{
		bool isKeyEvent = false;
		// 이동.
		if (m_keyEvent[VK_LEFT] == EAPP_INPUT_PRESS)
		{
			isKeyEvent = true;
			m_mainPlayer->MoveLeft(dt);
		}

		if (m_keyEvent[VK_RIGHT] == EAPP_INPUT_PRESS)
		{
			isKeyEvent = true;
			m_mainPlayer->MoveRight(dt);
		}

		if (m_keyEvent[VK_UP] == EAPP_INPUT_PRESS)
		{
			isKeyEvent = true;
			m_mainPlayer->MoveUp(dt);
		}

		if (m_keyEvent[VK_DOWN] == EAPP_INPUT_PRESS)
		{
			isKeyEvent = true;
			m_mainPlayer->MoveDown(dt);
		}

		if(	m_keyEvent[VK_LEFT] == EAPP_INPUT_UP ||
			m_keyEvent[VK_RIGHT] == EAPP_INPUT_UP ||
			m_keyEvent[VK_UP] == EAPP_INPUT_UP ||
			m_keyEvent[VK_DOWN] == EAPP_INPUT_UP)
		{
			isKeyEvent = true;
			m_mainPlayer->State(EAPP_CHAR_STATE::ESTATE_CHAR_IDLE);
		}

		// attack.
		// 이동이 아닐때만 공격 가능
		if (!isKeyEvent && m_keyEvent['A'] == EAPP_INPUT_PRESS)
		{
			isKeyEvent = true;
			m_mainPlayer->State(EAPP_CHAR_STATE::ESTATE_CHAR_ATTACK);
		}

	}

	m_mainPlayer->Update(gt);
	
	// update ui
	m_pUi->Update(dt);
	return S_OK;
}

int ScenePlay::Render()
{
	auto pGameInfo = GameInfo::instance();

	m_pUi->Draw();


	{
		vector<SpineRender*> objRender;
		for (size_t i = 0; i < m_vecMob.size(); ++i)
		{
			auto* mob = m_vecMob[i];
			if (mob && 0 < mob->HP())
			{
				auto model = dynamic_cast<SpineRender*>(mob->ModelObject());
				if(model)
					objRender.push_back(model);
			}
		}
		if (pGameInfo->m_enablePlay)
		{
			auto model = dynamic_cast<SpineRender*>(m_mainPlayer->ModelObject());
			if (model)
				objRender.push_back(model);
		}

		std::sort(objRender.begin(), objRender.end(), [](SpineRender* a, SpineRender* b)
		{
			auto p0 = a->Position();
			auto p1 = b->Position();
			return p1.y < p0.y;
		});

		for (size_t i = 0; i < objRender.size(); ++i)
		{
			auto& obj = objRender[i];
			obj->Render();
		}
	}


	m_pUi->DrawFront();


	if(m_stageChangeing)
	{
		if(m_stageComplete)
			m_stageComplete = false;

		((UiPlay*)m_pUi)->StageChangingDraw();
	}

	return S_OK;
}

int ScenePlay::Notify(const std::string& name, const std::any& t)
{
	auto pGameInfo = GameInfo::instance();
	auto curStageIndex = pGameInfo->CurrentStateIndex();

	if(name == "KeyEvent")
	{
		auto keyState = any_cast<const uint8_t*>(t);
		std::copy(keyState, keyState + EAPP_MAX_KEY, m_keyEvent.begin());
	}
	else if (name == "MouseUp")
	{
		auto mousePos = any_cast<const ::POINT&>(t);
		if(m_stageChangeing)
		{
			if( (curStageIndex+1) == GameInfo::MAX_STAGE)
			{
				pGameInfo->m_enablePlay = false;
				IG2AppFrame::instance()->command(EAPP_CMD_CHANGE_SCENE, EAPP_SCENE::EAPP_SCENE_END);
			}
			else
			{
				GameInfo::instance()->IncreaseStage();
			}
		}

		if(!pGameInfo->m_enablePlay)
			IG2AppFrame::instance()->command(EAPP_CMD_CHANGE_SCENE, EAPP_SCENE::EAPP_SCENE_END);
	}

	return S_OK;
}

int ScenePlay::CreateMainPlayerModel()
{
	auto pGameInfo = GameInfo::instance();

	m_mainPlayer = pGameInfo->MainPlayer();
	if (!m_mainPlayer)
		return E_FAIL;

	//           model type  position scale  direction
	tuple<EAPP_MODEL, float, float> charModel
	{
		EAPP_MODEL::EMODEL_KNIGHT, 0.5F,  1.0F,
	};
	const auto& [model, scale, direction] = charModel;
	
	SPINE_ATTRIB* att = FactorySpineObject::FindSpineAttribute(model);
	if (!att)
		return E_FAIL;
	auto spineModel = new(std::nothrow) SpineRender;
	if (!spineModel)
		return E_FAIL;
	if (FAILED(spineModel->Init(*att)))
	{
		delete spineModel;
		return E_FAIL;
	}

	m_mainPlayer->Init(model, spineModel);
	m_mainPlayer->Scale(0.7F);

	return S_OK;
}

int ScenePlay::StageInit()
{
	auto pGameInfo = GameInfo::instance();
	auto* pCurStage = pGameInfo->CurrentState();
	auto  curStageIndex = pGameInfo->CurrentStateIndex();

	m_stageComplete = false;
	m_stageChangeing = false;
	pGameInfo->m_stageIncrease = false;

	vector< function<void(void)> > StageSetup
	{
		// stage 0
		[&]()
		{
			m_mainPlayer->Position({-600, 0});
			SAFE_DELETE_VECTOR(m_vecMob);
			m_vecMob.resize(pCurStage->mobMax,{});
			for(size_t i = 0; i < m_vecMob.size(); ++i)
			{
				int modelIndex = G2::randomRange((int)0,(int)pCurStage->mobType.size()-1);
				auto model = pCurStage->mobType[modelIndex];
				SPINE_ATTRIB* att_p = FactorySpineObject::FindSpineAttribute(model);
				if(!att_p)
					continue;
				auto spineModel = new(std::nothrow) SpineRender;
				if(!spineModel)
					continue;

				SPINE_ATTRIB att = *att_p;
				att.aniBegin = (float)G2::randomRange(0.0F,2.0F);
				if(FAILED(spineModel->Init(att)))
				{
					delete spineModel;
					continue;
				}
				auto mob = new GameMob;
				if(!mob)
				{
					delete spineModel;
					continue;
				}
				if(FAILED(mob->Init(model,spineModel)))
				{
					continue;
				};

				SetupMobMovemoent(mob);
				m_vecMob[i] = mob;
			}
		},
		// stage 3
		[&]()
		{
			m_mainPlayer->Position({0,0});
			SAFE_DELETE_VECTOR(m_vecMob);
			m_vecMob.resize(pCurStage->mobMax,{});
			for(size_t i = 0; i < m_vecMob.size(); ++i)
			{
				int modelIndex = G2::randomRange((int)0,(int)pCurStage->mobType.size()-1);
				auto model = pCurStage->mobType[modelIndex];
				SPINE_ATTRIB* att_p = FactorySpineObject::FindSpineAttribute(model);
				if(!att_p)
					continue;
				auto spineModel = new(std::nothrow) SpineRender;
				if(!spineModel)
					continue;

				SPINE_ATTRIB att = *att_p;
				att.aniBegin = (float)G2::randomRange(0.0F,2.0F);
				if(FAILED(spineModel->Init(att)))
				{
					delete spineModel;
					continue;
				}
				auto mob = new GameMob;
				if(!mob)
				{
					delete spineModel;
					continue;
				}
				if(FAILED(mob->Init(model,spineModel)))
				{
					continue;
				};

				SetupMobMovemoent(mob);
				m_vecMob[i] = mob;
			}
		},
	};

	if(2>= curStageIndex)
		StageSetup[0]();
	else
		StageSetup[1]();

	int hr = S_OK;
	return S_OK;
}

int ScenePlay::StageChange(const GameTimer& gt)
{
	m_stageChangeing = true;
	auto dt = gt.DeltaTime();

	m_mainPlayer->Update(gt);
	// update ui
	m_pUi->Update(dt);
	return 0;
}

int ScenePlay::StageChangingUpdate(const GameTimer& gt)
{
	m_mainPlayer->Update(gt);
	return 0;
}

int ScenePlay::StageComplete()
{
	return 0;
}

int ScenePlay::SetupMobMovemoent(GameMob* mob)
{
	auto pGameInfo = GameInfo::instance();
	auto mainPlayerPos = m_mainPlayer->Position();

	// mob 들이 서있기만 함.
	if(0 == pGameInfo->CurrentStateIndex())
	{
		float posx = G2::randomRange( 200.0F,  700.0F);
		float posy = G2::randomRange( -250.0F,  350.0F);
		float dir  = -1.0F;
		float scale = G2::randomRange(0.5F, 0.6F);
		mob->Init();
		mob->Position({ posx, posy });
		mob->Scale(scale);
		mob->State(EAPP_CHAR_STATE::ESTATE_CHAR_IDLE);
		mob->Direction(dir);
	}
	else if(1 == pGameInfo->CurrentStateIndex())
	{
		float posx = G2::randomRange(200.0F,700.0F);
		float posy = G2::randomRange(-250.0F,300.0F);
		float dir = -1.0F;
		float scale = G2::randomRange(0.5F,0.6F);
		mob->Init();
		mob->Position({posx,posy});
		mob->Scale(scale);
		mob->Direction(dir);
	}
	else if(2 == pGameInfo->CurrentStateIndex())
	{
		float posx = G2::randomRange(200.0F,700.0F);
		float posy = G2::randomRange(-250.0F,300.0F);
		float dir = -1.0F;
		float scale = G2::randomRange(0.5F,0.6F);
		mob->Init();
		mob->Position({posx,posy});
		mob->Scale(scale);
		mob->Direction(dir);
	}
	else
	{
		float posx_plus   = G2::randomRange( 600.0F,  900.0F);
		float posx_minus  = G2::randomRange(-900.0F, -600.0F);
		int   posx_choise = G2::randomRange(0, 1);
		float posx = posx_choise ? posx_plus : posx_minus;

		float posy_plus   = G2::randomRange( 40.0F,  80.0F);
		float posy_minus  = G2::randomRange(-80.0F, -40.0F);
		int   posy_choise = G2::randomRange(0, 1);
		float posy = posy_choise ? posy_plus : posy_minus;

		float dir = posx  < mainPlayerPos.x ?  1.0F : -1.0F;

		float scale = G2::randomRange(0.4F, 0.8F);

		mob->Init();	// 모델 교체는 없이, 초기화만 다시 진행.
		mob->Position({ posx, posy });
		mob->Scale(scale);
		mob->Direction(dir);
	}

	return S_OK;
}
