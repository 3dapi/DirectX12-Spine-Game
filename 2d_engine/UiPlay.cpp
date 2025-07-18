﻿#include <any>
#include <utility>
#include <d3d12.h>
#include "Common/G2.FactoryFontResource.h"
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
	m_font.reset();

	return S_OK;
}

int UiPlay::Init()
{
	auto d3d        = IG2GraphicsD3D::instance();
	auto device     = std::any_cast<ID3D12Device*             >(d3d->getDevice());
	auto cmdList    = std::any_cast<ID3D12GraphicsCommandList*>(d3d->getCommandList());
	auto cmdQue     = std::any_cast<ID3D12CommandQueue*       >(d3d->getCommandQueue());
	UINT descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	vector<tuple<string, string>>  uiTextureList
	{
		{"ui/ui_play_background"	, "asset/ui/ui_play_background.png"	},
		{"ui/ui_rect"				, "asset/ui/ui_rect.png"	},
		{"ui/ui_gameover"			, "asset/ui/ui_gameover.png"},
		{"ui/ui_touch_the_screen"	, "asset/ui/ui_touch_the_screen.png"},
		{"ui/ui_touch_the_screen"	, "asset/ui/ui_touch_the_screen.png"},
		{"ui/ui_stage_cleared"		, "asset/ui/ui_stage_cleared.png"},
	};
	auto texManager = FactoryTexture::instance();
	for (const auto& [name, file] : uiTextureList)
	{
		auto r = texManager->Load(name, file);
		r->name;
		m_uiTex.insert(std::make_pair(r->name, UI_TEXTURE{ r->r, r->size, {} }));
	}

	{
		std::string text = "Score: ";
		auto [fontTex, sizeTex, sizeSrc] = StringTexture::CreateStringTexture("고도 B", 24, text);
		m_uiTex.insert(std::make_pair("ui_font score", UI_TEXTURE{fontTex, sizeTex, {}, sizeSrc}));
	}
	{
		std::string text = "HP: ";
		auto [fontTex, sizeTex, sizeSrc] = StringTexture::CreateStringTexture("고도 B", 24, text);
		m_uiTex.insert(std::make_pair("ui_font hp", UI_TEXTURE{fontTex, sizeTex, {}, sizeSrc}));	
	}
	{
		std::string text = "Stage: ";
		auto [fontTex, sizeTex, sizeSrc] = StringTexture::CreateStringTexture("고도 B", 48, text);
		m_uiTex.insert(std::make_pair("ui_font stage", UI_TEXTURE{fontTex, sizeTex, {}, sizeSrc}));
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

int UiPlay::Update(float dt)
{
	auto pGameInfo = GameInfo::instance();

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

	{
		auto& tex = m_uiTex["ui_font score"];
		std::string text = "Score: " + std::to_string(pGameInfo->m_gameScore);
		auto [fontTex, sizeTex, sizeSrc] = StringTexture::UpdateStringTexture(tex.res, "고도 B", 24, text);
		if(fontTex)
		{
			tex.size    = sizeTex;
			tex.sizeSrc = sizeSrc;
		}
	}
	{
		auto& tex = m_uiTex["ui_font hp"];
		std::string text = "HP: " + std::to_string((int)pGameInfo->MainPlayer()->HP());
		auto [fontTex, sizeTex, sizeSrc] = StringTexture::UpdateStringTexture(tex.res, "고도 B", 24, text);
		if(fontTex)
		{
			tex.size    = sizeTex;
			tex.sizeSrc = sizeSrc;
		}
	}
	{
		auto& tex = m_uiTex["ui_font stage"];
		std::string text = std::to_string(pGameInfo->CurrentStateIndex() + 1);
		auto [fontTex, sizeTex, sizeSrc] = StringTexture::UpdateStringTexture(tex.res, "고도 B", 48, text);
		if(fontTex)
		{
			tex.size    = sizeTex;
			tex.sizeSrc = sizeSrc;
		}
	}

	return S_OK;
}

int UiPlay::Draw()
{
	auto d3d     = IG2GraphicsD3D::instance();
	auto cmdList = std::any_cast<ID3D12GraphicsCommandList*>(d3d->getCommandList());
	auto sprite  = std::any_cast<SpriteBatch*>(IG2AppFrame::instance()->getAttrib(EAPP_ATTRIB::EAPP_ATT_XTK_SPRITE));
	::SIZE screenSize = *any_cast<::SIZE*>(d3d->getAttrib(ATT_SCREEN_SIZE));

	auto pGameInfo = GameInfo::instance();

	ID3D12DescriptorHeap* heaps[] = { m_srvHeapUI.Get() };
	cmdList->SetDescriptorHeaps(1, heaps);
	sprite->Begin(cmdList);
	{
		// background
		{
			auto& tex = m_uiTex["ui/ui_play_background"];
			XMFLOAT2 position = { 0.0F, 0.0F };
			sprite->Draw(tex.hGpu, tex.size, position);
		}
		// score
		{
			auto& texScore = m_uiTex["ui_font score"];
			XMFLOAT2 position = {10.0F, 10.0F};
			XMFLOAT2 origin = {0.0F, 0.0F};
			XMFLOAT2 scale = {1.0F, 1.0f};
			sprite->Draw(texScore.hGpu, texScore.size, position, nullptr, Colors::Yellow, 0.0F, origin, scale);
		}
		// HP
		{
			auto& texHp = m_uiTex["ui_font hp"];
			XMFLOAT2 position = {10.0F, 70.0F};
			XMFLOAT2 origin = {0.0F, 0.0F};
			XMFLOAT2 scale = {1.0F, 1.0f};
			sprite->Draw(texHp.hGpu, texHp.size, position, nullptr, Colors::Red, 0.0F, origin, scale);
		}
		{
			auto hp = (int)pGameInfo->MainPlayer()->HP();
			//wstring wstr = L"HP: " + std::to_wstring(hp);
			//m_font->DrawString(sprite, wstr.c_str(), XMFLOAT2(10, 60), Colors::Red, 0, XMFLOAT2(0, 0), 1.5F);

			auto& tex = m_uiTex["ui/ui_rect"];
			XMFLOAT2 position = { 145, 75.0F };
			XMFLOAT2 origin = { 0, 0 };
			XMFLOAT2 scale = { (hp +0.4F)*6.0F /100.0F, 0.25F};
			sprite->Draw(tex.hGpu, tex.size, position, nullptr, XMVECTORF32{ { { 1.F, 0.F, 0.F, 1.0F } } }, 0.0F, origin, scale);
		}
		if (!pGameInfo->m_enablePlay)
		{
			auto& tex = m_uiTex["ui/ui_gameover"];
			XMFLOAT2 position = { screenSize.cx / 2.0F - tex.size.x / 2.0F, 100.0F };
			XMFLOAT2 origin = { 0, 0 };
			XMFLOAT2 scale = { 1.0F, 1.0F };
			sprite->Draw(tex.hGpu, tex.size, position, nullptr, XMVECTORF32{ { { 1.F, 0.F, 1.F, 1.0F } } }, 0.0F, origin, scale);
		}
	}
	sprite->End();

	return S_OK;
}

int UiPlay::DrawFront()
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
		if (!pGameInfo->m_enablePlay)
		{
			auto& tex = m_uiTex["ui/ui_stage_cleared"];
			XMFLOAT2 position = { screenSize.cx / 2.0F - tex.size.x / 2.0F + 100.0F, 520.0F };
			XMFLOAT2 origin = { 0, 0 };
			XMFLOAT2 scale = { 0.6F, 0.6F };
			XMVECTOR color = XMVectorSet(1.f, 1.f, 1.f, m_blend);
			sprite->Draw(tex.hGpu, tex.size, position, nullptr, color, 0.0F, origin, scale);
		}
	}
	sprite->End();

	return S_OK;
}

int UiPlay::StageChangingDraw()
{
	auto d3d          =  IG2GraphicsD3D::instance();
	auto cmdList      = std::any_cast<ID3D12GraphicsCommandList*>(d3d->getCommandList());
	auto sprite       = std::any_cast<SpriteBatch*              >(IG2AppFrame::instance()->getAttrib(EAPP_ATTRIB::EAPP_ATT_XTK_SPRITE));
	::SIZE screenSize = *any_cast<::SIZE*                       >(d3d->getAttrib(ATT_SCREEN_SIZE));


	ID3D12DescriptorHeap* heaps[] = { m_srvHeapUI.Get() };
	cmdList->SetDescriptorHeaps(1, heaps);
	sprite->Begin(cmdList);
	{
		{
			auto& tex = m_uiTex["ui/ui_stage_cleared"];
			XMFLOAT2 position = { 0.0F, 0.0F};
			XMFLOAT2 origin = { 0, 0 };
			XMFLOAT2 scale = { 1.0F, 1.0F };
			XMVECTOR color = XMVectorSet(1.f, 1.f, 0.f, 0.8F);
			sprite->Draw(tex.hGpu, tex.size, position, nullptr, color);
		}
		{
			auto& tex = m_uiTex["ui_font stage"];
			XMFLOAT2 position = { screenSize.cx / 2.0F-50.0F, 140.0F };
			XMFLOAT2 origin = {0, 0};
			XMFLOAT2 scale = {2.0F, 2.0F};
			XMVECTOR color = XMVectorSet(1.f, 1.f, 0.f, 0.8F);
			sprite->Draw(tex.hGpu, tex.size, position, nullptr, color, 0.0F, origin, scale);
		}
		{
			auto& tex = m_uiTex["ui/ui_touch_the_screen"];
			XMFLOAT2 position = { screenSize.cx / 2.0F - tex.size.x / 2.0F + 100.0F, 520.0F };
			XMFLOAT2 origin = { 0, 0 };
			XMFLOAT2 scale = { 0.6F, 0.6F };
			XMVECTOR color = XMVectorSet(1.f, 1.f, 1.f, m_blend);
			sprite->Draw(tex.hGpu, tex.size, position, nullptr, color, 0.0F, origin, scale);
		}
	}
	sprite->End();
	return 0;
}