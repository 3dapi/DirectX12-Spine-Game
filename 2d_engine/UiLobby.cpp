#include <any>
#include <utility>
#include <d3d12.h>
#include "Common/G2.FactoryTexture.h"
#include "Common/G2.FactorySIgnature.h"
#include "Common/G2.Util.h"
#include "AppCommon.h"
#include "AppCommonXTK.h"
#include "GameInfo.h"
#include "UiLobby.h"

using std::any_cast;
using namespace std;
using namespace DirectX;
using namespace G2;

UiLobby::UiLobby()
{
}

UiLobby::~UiLobby()
{
	Destroy();
}

int UiLobby::Destroy()
{
	m_srvHeapUI.Reset();
	m_uiTex.clear();
	m_font.reset();

	return S_OK;
}

int UiLobby::Init()
{
	auto d3d        = IG2GraphicsD3D::instance();
	auto device     = std::any_cast<ID3D12Device*             >(d3d->getDevice());
	auto cmdList    = std::any_cast<ID3D12GraphicsCommandList*>(d3d->getCommandList());
	auto cmdQue     = std::any_cast<ID3D12CommandQueue*       >(d3d->getCommandQueue());
	UINT descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	vector<tuple<string, string>>  uiTextureList
	{
		{"ui/ui_select_char"	, "asset/ui/ui_select_char.png"	},
		{"ui/ui_box"			, "asset/ui/ui_box.png"			},
		{"ui/ui_doc_key"		, "asset/ui/ui_doc_key.png"		},
		{"ui/ui_game_start"		, "asset/ui/ui_game_start.png"	},
	};
	auto texManager = FactoryTexture::instance();
	for(const auto& [name, file]: uiTextureList)
	{
		auto r = texManager->Load(name, file);
		r->name;
		m_uiTex.insert(std::make_pair(r->name, UI_TEXTURE{ r->r, r->size, {} }));
	}

	m_srvHeapUI = G2::CreateDescHeap((UINT)m_uiTex.size() + 1);
	auto hCpu = m_srvHeapUI->GetCPUDescriptorHandleForHeapStart();
	auto hGpu = m_srvHeapUI->GetGPUDescriptorHandleForHeapStart();
	for (auto& itr : m_uiTex)
	{
		decltype(itr.second.res) res = itr.second.res;
		device->CreateShaderResourceView(res, nullptr, hCpu);
		itr.second.hGpu = hGpu;

		// 다음 리소스 대응.
		hCpu.ptr += descriptorSize;
		hGpu.ptr += descriptorSize;
	}
	ResourceUploadBatch resourceUpload(device);
	{
		resourceUpload.Begin();
		{
			m_font = std::make_unique<SpriteFont>(device, resourceUpload, L"asset/font/SegoeUI_18.spritefont", hCpu, hGpu);
		}
		resourceUpload.End(cmdQue).wait();
	}

	return S_OK;
}

int UiLobby::Update(float dt)
{
	m_blend += dt * m_blendDir;
	if (1.0F < m_blend)
	{
		m_blendDir = -1.0f;
		m_blend = 1.0f;
	}
	else if (0.0F > m_blend)
	{
		m_blendDir = +1.0f;
		m_blend = 0.0f;
	}
	return S_OK;
}

int UiLobby::Draw()
{
	auto d3d          =  IG2GraphicsD3D::instance();
	auto cmdList      = std::any_cast<ID3D12GraphicsCommandList*>(d3d->getCommandList());
	auto sprite       = std::any_cast<SpriteBatch*              >(IG2AppFrame::instance()->getAttrib(EAPP_ATTRIB::EAPP_ATT_XTK_SPRITE));
	::SIZE screenSize = *any_cast<::SIZE*                       >(d3d->getAttrib(ATT_SCREEN_SIZE));
	auto pGameInfo    = GameInfo::instance();

	ID3D12DescriptorHeap* heaps[] = { m_srvHeapUI.Get() };
	cmdList->SetDescriptorHeaps(1, heaps);

	sprite->Begin(cmdList);
	{
		float alpha = (pGameInfo->MainPlayer()->ModelType() == EAPP_MODEL::EMODEL_KNIGHT)? 1.0F * m_blend : 0.4F;
		{
			auto& tex = m_uiTex["ui/ui_select_char"];
			sprite->Draw(tex.hGpu, tex.size, XMFLOAT2(screenSize.cx / 2.0F - tex.size.x / 2.0F, 20.0F), DirectX::XMVectorSet(1.0F, 1.0F, 0.6F, 1.0F));
		}
		{
			auto& tex = m_uiTex["ui/ui_box"];
			sprite->Draw(tex.hGpu, tex.size, XMFLOAT2(190, 150), DirectX::XMVectorSet(1.0F, 0.0F, 1.0F, alpha));
			sprite->Draw(tex.hGpu, tex.size, XMFLOAT2(580, 150), DirectX::XMVectorSet(0.0F, 0.0F, 1.0F, 0.4F));
		}
		{
			auto& tex = m_uiTex["ui/ui_doc_key"];
			sprite->Draw(tex.hGpu, tex.size, XMFLOAT2(950, 300), DirectX::XMVectorSet(0.8F, 0.8F, 0.8F, 1.0F));
		}

		sprite->End();
	}

	return S_OK;
}

int UiLobby::DrawFront()
{
	auto d3d          =  IG2GraphicsD3D::instance();
	auto cmdList      = std::any_cast<ID3D12GraphicsCommandList*>(d3d->getCommandList());
	auto sprite       = std::any_cast<SpriteBatch*              >(IG2AppFrame::instance()->getAttrib(EAPP_ATTRIB::EAPP_ATT_XTK_SPRITE));
	::SIZE screenSize = *any_cast<::SIZE*                       >(d3d->getAttrib(ATT_SCREEN_SIZE));
	auto pGameInfo    = GameInfo::instance();

	ID3D12DescriptorHeap* heaps[] = { m_srvHeapUI.Get() };
	cmdList->SetDescriptorHeaps(1, heaps);
	sprite->Begin(cmdList);
	{
		if (pGameInfo->MainPlayer()->ModelType() == EAPP_MODEL::EMODEL_KNIGHT)
		{
			auto& tex = m_uiTex["ui/ui_game_start"];
			sprite->Draw(tex.hGpu, tex.size, XMFLOAT2(screenSize.cx / 2.0F - tex.size.x / 2.0F, 500), DirectX::XMVectorSet(1.0F, 1.0F, 1.0F, m_blend));
		}
	}
	sprite->End();

	return S_OK;
}

