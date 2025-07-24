
#include "Common/G2.FactoryCamera.h"
#include "Common/G2.FactoryTexture.h"
#include "Common/G2.FactoryShader.h"
#include "Common/G2.FactorySIgnature.h"
#include "Common/G2.FactoryPipelineState.h"
#include "Common/G2.Geometry.h"
#include "Common/G2.Util.h"
#include "Common/GameTimer.h"
#include "CommonStates.h"

#include "DirectXHelpers.h"
#include "AppCommonXTK.h"

#include "GameInfo.h"
#include "UiBegin.h"
#include "SceneBegin.h"

using namespace std;
using std::any_cast;
using namespace DirectX;
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

	// spine camera
	auto cameraSpine = FactoryCamera::instance()->FindRes(IG2Camera::SPINE_2D);
	if(cameraSpine)
	{
		cameraSpine->Position({0.0f, +300.0f,-700.0f});
		cameraSpine->LookAt  ({0.0f, +300.0f,   0.0f});
		cameraSpine->Update  ();
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
	if (name == "MouseUp")
	{
		auto mousePos = any_cast<const ::POINT&>(t);
		if (chckPointInRect(mousePos.x, mousePos.y, 120, 440, 480, 650))
		{
			IG2AppFrame::instance()->command(EAPP_CMD_CHANGE_SCENE, EAPP_SCENE::EAPP_SCENE_LOBBY);
		}
	}

	return S_OK;
}
