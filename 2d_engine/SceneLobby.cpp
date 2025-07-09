
#include <any>
#include <filesystem>
#include <tuple>
#include <utility>
#include <d3d12.h>
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
#include "SceneLobby.h"
#include "DirectXHelpers.h"
#include "ResourceUploadBatch.h"
#include "GraphicsMemory.h"
#include "SceneLobby.h"
#include "RenderSpine.h"
#include "GameInfo.h"

using namespace std;
using std::any_cast;
using namespace DirectX;
using namespace spine;
using namespace G2;


SceneLobby::SceneLobby()
{
	printf("SceneLobby: Create\n");
}

SceneLobby::~SceneLobby()
{
	Destroy();
}

int SceneLobby::Init(const std::any& initial_value)
{
	auto d3d        =  IG2GraphicsD3D::instance();
	auto device     = std::any_cast<ID3D12Device*             >(d3d->getDevice());
	auto cmdList    = std::any_cast<ID3D12GraphicsCommandList*>(d3d->getCommandList());
	UINT descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	vector<string> detachSlotHero = {
		"weapon-morningstar-path",
		"chain-ball", "chain-round", "chain-round2", "chain-round3",
		"chain-flat", "chain-flat2", "chain-flat3", "chain-flat4", "handle"
	};
	vector< SPINE_ATTRIB> spine_rsc =
	{
		{"hero"     , "hero-pro.atlas"      , "hero-pro.json"       , randomRange(0.0F, 1.0F), 1.0F, 1.0F, 1.00F, { -300.0F, -150.0F}, "idle", "weapon/sword", detachSlotHero },
		{"spineboy" , "spineboy-pma.atlas"  , "spineboy-pro.json"   , randomRange(0.0F, 1.0F), 1.0F, 1.0F, 0.49F, {  300.0F, -150.0F}, "idle", "default", {} },
	};

	for (size_t i=0; i<spine_rsc.size(); ++i)
	{
		m_char[i] = std::make_unique<RenderSpine>();
		if (m_char[i])
		{
			const auto& initArgs = spine_rsc[i];
			m_char[i]->Init(initArgs);
		}
	}

	m_char[1]->Look(-1);

	m_pUi = new UiLobby;
	if (m_pUi)
		m_pUi->Init();

	printf("SceneLobby: Init\n");
	return S_OK;
}

int SceneLobby::Destroy()
{
	SAFE_DELETE(m_pUi);
	printf("SceneLobby: Destroy\n");
	return S_OK;
}

int SceneLobby::Update(const std::any& t)
{
	GameTimer gt = std::any_cast<GameTimer>(t);
	auto deltaTime = gt.DeltaTime();
	//printf("SceneLobby: %f\n", deltaTime);

	for (size_t i = 0; i < m_char.size(); ++i)
	{
		if (m_char[i])
			m_char[i]->Update(t);
	}

	if (m_pUi)
		m_pUi->Update(deltaTime);
	return S_OK;
}

int SceneLobby::Render()
{
	if (m_pUi)
	{
		m_pUi->Draw();
	}

	for(size_t i=0; i< m_char.size(); ++i)
	{
		if (m_char[i])
			m_char[i]->Render();
	}
	//printf("SceneLobby: Render\n");
	return S_OK;
}

int SceneLobby::Notify(const std::string& name, const std::any& t)
{
	if(name == "MouseUp")
	{
		auto mousePos = any_cast<const ::POINT&>(t);
		CheckSelectCharacter(mousePos);
	}

	return S_OK;
}

void SceneLobby::CheckSelectCharacter(const ::POINT& mousePos)
{
	// character knight 선택
	if(chckPointInRect (mousePos.x, mousePos.y, 340, 170, 600, 430))
	{
		g_gameInfo->m_player->Model(EMODEL_KNIGHT);
		return;
	}
	else
	{
		if (g_gameInfo->m_player->Model() == EMODEL_KNIGHT)
		{
			if (chckPointInRect(mousePos.x, mousePos.y, 450, 500, 830, 560))
			{
				IG2AppFrame::instance()->command(EAPP_CMD_CHANGE_SCENE, EAPP_SCENE_PLAY);
				return;
			}
			else
			{
				g_gameInfo->m_player->Model(EMODEL_NONE);
				return;
			}
		}
		else
		{
			return;
		}
	}
}
