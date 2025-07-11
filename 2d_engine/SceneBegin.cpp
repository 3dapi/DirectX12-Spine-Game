
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
#include "SceneBegin.h"
#include "DirectXHelpers.h"
#include "ResourceUploadBatch.h"
#include "GraphicsMemory.h"
#include "SceneBegin.h"
#include "SpineRender.h"
#include "GameInfo.h"

using namespace std;
using std::any_cast;
using namespace DirectX;
using namespace spine;
using namespace G2;


SceneBegin::SceneBegin()
{
}

SceneBegin::~SceneBegin()
{
	Destroy();
}

int SceneBegin::Init(const std::any& initial_value)
{
	auto d3d = IG2GraphicsD3D::instance();
	auto device = std::any_cast<ID3D12Device*>(d3d->getDevice());
	auto cmdList = std::any_cast<ID3D12GraphicsCommandList*>(d3d->getCommandList());
	UINT descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//           model type  positgion scale  direction
	vector<tuple<EAPP_MODEL, XMFLOAT2, float, float> >	charModel
	{
		{ EAPP_MODEL::EMODEL_RAPTOR	, {-840.0F, -300.0F}, 1.0F,  1.0F, },
		{ EAPP_MODEL::EMODEL_GOBLIN	, {-400.0F, -300.0F}, 1.0F,  1.0F, },
		{ EAPP_MODEL::EMODEL_KNIGHT	, {-120.0F, -300.0F}, 0.7F,  1.0F, },
		{ EAPP_MODEL::EMODEL_BOY	, { 260.0F, -300.0F}, 0.7F, -1.0F, },
		{ EAPP_MODEL::EMODEL_STMAN	, { 550.0F, -300.0F}, 1.0F, -1.0F, },
		{ EAPP_MODEL::EMODEL_ALIEN	, { 800.0F, -300.0F}, 1.0F, -1.0F, },
	};	

	for(size_t i = 0; i < charModel.size(); ++i)
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
		m_char.push_back(std::move(spineChar));
	}

	SAFE_DELETE(m_pUi);
	m_pUi = new UiBegin;
	if (m_pUi)
		m_pUi->Init();

	return S_OK;
}

int SceneBegin::Destroy()
{
	SAFE_DELETE(m_pUi);
	return S_OK;
}

int SceneBegin::Update(const std::any& t)
{
	GameTimer gt = std::any_cast<GameTimer>(t);
	auto deltaTime = gt.DeltaTime();
	//printf("SceneBegin: %f\n", deltaTime);

	for (size_t i = 0; i < m_char.size(); ++i)
	{
		if (m_char[i])
			m_char[i]->Update(t);
	}

	if (m_pUi)
		m_pUi->Update(deltaTime);
	return S_OK;
}

int SceneBegin::Render()
{
	if (m_pUi)
	{
		m_pUi->Draw();
	}

	for (size_t i = 0; i < m_char.size(); ++i)
	{
		if (m_char[i])
			m_char[i]->Render();
	}

	if (m_pUi)
	{
		m_pUi->Draw();
	}

	if (m_pUi)
	{
		m_pUi->DrawFront();
	}
	return S_OK;
}

int SceneBegin::Notify(const std::string& name, const std::any& t)
{
	//printf("SceneBegin: Notify: %s\n", name.c_str());

	if (name == "MouseUp")
	{
		auto mousePos = any_cast<const ::POINT&>(t);

		if (chckPointInRect(mousePos.x, mousePos.y, 400, 160, 900, 550))
		{
			IG2AppFrame::instance()->command(EAPP_CMD_CHANGE_SCENE, EAPP_SCENE::EAPP_SCENE_LOBBY);
		}
	}

	return S_OK;
}
