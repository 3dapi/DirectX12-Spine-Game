
#include <any>
#include <filesystem>
#include <tuple>
#include <utility>
#include <d3d12.h>
#include "Common/G2.FactoryCamera.h"
#include "Common/G2.FactoryTexture.h"
#include "Common/G2.FactoryShader.h"
#include "Common/G2.FactorySIgnature.h"
#include "Common/G2.FactoryPipelineState.h"
#include "Common/G2.Geometry.h"
#include "Common/G2.Util.h"
#include "Common/GameTimer.h"
#include <pix.h>
#include "CommonStates.h"
#include "SceneLobby.h"
#include "DirectXHelpers.h"
#include "ResourceUploadBatch.h"
#include "GraphicsMemory.h"
#include "SceneLobby.h"
#include "GameInfo.h"
#include "UiLobby.h"

using namespace std;
using std::any_cast;
using namespace DirectX;
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

	// spine camera
	auto cameraSpine = FactoryCamera::instance()->FindRes(IG2Camera::SPINE_2D);
	if(cameraSpine)
	{
		cameraSpine->Position({0.0f, +160.0f,-700.0f});
		cameraSpine->LookAt  ({0.0f, +160.0f,   0.0f});
		cameraSpine->Update  ();
	}

	//           model type  position scale  direction
	vector<tuple<EAPP_MODEL, XMFLOAT2, float, float> >	charModel
	{
		{ EAPP_MODEL::EMODEL_KNIGHT	, {-480.0F, 0.0F}, 1.0F,  1.0F, },
		{ EAPP_MODEL::EMODEL_BOY	, { 120.0F, 0.0F}, 1.0F, -1.0F, },
	};


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
		return;
	}
	else
	{
	}
}
