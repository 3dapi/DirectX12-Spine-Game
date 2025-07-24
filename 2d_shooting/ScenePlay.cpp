
#include "Common/G2.FactoryCamera.h"
#include "Common/G2.FactoryTexture.h"
#include "Common/G2.FactoryShader.h"
#include "Common/G2.FactorySIgnature.h"
#include "Common/G2.FactoryPipelineState.h"
#include "Common/G2.InputManager.h"
#include "Common/G2.Geometry.h"
#include "Common/G2.Util.h"
#include "Common/GameTimer.h"
#include <pix.h>
#include "CommonStates.h"
#include "ScenePlay.h"
#include "DirectXHelpers.h"
#include "ResourceUploadBatch.h"
#include "GraphicsMemory.h"
#include "ScenePlay.h"
#include "UiPlay.h"
#include "UiBackground.h"

using namespace std;
using std::any_cast;
using namespace DirectX;
using namespace G2;


ScenePlay::ScenePlay()
{
}

ScenePlay::~ScenePlay()
{
	Destroy();
}

int ScenePlay::Init(const std::any& initial_value)
{
	int hr = S_OK;

	auto d3d        = IG2GraphicsD3D::instance();
	auto device     = std::any_cast<ID3D12Device*             >(d3d->getDevice());
	auto cmdList    = std::any_cast<ID3D12GraphicsCommandList*>(d3d->getCommandList());
	auto cmdQue     = std::any_cast<ID3D12CommandQueue*       >(d3d->getCommandQueue());
	UINT descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	
	// texture setup
	vector<tuple<string, string> >  uiTextureList
	{
		{EMODEL_SHIP[0], "asset/sprite/" + EMODEL_SHIP[0] + ".png"  },
		{EMODEL_SHIP[1], "asset/sprite/" + EMODEL_SHIP[1] + ".png"  },
	};
	for(const auto& itr :EMODEL_DRONE)
	{
		uiTextureList.push_back(make_tuple(itr, "asset/sprite/" + itr + ".png"));
	}
	for(const auto& itr :EMODEL_BULLET)
	{
		uiTextureList.push_back(make_tuple(itr, "asset/sprite/" + itr + ".png"));
	}
	for(const auto& itr :EMODEL_BOSS)
	{
		uiTextureList.push_back(make_tuple(itr, "asset/sprite/" + itr + ".png"));
	}

	auto texManager = FactoryTexture::instance();
	for(const auto& [name, file]: uiTextureList)
	{
		auto r = texManager->Load(name, file);
		r->name;
		m_srvTex.insert(std::make_pair(r->name, UI_TEXTURE{ r->r, r->size, {} }));
	}

	m_srvHeap = G2::CreateDescHeap((UINT)m_srvTex.size() + 1);
	auto hCpu = m_srvHeap->GetCPUDescriptorHandleForHeapStart();
	auto hGpu = m_srvHeap->GetGPUDescriptorHandleForHeapStart();
	for (auto& itr : m_srvTex)
	{
		decltype(itr.second.res) res = itr.second.res;
		device->CreateShaderResourceView(res, nullptr, hCpu);
		itr.second.hGpu = hGpu;

		// 다음 리소스 대응.
		hCpu.ptr += descriptorSize;
		hGpu.ptr += descriptorSize;
	}

	auto pGameInfo = GameInfo::instance();

	pGameInfo->MainPlayer()->Init();

	pGameInfo->StageInit();

	SAFE_DELETE(m_pUi);
	m_pUi = new UiPlay;
	if (!m_pUi)
	{
		return E_FAIL;
	}
	m_pUi->Init();

	SAFE_DELETE(m_pUiBg);
	m_pUiBg = new UiBackground;
	if(!m_pUiBg)
	{
		return E_FAIL;
	}
	m_pUiBg->Init();

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
	m_srvHeap.Reset();
	m_srvTex.clear();

	SAFE_DELETE_VECTOR(m_vecMob);
	SAFE_DELETE(m_pUi);
	SAFE_DELETE(m_pUiBg);

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
	if(!m_stageComplete && !m_stageChanging)
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
		//장면 전환
		StageChange(gt);
		return S_OK;
	}

	//------------------------------------------------------------------------------
	// 스테이지 교체 업데이트
	if(m_stageChanging)
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
		//m_mainPlayer->HP(newHp);
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
			// 죽은 몹 재생
			for(size_t i=0; i<m_vecMob.size(); ++i)
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
	for (size_t i=0; i<m_vecMob.size(); ++i)
	{
		auto* mob = m_vecMob[i];
		if(!mob)
			continue;
		mob->Update(gt);
	}

	if (pGameInfo->m_enablePlay)
	{
		bool hasKeyEvent = InputManager::instance()->hasEvent();
		auto const keyEvent = InputManager::instance()->Key();
		// 이동.
		if (keyEvent[VK_LEFT] == EAPP_INPUT_PRESS)
		{
			m_mainPlayer->MoveLeft(dt);
		}

		if (keyEvent[VK_RIGHT] == EAPP_INPUT_PRESS)
		{
			m_mainPlayer->MoveRight(dt);
		}

		if (keyEvent[VK_UP] == EAPP_INPUT_PRESS)
		{
			m_mainPlayer->MoveUp(dt);
		}

		if (keyEvent[VK_DOWN] == EAPP_INPUT_PRESS)
		{
			m_mainPlayer->MoveDown(dt);
		}

		// bullet 발사
		if (keyEvent[VK_SPACE] == EAPP_INPUT_UP)
		{
		}

	}

	m_mainPlayer->Update(gt);
	
	// update ui
	m_pUi->Update(dt);
	m_pUiBg->Update(dt);

	return S_OK;
}

int ScenePlay::Render()
{
	auto pGameInfo = GameInfo::instance();
	m_pUiBg->Draw();

	auto d3d          = IG2GraphicsD3D::instance();
	auto cmdList      = std::any_cast<ID3D12GraphicsCommandList*>(d3d->getCommandList());
	auto sprite		  = std::any_cast<SpriteBatch*>(IG2AppFrame::instance()->getAttrib(EAPP_ATTRIB::EAPP_ATT_XTK_SPRITE));
	::SIZE screenSize = *any_cast<::SIZE*>(d3d->getAttrib(ATT_SCREEN_SIZE));

	ID3D12DescriptorHeap* heaps[] = {m_srvHeap.Get()};
	cmdList->SetDescriptorHeaps(1, heaps);

	sprite->Begin(cmdList);
	{
		// draw enemy drone
		{

		}
		// draw player
		{
			auto modelName = m_mainPlayer->Model();
			auto& tex = m_srvTex[modelName];

			auto pos = m_mainPlayer->Position();
			XMFLOAT2 origin = {0, 0};
			XMFLOAT2 scale = {1.0F, 1.0F};
			XMFLOAT2 position = {pos.x - tex.size.x/2, pos.x - tex.size.y/2};
			PositionToOtho(position);
			sprite->Draw(tex.hGpu, tex.size, position, nullptr, XMVECTORF32{{{1.F, 1.F, 1.F, 1.0F}}}, 0.0F, origin, scale);
		}
		// draw bullet
		{

		}
	}
	sprite->End();

	m_pUi->Draw();
	m_pUi->DrawFront();


	if(m_stageChanging)
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

	if (name == "MouseUp")
	{
		auto mousePos = any_cast<const ::POINT&>(t);
		if(m_stageChanging)
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

	return S_OK;
}

int ScenePlay::StageInit()
{
	auto pGameInfo = GameInfo::instance();
	auto* pCurStage = pGameInfo->CurrentState();
	auto  curStageIndex = pGameInfo->CurrentStateIndex();

	m_stageComplete = false;
	m_stageChanging = false;
	pGameInfo->m_stageIncrease = false;

	vector< function<void(void)> > StageSetup
	{
		// stage 0
		[&]()
		{
		},
		// stage 3
		[&]()
		{
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
	m_stageChanging = true;
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

int ScenePlay::SetupMobMovemoent(EnemyDrone* mob)
{
	auto pGameInfo = GameInfo::instance();
	auto mainPlayerPos = m_mainPlayer->Position();

	// mob 들이 서있기만 함.
	mob->Init(pGameInfo->CurrentStateIndex());

	return S_OK;
}

void ScenePlay::PositionToOtho(XMFLOAT2& pos)
{
	auto d3d = IG2GraphicsD3D::instance();
	::SIZE screenSize = *any_cast<::SIZE*>(d3d->getAttrib(ATT_SCREEN_SIZE));
	pos.x += screenSize.cx * 0.5f;
	pos.y  = -pos.y + screenSize.cy * 0.5f;
}
