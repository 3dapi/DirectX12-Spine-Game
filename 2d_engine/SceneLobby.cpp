
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

	// title 텍스처 로드
	auto texManager = FactoryTexture::instance();
	{
		auto r = texManager->Load("ui/ui_select_char", "assets/ui/ui_select_char.png");
		r->name;
		m_uiTex.insert(std::make_pair(r->name, UI_TEXTURE{ r->r.Get(), r->size, {} }));
	}
	{
		auto r = texManager->Load("ui/ui_box", "assets/ui/ui_box.png");
		r->name;
		m_uiTex.insert(std::make_pair(r->name, UI_TEXTURE{ r->r.Get(), r->size, {} }));
	}
	{
		auto r = texManager->Load("ui/ui_game_start", "assets/ui/ui_game_start.png");
		r->name;
		m_uiTex.insert(std::make_pair(r->name, UI_TEXTURE{ r->r.Get(), r->size, {} }));
	}
	
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = (UINT)m_uiTex.size();
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_srvHeapUI));
	auto hCpu = m_srvHeapUI->GetCPUDescriptorHandleForHeapStart();
	auto hGpu = m_srvHeapUI->GetGPUDescriptorHandleForHeapStart();
	for(auto& itr : m_uiTex)
	{
		decltype(itr.second.res) res = itr.second.res;
		device->CreateShaderResourceView(res, nullptr, hCpu);
		itr.second.hCpu = hGpu;

		hCpu.ptr += descriptorSize;
		hGpu.ptr += descriptorSize;

	}

	


	printf("SceneLobby: Init\n");
	return S_OK;
}

int SceneLobby::Destroy()
{
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
	return S_OK;
}

int SceneLobby::Render()
{
	auto d3d        =  IG2GraphicsD3D::instance();
	auto device     = std::any_cast<ID3D12Device*             >(d3d->getDevice());
	auto cmdList    = std::any_cast<ID3D12GraphicsCommandList*>(d3d->getCommandList());
	UINT frameIndex = *std::any_cast<UINT*                    >(d3d->getAttrib(ATT_DEVICE_CURRENT_FRAME_INDEX));
	auto sprite     = std::any_cast<SpriteBatch*>(IG2AppFrame::instance()->getAttrib(EAPP_ATTRIB::EAPP_ATT_XTK_SPRITE));
	::SIZE  screenSize = *any_cast<::SIZE*>(IG2GraphicsD3D::instance()->getAttrib(ATT_SCREEN_SIZE));

	ID3D12DescriptorHeap* heaps[] = { m_srvHeapUI.Get() };
	cmdList->SetDescriptorHeaps(1, heaps);
	sprite->Begin(cmdList);
	{
		float alpha = (g_gameInfo->m_player->Model() == EMODEL_KNIGHT)? 1.0F : 0.3F;
		{
			auto& tex = m_uiTex["ui/ui_select_char"];
			sprite->Draw(tex.hCpu, tex.size, XMFLOAT2(screenSize.cx / 2.0F - tex.size.x / 2.0F, 20.0F), DirectX::XMVectorSet(1.0F, 0.9F, 0.0F, 0.8F));
		}

		{
			auto& tex = m_uiTex["ui/ui_box"];
			sprite->Draw(tex.hCpu, tex.size, XMFLOAT2(320, 150), DirectX::XMVectorSet(1.0F, 0.0F, 1.0F, alpha));
			sprite->Draw(tex.hCpu, tex.size, XMFLOAT2(710, 150), DirectX::XMVectorSet(0.0F, 0.0F, 1.0F, 0.2F));
		}

		if(1.0F == alpha)
		{
			auto& tex = m_uiTex["ui/ui_game_start"];
			sprite->Draw(tex.hCpu, tex.size, XMFLOAT2(screenSize.cx / 2.0F - tex.size.x / 2.0F, 500), DirectX::XMVectorSet(1.0F, 1.0F, 1.0F, 1.0F));
		}
		sprite->End();
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
	printf("SceneLobby: Notify: %s\n", name.c_str());

	if(name == "MouseUp")
	{
		auto mousePos = any_cast<const ::POINT&>(t);
		checkSelectChar(mousePos);
	}

	return S_OK;
}

void SceneLobby::checkSelectChar(const ::POINT& mousePos)
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
