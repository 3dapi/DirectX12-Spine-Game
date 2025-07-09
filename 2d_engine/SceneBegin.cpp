
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
#include "RenderSpine.h"
#include "GameInfo.h"

using namespace std;
using std::any_cast;
using namespace DirectX;
using namespace spine;
using namespace G2;


SceneBegin::SceneBegin()
{
	printf("SceneBegin: Create\n");
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

	vector<EAPP_MODEL>	charModel
	{
		EMODEL_RAPTOR	,
		EMODEL_GOBLIN	,
		EMODEL_KNIGHT	,
		EMODEL_BOY		,
		EMODEL_STMAN	,
		EMODEL_ALIEN	,
	};

	for(size_t i = 0; i < charModel.size(); ++i)
	{
		SPINE_ATTRIB* att = FindSpineAttribute(charModel[i]);
		if(!att)
			continue;
		auto spineChar = std::make_unique<RenderSpine>();
		if (!spineChar)
			continue;
		if(FAILED(spineChar->Init(*att)))
			continue;
		m_char.push_back(std::move(spineChar));
	}

	m_char[3]->Look(-1);
	m_char[4]->Look(-1);
	m_char[5]->Look(-1);

	m_pUi = new UiBegin;
	if (m_pUi)
		m_pUi->Init();

	printf("SceneBegin: Init\n");
	return S_OK;
}

int SceneBegin::Destroy()
{
	SAFE_DELETE(m_pUi);
	printf("SceneBegin: Destroy\n");
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
	//printf("SceneBegin: Render\n");
	return S_OK;
}

int SceneBegin::Notify(const std::string& name, const std::any& t)
{
	printf("SceneBegin: Notify: %s\n", name.c_str());

	if (name == "MouseUp")
	{
		auto mousePos = any_cast<const ::POINT&>(t);

		if (chckPointInRect(mousePos.x, mousePos.y, 400, 160, 900, 550))
		{
			IG2AppFrame::instance()->command(EAPP_CMD_CHANGE_SCENE, EAPP_SCENE_LOBBY);
		}
	}

	return S_OK;
}
