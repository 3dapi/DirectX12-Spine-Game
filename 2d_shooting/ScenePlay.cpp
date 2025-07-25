
#include "Common/G2.FactoryMfAudio.h"
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
	
	// for debugging
	auto texManager = FactoryTexture::instance();
	auto r = texManager->Find(FactoryTexture::RES_DEBUUGING);
	r->name;
	m_srvTex.insert(std::make_pair(r->name, UI_TEXTURE{r->r, r->size, {}}));

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
	for(const auto& itr :EMODEL_MISSILE)
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

	for(const auto& [name, file]: uiTextureList)
	{
		auto r = texManager->Load(name, file);
		r->name;
		m_srvTex.insert(std::make_pair(r->name, UI_TEXTURE{ r->r, r->size, {} }));
	}

	m_srvHeap = G2::CreateDescHeap((UINT)m_srvTex.size() + 100);
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
	{
		auto modelName = m_mainPlayer->Model();
		auto& tex = m_srvTex[modelName];
		XMFLOAT2 box{(float)tex.size.x, (float)tex.size.y};
		m_mainPlayer->Box(box);
	}


	m_vecDrone.resize(MAX_DRONE, nullptr);
	std::generate(m_vecDrone.begin(), m_vecDrone.end(), [](){ return new EnemyDrone;});

	m_vecBulletEnemy.resize(MAX_BULLET_ENEMY, nullptr);
	std::generate(m_vecBulletEnemy.begin(), m_vecBulletEnemy.end(), [](){ return new GameBullet; });

	m_vecBulletPlayer.resize(MAX_BULLET_PLAYER, nullptr);
	std::generate(m_vecBulletPlayer.begin(), m_vecBulletPlayer.end(), [](){ return new GameBullet; });

	m_sndLaser.resize(MAX_SND_EFFECT, nullptr);
	std::generate(m_sndLaser.begin(), m_sndLaser.end(), []() { return MfAudioPlayer::Create("asset/sound/laser_02.wav");});

	m_sndBoom .resize(MAX_SND_EFFECT, nullptr);
	std::generate(m_sndBoom.begin(), m_sndBoom.end(), []() { return MfAudioPlayer ::Create("asset/sound/bakuhatu29.wav");});

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
	SAFE_DELETE(m_vecMobBoss);
	SAFE_DELETE_VECTOR(m_vecBulletEnemy);
	SAFE_DELETE_VECTOR(m_vecBulletPlayer);
	SAFE_DELETE_VECTOR(m_sndLaser);
	SAFE_DELETE_VECTOR(m_sndBoom);
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
		SetStateEnd();
		return S_OK;
	}

	//------------------------------------------------------------------------------
	// 지난 시간만큼 HP를 채운다.
	if (pGameInfo->m_enablePlay)
	{
		auto newHp = m_mainPlayer->HP() + dt * 10;
		if (500 < newHp)
			newHp = 500;
		m_mainPlayer->HP(newHp);
	}

	// setup play state
	{
		if(1<m_timeStored && PLAY_STATE::BEGIN == m_playState)
		{
			m_playState = PLAY_STATE::LOW;
		}
		else if(10<m_timeStored && PLAY_STATE::LOW == m_playState)
		{
			m_playState = PLAY_STATE::MIDDLE;
		}
		else if(20<m_timeStored && PLAY_STATE::MIDDLE == m_playState)
		{
			m_playState = PLAY_STATE::HI;
		}
		else if(40<m_timeStored && PLAY_STATE::HI == m_playState)
		{
			//m_playState = PLAY_STATE::BOSS;
			SetStateEnd();
		}
	}

	// update enemy
	UpdateEnemy(t);

	// update enemy
	BulletUpdate(t);

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
			BulletFire(m_mainPlayer, true);
		}

		if(GameInfo::M_CHEAT)
		{
			if(keyEvent[VK_F2] == EAPP_INPUT_UP)
			{
				m_mainPlayer->HP(500);
			}
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
			auto box = drone->Box();

			XMFLOAT2 origin = {tex.size.x/2.0F, tex.size.y/2.0F};
			XMFLOAT2 scale = {1.0F, 1.0F};
			XMFLOAT2 position = G2::GameCoordToScreen(pos);

			XMFLOAT2 begin = G2::GameCoordToScreen({pos.x - box.x/2, pos.y - box.y/2});
			XMFLOAT2 end = G2::GameCoordToScreen({pos.x + box.x/2, pos.y + box.y/2});

			sprite->Draw(tex.hGpu, tex.size, position, nullptr, XMVECTORF32{{{1.F, 1.F, 1.F, 1.0F}}}, 0.0F, origin, scale);
			RenderDebugging(sprite, begin, end, XMVECTORF32{{{1.F, 1.F, 0.F, 0.6F}}});
		}
		// draw player bullet
		for(auto& bullet : m_vecBulletPlayer)
		{
			if(!bullet->alive)
				continue;

			auto modelName = bullet->m_model;
			auto& tex = m_srvTex[modelName];

			auto pos = bullet->pos;
			auto box = bullet->box;

			XMFLOAT2 origin = {tex.size.x/2.0F, tex.size.y/2.0F};
			XMFLOAT2 scale = {1.0F, 1.0F};
			XMFLOAT2 position = G2::GameCoordToScreen(pos);

			XMFLOAT2 begin = G2::GameCoordToScreen({pos.x - box.x/2, pos.y - box.y/2});
			XMFLOAT2 end = G2::GameCoordToScreen({pos.x + box.x/2, pos.y + box.y/2});

			sprite->Draw(tex.hGpu, tex.size, position, nullptr, XMVECTORF32{{{1.F, 1.F, 1.F, 1.0F}}}, 0.0F, origin, scale);
			RenderDebugging(sprite, begin, end, XMVECTORF32{{{1.F, 0.F, 0.F, 0.4F}}});
		}

		// draw player
		{
			auto modelName = m_mainPlayer->Model();
			auto& tex = m_srvTex[modelName];

			auto pos = m_mainPlayer->Position();
			auto box = m_mainPlayer->Box();
			XMFLOAT2 origin = {tex.size.x/2.0F, tex.size.y/2.0F};
			XMFLOAT2 scale = {1.0F, 1.0F};
			XMFLOAT2 position = G2::GameCoordToScreen(pos);

			XMFLOAT2 begin = G2::GameCoordToScreen({pos.x - box.x/2, pos.y - box.y/2});
			XMFLOAT2 end   = G2::GameCoordToScreen({pos.x + box.x/2, pos.y + box.y/2});

			sprite->Draw(tex.hGpu, tex.size, position, nullptr, XMVECTORF32{{{1.F, 1.F, 1.F, 1.0F}}}, 0.0F, origin, scale);
			RenderDebugging(sprite, begin, end);
		}

		// draw enemy bullet
		for(auto& bullet : m_vecBulletEnemy)
		{
			if(!bullet->alive)
				continue;

			auto modelName = bullet->m_model;
			auto& tex = m_srvTex[modelName];

			auto pos = bullet->pos;
			XMFLOAT2 origin = {tex.size.x/2.0F, tex.size.y/2.0F};
			XMFLOAT2 scale = {1.0F, 1.0F};
			XMFLOAT2 position = G2::GameCoordToScreen(pos);
			sprite->Draw(tex.hGpu, tex.size, position, nullptr, XMVECTORF32{{{1.F, 1.F, 1.F, 1.0F}}}, 0.0F, origin, scale);
		}
	}
	sprite->End();

	m_pUi->Draw();
	m_pUi->DrawFront();

	return S_OK;
}

void ScenePlay::RenderDebugging(SpriteBatch* sprite, const XMFLOAT2& from, const XMFLOAT2& to, const XMVECTORF32& color)
{
	return;
	auto& tex = m_srvTex[FactoryTexture::RES_DEBUUGING];
	auto r = (LONG)std::fabsf(from.x - to.x);
	auto b = (LONG)std::fabsf(from.y - to.y);
	::RECT rc {0, 0, r, b};
	XMFLOAT2 pos { std::min(from.x, to.x), std::min(from.y, to.y)};
	sprite->Draw(tex.hGpu, tex.size, pos, &rc, color);
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

void ScenePlay::SetStateEnd()
{
	GameInfo::instance()->m_enablePlay = false;
	m_playState = PLAY_STATE::END;
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
					T_KINETICS kt{};

					int indexModel = G2::randomRange(0, 1);
					drone->Model(EMODEL_DRONE[indexModel]);
					{
						auto modelName = drone->Model();
						auto& tex = m_srvTex[modelName];
						XMFLOAT2 box{(float)tex.size.x, (float)tex.size.y};
						kt.box = box;
					}
					kt.dif = XMVECTORF32{{{1.0F, 1.0F, 1.0F, 1.0F}}};
					kt.alive = true;
					kt.pos = XMFLOAT2{G2::randomRange(-260.0F, +260.0F), G2::randomRange(550.0F, +750.0F)};
					kt.vlc = XMFLOAT2{0.0F, G2::randomRange(-400.0F, -250.0F)};
					drone->Init((int)PLAY_STATE::LOW, kt);
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
					T_KINETICS kt{};

					int indexModel = G2::randomRange(2, 3);
					drone->Model(EMODEL_DRONE[indexModel]);
					{
						auto modelName = drone->Model();
						auto& tex = m_srvTex[modelName];
						XMFLOAT2 box{(float)tex.size.x, (float)tex.size.y};
						kt.box = box;
					}
					kt.dif = XMVECTORF32{{{1.0F, 1.0F, 1.0F, 1.0F}}};
					kt.alive = true;
					kt.pos = XMFLOAT2{G2::randomRange(-260.0F, +260.0F), G2::randomRange(550.0F, +750.0F)};
					kt.vlc = XMFLOAT2{0.0F, G2::randomRange(-400.0F, -350.0F)};
					drone->Init((int)PLAY_STATE::MIDDLE, kt);
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
					T_KINETICS kt{};

					int indexModel = G2::randomRange(2, 3);
					drone->Model(EMODEL_DRONE[indexModel]);
					{
						auto modelName = drone->Model();
						auto& tex = m_srvTex[modelName];
						XMFLOAT2 box{(float)tex.size.x, (float)tex.size.y};
						kt.box = box;
					}
					kt.dif = XMVECTORF32{{{1.0F, 1.0F, 1.0F, 1.0F}}};
					kt.alive = true;
					kt.pos = XMFLOAT2{G2::randomRange(-260.0F, +260.0F), G2::randomRange(550.0F, +750.0F)};
					kt.vlc = XMFLOAT2{0.0F, G2::randomRange(-500.0F, -450.0F)};
					drone->Init((int)PLAY_STATE::HI, kt);
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
	else if(PLAY_STATE::MIDDLE == m_playState)
	{
		if(0.35F < m_timeDrone)
		{
			m_timeDrone -= 0.35F;
			enemyGen[(int)m_playState]();
		}
	}
	else if(PLAY_STATE::HI == m_playState)
	{
		if(0.15F < m_timeDrone)
		{
			m_timeDrone -= 0.15F;
			enemyGen[(int)m_playState]();
		}
	}

	// update drone
	for(auto& drone : m_vecDrone)
	{
		if(!drone || !drone->Alive())
			continue;
		drone->Update(gt);

		// drone 에서 탄환 발사.
		bool filedBullet = drone->m_firedBullet;
		if(filedBullet)
		{
			drone->m_firedBullet = false;
			BulletFire(drone, false);
		}

		if(600 < fabsf(drone->Position().y))
		{
			drone->Alive(false);
		}
	}

	return S_OK;
}

void ScenePlay::BulletFire(GameObject* obj, bool isPlayer)
{
	if(isPlayer)
	{
		BulletSoundPlay();
		for(auto& bullet : m_vecBulletPlayer)
		{
			if(!bullet || bullet->alive)
				continue;

			T_KINETICS kt{};

			int indexModel = 0;
			bullet->m_model = EMODEL_BULLET[indexModel];
			{
				auto modelName = bullet->m_model;
				auto& tex = m_srvTex[modelName];
				XMFLOAT2 box{(float)tex.size.x, (float)tex.size.y};
				kt.box = box;
			}
			kt.dif = XMVECTORF32{{{1.0F, 1.0F, 1.0F, 1.0F}}};
			kt.alive = true;
			kt.pos = obj->Position();
			kt.vlc.y = m_speedBullet;

			bullet->Init((int)PLAY_STATE::LOW, kt, true);
			break;
		}
	}
	else
	{
		for(auto& bullet : m_vecBulletEnemy)
		{
			if(!bullet || bullet->alive)
				continue;

			T_KINETICS kt{};

			int indexModel = 2;
			bullet->m_model = EMODEL_BULLET[indexModel];
			{
				auto modelName = bullet->m_model;
				auto& tex = m_srvTex[modelName];
				XMFLOAT2 box{(float)tex.size.x, (float)tex.size.y};
				kt.box = box;
			}
			kt.dif = XMVECTORF32{{{1.0F, 1.0F, 1.0F, 1.0F}}};
			kt.alive = true;
			kt.pos = obj->Position();

			float speed = m_speedBullet;
			auto vlc_x = m_mainPlayer->Position().x - obj->Position().x;
			auto vlc_y = m_mainPlayer->Position().y - obj->Position().y;
			float len = kt.vlc.x = sqrtf(vlc_x * vlc_x + vlc_y * vlc_y);

			// 너무 가까와서 충돌로 플레이어 죽음
			if(0.0001F>len)
			{
				bullet->m_model = "";
				continue;
			}

			kt.vlc.x = vlc_x * speed/len;
			kt.vlc.y = vlc_y * speed/len;

			bullet->Init((int)PLAY_STATE::LOW, kt, false);
			break;
		}
	}
}

void ScenePlay::BulletUpdate(const std::any& t)
{
	GameTimer gt = std::any_cast<GameTimer>(t);

	auto pGameInfo = GameInfo::instance();

	auto funcPlayerBullet = [&](GameBullet* bt)
	{
		bool isCollision = false;
		for(auto& drone : m_vecDrone)
		{
			if(!drone->Alive())
				continue;
			auto droneKt = drone->Kinetics();
			isCollision = IsCollision(droneKt->pos.x, droneKt->pos.y
									, droneKt->box.x, droneKt->box.y, droneKt->scale
									, bt->pos.x, bt->pos.y
									, bt->box.x, bt->box.y, bt->scale);
			if(isCollision)
			{
				BoomSoundPlay();
				drone->Alive(false);
				break;
			}
		}
		auto ret = isCollision && pGameInfo->m_enablePlay;
		if(ret)
		{
			pGameInfo->IncreaseScore(250);
		}
		return ret;
	};

	auto d3d = IG2GraphicsD3D::instance();
	::SIZE screenSize = *any_cast<::SIZE*>(IG2GraphicsD3D::instance()->getAttrib(ATT_SCREEN_SIZE));

	for(auto& bullet : m_vecBulletPlayer)
	{
		if(!bullet || !bullet->alive)
			continue;

		bullet->Update(gt, funcPlayerBullet);
		if(screenSize.cy/2.2F < fabsf(bullet->pos.y) || screenSize.cx/2 < fabsf(bullet->pos.x))
		{
			bullet->alive = false;
		}
	}

	auto funcDroneBulletCollision = [&](GameBullet* bt)
	{
		auto playerKt = pGameInfo->MainPlayer()->Kinetics();
		bool isCollision = IsCollision(   playerKt->pos.x, playerKt->pos.y
										, playerKt->box.x, playerKt->box.y, playerKt->scale
										, bt->pos.x, bt->pos.y
										, bt->box.x, bt->box.y, bt->scale);
		if(isCollision)
		{
			//__debugbreak();
		}
		auto ret = isCollision && pGameInfo->m_enablePlay;
		return ret;
	};

	for(auto& bullet : m_vecBulletEnemy)
	{
		if(!bullet || !bullet->alive)
			continue;

		bullet->Update(gt, funcDroneBulletCollision);

		if(screenSize.cy/2.0F < fabsf(bullet->pos.y) || screenSize.cx/2 < fabsf(bullet->pos.x))
		{
			bullet->alive = false;
		}
	}
}

void ScenePlay::BulletSoundPlay()
{
	for(auto& sound : m_sndLaser)
	{
		if(!sound->IsPlaying())
		{
  			sound->Play(false);
			break;
		}
	}
}

void ScenePlay::BoomSoundPlay()
{
	for(auto& sound : m_sndBoom)
	{
		if(!sound->IsPlaying())
		{
			sound->Play(false);
			break;
		}
	}
}

