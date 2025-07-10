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
	return S_OK;
}

int UiPlay::Init()
{
	auto d3d        = IG2GraphicsD3D::instance();
	auto device     = std::any_cast<ID3D12Device*             >(d3d->getDevice());
	auto cmdList    = std::any_cast<ID3D12GraphicsCommandList*>(d3d->getCommandList());
	auto cmdQue     = std::any_cast<ID3D12CommandQueue*       >(d3d->getCommandQueue());
	UINT descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	auto texManager = FactoryTexture::instance();
	{
		auto r = texManager->Load("ui/ui_gameover", "asset/ui/ui_gameover.png");
		r->name;
		m_uiTex.insert(std::make_pair(r->name, UI_TEXTURE{ r->r.Get(), r->size, {} }));
	}

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = (UINT)m_uiTex.size() + 1;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_srvHeapUI));

	auto hCpu = m_srvHeapUI->GetCPUDescriptorHandleForHeapStart();
	auto hGpu = m_srvHeapUI->GetGPUDescriptorHandleForHeapStart();
	ResourceUploadBatch resourceUpload(device);
	{
		resourceUpload.Begin();
		{
			m_font = std::make_unique<SpriteFont>(device, resourceUpload, L"asset/font/SegoeUI_18.spritefont", hCpu, hGpu);
		}
		resourceUpload.End(cmdQue).wait();
	}

	for (auto& itr : m_uiTex)
	{
		hCpu.ptr += descriptorSize;
		hGpu.ptr += descriptorSize;

		decltype(itr.second.res) res = itr.second.res;
		device->CreateShaderResourceView(res, nullptr, hCpu);
		itr.second.hCpu = hGpu;
	}

	return S_OK;
}

int UiPlay::Update(float)
{
	return S_OK;
}

int UiPlay::Draw()
{
	auto d3d     = IG2GraphicsD3D::instance();
	auto cmdList = std::any_cast<ID3D12GraphicsCommandList*>(d3d->getCommandList());
	auto sprite  = std::any_cast<SpriteBatch*>(IG2AppFrame::instance()->getAttrib(EAPP_ATTRIB::EAPP_ATT_XTK_SPRITE));
	::SIZE screenSize = *any_cast<::SIZE*>(d3d->getAttrib(ATT_SCREEN_SIZE));

	ID3D12DescriptorHeap* heaps[] = { m_srvHeapUI.Get() };
	cmdList->SetDescriptorHeaps(1, heaps);
	sprite->Begin(cmdList);
	{
		{
			wstring wstr = L"SCORE: " + std::to_wstring(g_gameInfo->m_gameScore);
			m_font->DrawString(sprite, wstr.c_str(), XMFLOAT2(10, 10), Colors::Yellow, 0, XMFLOAT2(0, 0), 1.5F);
		}
		{
			auto hp = (int)g_gameInfo->MainPlayer()->HP();
			wstring wstr = L"HP: " + std::to_wstring(hp);
			m_font->DrawString(sprite, wstr.c_str(), XMFLOAT2(10, 60), Colors::Red, 0, XMFLOAT2(0, 0), 1.5F);
		}


		auto hp = g_gameInfo->MainPlayer()->HP();
		if(0>= hp)
		{
			auto& tex = m_uiTex["ui/ui_gameover"];
			XMFLOAT2 position = { screenSize.cx / 2.0F - tex.size.x / 2.0F, 120.0F };
			XMFLOAT2 origin = { 0, 0 };
			XMFLOAT2 scale = { 1.0F, 1.0F };
			sprite->Draw(tex.hCpu, tex.size, position, nullptr, XMVECTORF32{ { { 1.f, 0.f, 1.f, 1.0f } } }, 0.0f, origin, scale);
		}
	}
	sprite->End();

	return S_OK;
}
