
#include <any>
#include <filesystem>
#include <tuple>
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
#include "SceneEnd.h"
#include "DirectXHelpers.h"
#include "ResourceUploadBatch.h"
#include "GraphicsMemory.h"
#include "SceneEnd.h"
#include "SpineFactory.h"

using namespace std;
using std::any_cast;
using namespace DirectX;
using namespace spine;
using namespace G2;


SceneEnd::SceneEnd()
{
	printf("SceneEnd: Create\n");
}

SceneEnd::~SceneEnd()
{
	Destroy();
}

int SceneEnd::Init(const std::any& initial_value)
{
	m_pUi = new UiEnd;
	if (m_pUi)
		m_pUi->Init();

	printf("SceneEnd: Init\n");
	return S_OK;
}

int SceneEnd::Destroy()
{
	SAFE_DELETE(m_pUi);
	printf("SceneEnd: Destroy\n");
	return S_OK;
}

int SceneEnd::Update(const std::any& t)
{
	GameTimer gt = std::any_cast<GameTimer>(t);
	auto deltaTime = gt.DeltaTime();
	if (m_pUi)
		m_pUi->Update(deltaTime);
	return S_OK;
}

int SceneEnd::Render()
{
	if (m_pUi)
	{
		m_pUi->Draw();
	}
	return S_OK;
}

int SceneEnd::Notify(const std::string& name, const std::any& t)
{
	printf("SceneBegin: Notify: %s\n", name.c_str());

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
