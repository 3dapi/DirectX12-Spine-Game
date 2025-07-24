
#include "UiBackground.h"
#include "Common/G2.Util.h"
#include "Common/G2.FactoryTexture.h"

UiBackground::UiBackground()
{
}

UiBackground::~UiBackground()
{
	Destroy();
}

int UiBackground::Destroy()
{
	m_srvHeapUI.Reset();
	m_uiTex.clear();

	return S_OK;
}

int UiBackground::Init()
{
	m_scrollBegin = -960;
	m_scrollY	  = m_scrollBegin;
	m_scrollSpeed = 200.0F;

	auto d3d            = IG2GraphicsD3D::instance();
	auto device         = std::any_cast<ID3D12Device*>(d3d->getDevice());
	UINT descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	vector<tuple<string, string>>  uiTextureList
	{
		{EMODEL_BG[0], "asset/sprite/" + EMODEL_BG[0] + ".png"},
	};

	auto texManager = FactoryTexture::instance();
	for(const auto& [name, file]: uiTextureList)
	{
		auto r = texManager->Load(name, file);
		r->name;
		m_uiTex.insert(std::make_pair(r->name, UI_TEXTURE{r->r, r->size, {}}));
	}

	m_srvHeapUI = G2::CreateDescHeap((UINT)m_uiTex.size() + 1);
	auto hCpu = m_srvHeapUI->GetCPUDescriptorHandleForHeapStart();
	auto hGpu = m_srvHeapUI->GetGPUDescriptorHandleForHeapStart();
	for(auto& itr : m_uiTex)
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

int UiBackground::Update(float dt)
{
	m_scrollY	+= dt * m_scrollSpeed;
	if(0< m_scrollY)
	{
		m_scrollY += m_scrollBegin;
	}
	return S_OK;
}

int UiBackground::Draw()
{
	auto d3d = IG2GraphicsD3D::instance();
	auto cmdList = std::any_cast<ID3D12GraphicsCommandList*>(d3d->getCommandList());
	auto sprite = std::any_cast<SpriteBatch*>(IG2AppFrame::instance()->getAttrib(EAPP_ATTRIB::EAPP_ATT_XTK_SPRITE));
	::SIZE screenSize = *any_cast<::SIZE*>(d3d->getAttrib(ATT_SCREEN_SIZE));

	ID3D12DescriptorHeap* heaps[] = {m_srvHeapUI.Get()};
	cmdList->SetDescriptorHeaps(1, heaps);
	sprite->Begin(cmdList);
	{
		{
			auto& tex = m_uiTex[EMODEL_BG[0]];
			XMFLOAT2 origin = {0, 0};
			XMFLOAT2 scale = {1.0F, 1.0F};
			XMFLOAT2 position = {screenSize.cx / 2.0F - scale.x * tex.size.x / 2.0F, m_scrollY};
			sprite->Draw(tex.hGpu, tex.size, position, nullptr, XMVECTORF32{{{1.F, 1.F, 1.F, 1.0F}}}, 0.0F, origin, scale);
		}
	}
	sprite->End();

	return S_OK;
}

int UiBackground::DrawFront()
{
	return S_OK;
}
