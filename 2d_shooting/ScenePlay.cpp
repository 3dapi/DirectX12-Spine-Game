
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

int ScenePlay::Init(const std::any&)
{
	int hr = S_OK;

	Destroy();

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
	m_mainPlayer = pGameInfo->MainPlayer();
	m_mainPlayer->Init();

	m_vecDrone.resize(MAX_DRONE, nullptr);
	std::generate(m_vecDrone.begin(), m_vecDrone.end(), [](){ return new EnemyDrone;});

	m_pUi = new UiPlay;
	if (!m_pUi)
	{
		return E_FAIL;
	}
	m_pUi->Init();

	m_pUiBg = new UiBackground;
	if(!m_pUiBg)
	{
		return E_FAIL;
	}
	m_pUiBg->Init();

	return S_OK;
}

int ScenePlay::Destroy()
{
	m_srvHeap.Reset();
	m_srvTex.clear();

	SAFE_DELETE_VECTOR(m_vecDrone);
	SAFE_DELETE(m_pUi);
	SAFE_DELETE(m_pUiBg);

	return S_OK;
}

int ScenePlay::Update(const std::any& t)
{
	auto pGameInfo    = GameInfo::instance();
	auto playerState  = m_mainPlayer->State();
	auto playerPos    = m_mainPlayer->Position();

	GameTimer gt = std::any_cast<GameTimer>(t);
	auto dt = gt.DeltaTime();

	// 타임 누적
	m_timeStored += dt;
	m_timeDrone	 += dt;

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

	// setup play state
	if(2<m_timeStored && PLAY_STATE::BEGIN == m_playState)
	{
		m_playState = PLAY_STATE::LOW;
	}
	else if(30<m_timeStored && PLAY_STATE::LOW == m_playState)
	{
		m_playState = PLAY_STATE::MIDDLE;
	}
	else if(120<m_timeStored && PLAY_STATE::MIDDLE == m_playState)
	{
		m_playState = PLAY_STATE::HI;
	}
	else if(240<m_timeStored && PLAY_STATE::HI == m_playState)
	{
		m_playState = PLAY_STATE::BOSS;
	}

	// update enemy
	UpdateEnemy(t);

	// check input event
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

	// update player
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
		for(auto& drone : m_vecDrone)
		{
			if(!drone->Alive())
				continue;

			auto modelName = drone->Model();
			auto& tex = m_srvTex[modelName];

			auto pos = drone->Position();
			XMFLOAT2 origin = {tex.size.x/2.0F, tex.size.y/2.0F};
			XMFLOAT2 scale = {1.0F, 1.0F};
			XMFLOAT2 position = G2::ScreenToGameCoord(pos);
			sprite->Draw(tex.hGpu, tex.size, position, nullptr, XMVECTORF32{{{1.F, 1.F, 1.F, 1.0F}}}, 0.0F, origin, scale);
		}
		// draw player
		{
			auto modelName = m_mainPlayer->Model();
			auto& tex = m_srvTex[modelName];

			auto pos = m_mainPlayer->Position();
			XMFLOAT2 origin = {tex.size.x/2.0F, tex.size.y/2.0F};
			XMFLOAT2 scale = {1.0F, 1.0F};
			XMFLOAT2 position = G2::ScreenToGameCoord(pos);
			sprite->Draw(tex.hGpu, tex.size, position, nullptr, XMVECTORF32{{{1.F, 1.F, 1.F, 1.0F}}}, 0.0F, origin, scale);
		}
		// draw bullet
		{

		}
	}
	sprite->End();

	m_pUi->Draw();
	m_pUi->DrawFront();

	return S_OK;
}

int ScenePlay::Notify(const std::string& name, const std::any& t)
{
	auto pGameInfo = GameInfo::instance();

	if (name == "MouseUp")
	{
		auto mousePos = any_cast<const ::POINT&>(t);
		if(m_playState == PLAY_STATE::END)
		{
			IG2AppFrame::instance()->command(EAPP_CMD_CHANGE_SCENE, EAPP_SCENE::EAPP_SCENE_END);
		}
	}
	return S_OK;
}

int ScenePlay::UpdateEnemy(const std::any& t)
{
	GameTimer gt = std::any_cast<GameTimer>(t);

	vector<function<void(void)> > enemyGen
	{
		[&](){},
		[&]()
		{
			// PLAY_STATE::LOW
			for(auto& drone : m_vecDrone)
			{
				if(drone && !drone->Alive())
				{
					int indexModel = G2::randomRange(0, 1);
					T_KINETICS kt{};
					kt.dif = XMVECTORF32{{{1.0F, 1.0F, 1.0F, 1.0F}}};
					kt.alive = true;
					kt.pos = XMFLOAT2{G2::randomRange(-260.0F, +260.0F), G2::randomRange(550.0F, +750.0F)};
					kt.vlc = XMFLOAT2{0.0F, G2::randomRange(-400.0F, -250.0F)};
					drone->Init((int)PLAY_STATE::LOW, kt);
					drone->Model(EMODEL_DRONE[indexModel]);
					break;
				}
			}
		},
		[&]()
		{
			// PLAY_STATE::MIDDLE
			for(auto& drone : m_vecDrone)
			{
				if(drone && !drone->Alive())
				{
					int indexModel = G2::randomRange(2, 3);
					T_KINETICS kt{};
					kt.dif = XMVECTORF32{{{1.0F, 1.0F, 1.0F, 1.0F}}};
					kt.alive = true;
					kt.pos = XMFLOAT2{G2::randomRange(-260.0F, +260.0F), G2::randomRange(550.0F, +750.0F)};
					kt.vlc = XMFLOAT2{0.0F, G2::randomRange(-400.0F, -350.0F)};
					drone->Init((int)PLAY_STATE::LOW, kt);
					drone->Model(EMODEL_DRONE[indexModel]);
					break;
				}
			}
		},
		[&]()
		{
			// PLAY_STATE::HI
			for(auto& drone : m_vecDrone)
			{
				if(drone && !drone->Alive())
				{
					int indexModel = G2::randomRange(2, 3);
					T_KINETICS kt{};
					kt.dif = XMVECTORF32{{{1.0F, 1.0F, 1.0F, 1.0F}}};
					kt.alive = true;
					kt.pos = XMFLOAT2{G2::randomRange(-260.0F, +260.0F), G2::randomRange(550.0F, +750.0F)};
					kt.vlc = XMFLOAT2{0.0F, G2::randomRange(-500.0F, -450.0F)};
					drone->Init((int)PLAY_STATE::LOW, kt);
					drone->Model(EMODEL_DRONE[indexModel]);
					break;
				}
			}
		},
	};

	// 0.1초마다 적들을 생산함
	if(PLAY_STATE::LOW == m_playState)
	{
		if(0.6F < m_timeDrone)
		{
			m_timeDrone -= 0.6F;
			enemyGen[(int)m_playState]();
		}
	}

	if(PLAY_STATE::MIDDLE == m_playState)
	{
		if(0.35F < m_timeDrone)
		{
			m_timeDrone -= 0.35F;
			enemyGen[(int)m_playState]();
		}
	}

	if(PLAY_STATE::HI == m_playState)
	{
		if(0.15F < m_timeDrone)
		{
			m_timeDrone -= 0.15F;
			enemyGen[(int)m_playState]();
		}
	}

	// 전투
	for(auto& drone : m_vecDrone)
	{
		if(!drone || !drone->Alive())
			continue;
		drone->Update(gt);
		if(-550 > drone->Position().y)
		{
			drone->Alive(false);
		}
	}

	return S_OK;
}
