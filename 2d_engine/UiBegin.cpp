#include <any>
#include <utility>
#include <d3d12.h>
#include "Common/G2.FactoryTexture.h"
#include "Common/G2.FactorySIgnature.h"
#include "Common/G2.Util.h"
#include "AppCommon.h"
#include "AppCommonXTK.h"
#include "GameInfo.h"
#include "UiBegin.h"

using std::any_cast;
using namespace std;
using namespace DirectX;
using namespace G2;

UiBegin::UiBegin()
{
}

UiBegin::~UiBegin()
{
	Destroy();
}

int UiBegin::Destroy()
{
	m_srvHeapUI.Reset();
	m_uiTex.clear();
	return S_OK;
}

int UiBegin::Init()
{
	auto d3d        = IG2GraphicsD3D::instance();
	auto device     = std::any_cast<ID3D12Device*             >(d3d->getDevice());
	auto cmdList    = std::any_cast<ID3D12GraphicsCommandList*>(d3d->getCommandList());
	UINT descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	auto texManager = FactoryTexture::instance();
	{
		auto r = texManager->Load("ui/ui_big_title", "assets/ui/ui_big_title.png");
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

	return S_OK;
}

int UiBegin::Update(float)
{
	return S_OK;
}

int UiBegin::Draw()
{
	auto d3d          =  IG2GraphicsD3D::instance();
	auto cmdList      = std::any_cast<ID3D12GraphicsCommandList*>(d3d->getCommandList());
	auto sprite       = std::any_cast<SpriteBatch*              >(IG2AppFrame::instance()->getAttrib(EAPP_ATTRIB::EAPP_ATT_XTK_SPRITE));
	::SIZE screenSize = *any_cast<::SIZE*                       >(d3d->getAttrib(ATT_SCREEN_SIZE));

	ID3D12DescriptorHeap* heaps[] = { m_srvHeapUI.Get() };
	cmdList->SetDescriptorHeaps(1, heaps);
	sprite->Begin(cmdList);
	{
		float alpha = (g_gameInfo->m_player->Model() == EMODEL_KNIGHT)? 1.0F : 0.3F;
		{
			auto& tex = m_uiTex["ui/ui_big_title"];

			XMFLOAT2 position = { screenSize.cx / 2.0F - tex.size.x / 2.0F + 100.0F, 20 };
			XMFLOAT2 origin   = { 0, 0 };
			XMFLOAT2 scale    = { 0.6F, 0.6F };  // 2배 확대

			sprite->Draw(tex.hCpu, tex.size, position, nullptr, XMVECTORF32{ { { 1.f, 1.f, 0.f, 0.7f } } } , 0.0f, origin, scale );
		}
		sprite->End();
	}

	return S_OK;
}
