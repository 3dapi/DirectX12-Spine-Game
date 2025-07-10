#include <any>
#include <utility>
#include <d3d12.h>
#include "Common/G2.FactoryTexture.h"
#include "Common/G2.FactorySIgnature.h"
#include "Common/G2.Util.h"
#include "AppCommon.h"
#include "AppCommonXTK.h"
#include "GameInfo.h"
#include "UiPlay.h"

using std::any_cast;
using namespace std;
using namespace DirectX;
using namespace G2;

UiPlay::UiPlay()
{
}

UiPlay::~UiPlay()
{
	Destroy();
}

int UiPlay::Destroy()
{
	m_srvHeapUI.Reset();
	m_uiTex.clear();
	return S_OK;
}

int UiPlay::Init()
{
	auto d3d        = IG2GraphicsD3D::instance();
	auto device     = std::any_cast<ID3D12Device*             >(d3d->getDevice());
	auto cmdList    = std::any_cast<ID3D12GraphicsCommandList*>(d3d->getCommandList());
	UINT descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	auto texManager = FactoryTexture::instance();
	{
		auto r = texManager->Load("ui/ui_select_char", "asset/ui/ui_select_char.png");
		r->name;
		m_uiTex.insert(std::make_pair(r->name, UI_TEXTURE{ r->r.Get(), r->size, {} }));
	}
	{
		auto r = texManager->Load("ui/ui_box", "asset/ui/ui_box.png");
		r->name;
		m_uiTex.insert(std::make_pair(r->name, UI_TEXTURE{ r->r.Get(), r->size, {} }));
	}
	{
		auto r = texManager->Load("ui/ui_game_start", "asset/ui/ui_game_start.png");
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

	return S_OK;
}

int UiPlay::Update(float)
{
	return S_OK;
}

int UiPlay::Draw()
{
	auto d3d          =  IG2GraphicsD3D::instance();
	auto cmdList      = std::any_cast<ID3D12GraphicsCommandList*>(d3d->getCommandList());
	auto sprite       = std::any_cast<SpriteBatch*              >(IG2AppFrame::instance()->getAttrib(EAPP_ATTRIB::EAPP_ATT_XTK_SPRITE));
	::SIZE screenSize = *any_cast<::SIZE*                       >(d3d->getAttrib(ATT_SCREEN_SIZE));

	ID3D12DescriptorHeap* heaps[] = { m_srvHeapUI.Get() };
	cmdList->SetDescriptorHeaps(1, heaps);
	sprite->Begin(cmdList);
	{
		float alpha = (g_gameInfo->MainPlayer()->ModelType() == EMODEL_KNIGHT)? 1.0F : 0.3F;
		{
			auto& tex = m_uiTex["ui/ui_select_char"];
			sprite->Draw(tex.hCpu, tex.size, XMFLOAT2(screenSize.cx / 2.0F - tex.size.x / 2.0F, 20.0F), DirectX::XMVectorSet(1.0F, 0.9F, 0.0F, 0.8F));
		}
		{
			auto& tex = m_uiTex["ui/ui_box"];
			sprite->Draw(tex.hCpu, tex.size, XMFLOAT2(320, 150), DirectX::XMVectorSet(1.0F, 0.0F, 1.0F, alpha));
			sprite->Draw(tex.hCpu, tex.size, XMFLOAT2(710, 150), DirectX::XMVectorSet(0.0F, 0.0F, 1.0F, 0.2F));
		}

		if(g_gameInfo->MainPlayer()->ModelType() == EMODEL_KNIGHT)
		{
			auto& tex = m_uiTex["ui/ui_game_start"];
			sprite->Draw(tex.hCpu, tex.size, XMFLOAT2(screenSize.cx / 2.0F - tex.size.x / 2.0F, 500), DirectX::XMVectorSet(1.0F, 1.0F, 1.0F, 1.0F));
		}
		sprite->End();
	}

	return S_OK;
}
