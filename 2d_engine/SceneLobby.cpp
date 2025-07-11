
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
#include "SpineFactory.h"
#include "GameInfo.h"
#include "UiLobby.h"

using namespace std;
using std::any_cast;
using namespace DirectX;
using namespace spine;
using namespace G2;


SceneLobby::SceneLobby()
{
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

	//           model type  positgion scale  direction
	vector<tuple<EAPP_MODEL, XMFLOAT2, float, float> >	charModel
	{
		{ EAPP_MODEL::EMODEL_KNIGHT	, {-480.0F, -160.0F}, 1.0F,  1.0F, },
		{ EAPP_MODEL::EMODEL_BOY	, { 120.0F, -160.0F}, 1.0F, -1.0F, },
	};

	for (size_t i = 0; i < charModel.size(); ++i)
	{
		const auto& [model, pos, scale, direction] = charModel[i];
		SPINE_ATTRIB* att = FactorySpineObject::FindSpineAttribute(model);
		if(!att)
			continue;
		auto spineChar = std::make_unique<SpineRender>();
		if (!spineChar)
			continue;
		if(FAILED(spineChar->Init(*att)))
			continue;

		spineChar->Position  (pos);
		spineChar->Scale     (scale);
		spineChar->Direction (direction);
		m_char[i] = std::move(spineChar);
	}
	m_char[1]->Direction(-1);

	SAFE_DELETE(m_pUi);
	m_pUi = new UiLobby;
	if (m_pUi)
		m_pUi->Init();

	return S_OK;
}

int SceneLobby::Destroy()
{
	SAFE_DELETE(m_pUi);
	return S_OK;
}

int SceneLobby::Update(const std::any& t)
{
	GameTimer gt = std::any_cast<GameTimer>(t);
	auto deltaTime = gt.DeltaTime();

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

	if (m_pUi)
	{
		m_pUi->DrawFront();
	}
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
	auto pGameInfo = GameInfo::instance();

	// character knight 선택
	if(chckPointInRect (mousePos.x, mousePos.y, 210, 170, 470, 430))
	{
		pGameInfo->MainPlayer()->ModelType(EAPP_MODEL::EMODEL_KNIGHT);
		return;
	}
	else
	{
		if (pGameInfo->MainPlayer()->ModelType() == EAPP_MODEL::EMODEL_KNIGHT)
		{
			if (chckPointInRect(mousePos.x, mousePos.y, 450, 500, 830, 560))
			{
				IG2AppFrame::instance()->command(EAPP_CMD_CHANGE_SCENE, EAPP_SCENE::EAPP_SCENE_PLAY);
				return;
			}
			else
			{
				pGameInfo->MainPlayer()->ModelType(EAPP_MODEL::EMODEL_NONE);
				return;
			}
		}
		else
		{
			return;
		}
	}
}
