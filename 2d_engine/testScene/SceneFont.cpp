#include <any>
#include <utility>
#include <d3d12.h>
#include "Common/G2.FactoryFontResource.h"
#include "Common/G2.FactorySIgnature.h"
#include "Common/G2.Util.h"
#include "AppCommon.h"
#include "AppCommonXTK.h"
#include "GameInfo.h"
#include "SceneFont.h"

using std::any_cast;
using namespace std;
using namespace DirectX;
using namespace G2;

SceneFont::SceneFont()
{
}

SceneFont::~SceneFont()
{
	Destroy();
}

int SceneFont::Destroy()
{
	m_srvHeapUI.Reset();

	return S_OK;
}

int SceneFont::Init(const std::any&)
{
	HRESULT hr = S_OK;
	auto d3d        = IG2GraphicsD3D::instance();
	auto device     = std::any_cast<ID3D12Device*             >(d3d->getDevice());
	auto cmdList    = std::any_cast<ID3D12GraphicsCommandList*>(d3d->getCommandList());
	auto cmdQue     = std::any_cast<ID3D12CommandQueue*       >(d3d->getCommandQueue());
	auto cmdAlloc   = std::any_cast<ID3D12CommandAllocator*   >(d3d->getCommandAllocator());

	std::string text = "Touch the Screen";
	auto [fontTex, srcSize, texSize] = FactoryFontResource::CreateStringTexture("고도 B", 72, text);

	UINT descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	vector<tuple<string, string>>  uiTextureList
	{
		{"ui/ui_touch_the_screen"	, "asset/ui/ui_touch_the_screen.png"	},
	};

	auto texManager = FactoryTexture::instance();
	for(const auto& [name, file]: uiTextureList)
	{
		auto r = texManager->Load(name, file);
		r->name;
		m_uiTex.insert(std::make_pair(r->name, UI_TEXTURE{ r->r, r->size, {} }));
	}

	m_uiTex.insert(std::make_pair("ui_temp: font message", UI_TEXTURE{ fontTex, srcSize, {}, texSize}));

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

	return S_OK;
}

int SceneFont::Update(const std::any& t)
{
	int hr = S_OK;
	GameTimer gt = std::any_cast<GameTimer>(t);

	auto& tex = m_uiTex["ui_temp: font message"];
	std::string text = "SceneFont::Update: " + std::to_string(gt.DeltaTime());
	auto [fontTex, srcSize, texSize] = FactoryFontResource::UpdateStringTexture(tex.res, "고도 B", 48, text);
	if(fontTex)
	{
		tex.size     = srcSize;
		tex.texSize  = texSize;
	}
	return S_OK;
}

int SceneFont::Render()
{
	auto d3d = IG2GraphicsD3D::instance();
	auto cmdList = std::any_cast<ID3D12GraphicsCommandList*>(d3d->getCommandList());
	auto sprite = std::any_cast<SpriteBatch*>(IG2AppFrame::instance()->getAttrib(EAPP_ATTRIB::EAPP_ATT_XTK_SPRITE));
	::SIZE screenSize = *any_cast<::SIZE*>(d3d->getAttrib(ATT_SCREEN_SIZE));

	ID3D12DescriptorHeap* heaps[] = { m_srvHeapUI.Get() };
	cmdList->SetDescriptorHeaps(1, heaps);
	sprite->Begin(cmdList);
	{
		{
			auto& tex = m_uiTex["ui/ui_touch_the_screen"];
			XMFLOAT2 position = { screenSize.cx / 2.0F - tex.size.x / 2.0F + 100.0F, 520.0F };
			XMFLOAT2 origin = { 0, 0 };
			XMFLOAT2 scale = { 0.6F, 0.6F };
			XMVECTOR color = XMVectorSet(1.f, 1.f, 1.f, 1.0F);
			sprite->Draw(tex.hGpu, tex.size, position, nullptr, color, 0.0F, origin, scale);
		}
		{
			auto& tex = m_uiTex["ui_temp: font message"];
			XMFLOAT2 position = {100.0F, 100.0F};
			XMFLOAT2 origin = {0, 0};
			XMFLOAT2 scale = {1.0F, 1.0f};
			XMVECTOR color = XMVectorSet(1.f, 0.f, 1.f, 1.0F);
			RECT rc{0,0, (LONG)tex.texSize.x, (LONG)tex.texSize.y};
			sprite->Draw(tex.hGpu, tex.size, position, &rc, color, 0.0F, origin, scale);
		}
	}
	sprite->End();

	return S_OK;
}

int SceneFont::Notify(const std::string& name, const std::any& t)
{
	return S_OK;
}
