
#include "Common/G2.FactoryCamera.h"
#include "Common/G2.FactoryTexture.h"
#include "Common/G2.FactoryShader.h"
#include "Common/G2.FactorySIgnature.h"
#include "Common/G2.FactoryPipelineState.h"
#include "Common/G2.Geometry.h"
#include "Common/G2.Util.h"
#include "Common/GameTimer.h"
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
		CheckChooseShip(mousePos);
	}

	return S_OK;
}

void SceneLobby::CheckChooseShip(const ::POINT& mousePos)
{
	auto pGameInfo = GameInfo::instance();

	// ship1 선택
	if(chckPointInRect (mousePos.x, mousePos.y, 60, 310, 250, 500))
	{
		GameInfo::instance()->MainPlayer()->Model(EMODEL_SHIP[0]);
		return;
	}

	// start game
	if(chckPointInRect (mousePos.x, mousePos.y, 100, 590, 500, 670))
	{
		IG2AppFrame::instance()->command(EAPP_CMD_CHANGE_SCENE, EAPP_SCENE::EAPP_SCENE_PLAY);
	}
	else
	{
		GameInfo::instance()->MainPlayer()->Model(string(""));
	}
}


static void clearMap(map<int, string>& mp)
{
	auto itr = mp.begin();
	for(; itr != mp.end(); )
	{
		if(itr->second == "hello world")
		{
			mp.erase(itr);
		}
		else
		{
			++itr;
		}
	}
}