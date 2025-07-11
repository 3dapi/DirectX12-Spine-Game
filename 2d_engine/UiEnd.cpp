#include <any>
#include <utility>
#include <d3d12.h>
#include "Common/G2.FactoryTexture.h"
#include "Common/G2.FactorySIgnature.h"
#include "Common/G2.Util.h"
#include "AppCommon.h"
#include "AppCommonXTK.h"
#include "GameInfo.h"
#include "UiEnd.h"

using std::any_cast;
using namespace std;
using namespace DirectX;
using namespace G2;

UiEnd::UiEnd()
{
}

UiEnd::~UiEnd()
{
	Destroy();
}

int UiEnd::Destroy()
{
	m_srvHeapUI.Reset();

	return S_OK;
}

int UiEnd::Init()
{
	auto d3d        = IG2GraphicsD3D::instance();
	auto device     = std::any_cast<ID3D12Device*       >(d3d->getDevice());
	auto cmdQue     = std::any_cast<ID3D12CommandQueue* >(d3d->getCommandQueue());
	auto cmdList    = std::any_cast<ID3D12GraphicsCommandList*>(d3d->getCommandList());
	UINT descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	m_srvHeapUI = G2::CreateDescHeap((UINT)m_uiTex.size() + 1);
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

	return S_OK;
}

int UiEnd::Update(float)
{
	return S_OK;
}

int UiEnd::Draw()
{
	auto d3d          =  IG2GraphicsD3D::instance();
	auto cmdList      = std::any_cast<ID3D12GraphicsCommandList*>(d3d->getCommandList());
	auto sprite       = std::any_cast<SpriteBatch*              >(IG2AppFrame::instance()->getAttrib(EAPP_ATTRIB::EAPP_ATT_XTK_SPRITE));
	::SIZE screenSize = *any_cast<::SIZE*                       >(d3d->getAttrib(ATT_SCREEN_SIZE));

	ID3D12DescriptorHeap* heaps[] = { m_srvHeapUI.Get() };
	cmdList->SetDescriptorHeaps(1, heaps);
	sprite->Begin(cmdList);
	{
		wstring wstr = L"SCORE: " + std::to_wstring(g_gameInfo->m_gameScore);
		m_font->DrawString(sprite, wstr.c_str(), XMFLOAT2(400, 200), Colors::Yellow, 0, XMFLOAT2(0, 0), 1.5F);
	}
	sprite->End();

	return S_OK;
}
