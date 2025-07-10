
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
#include "ScenePlay.h"
#include "DirectXHelpers.h"
#include "ResourceUploadBatch.h"
#include "GraphicsMemory.h"
#include "ScenePlay.h"
#include "SpineFactory.h"


using namespace std;
using std::any_cast;
using namespace DirectX;
using namespace spine;
using namespace G2;


ScenePlay::ScenePlay()
	: m_keyEvent(256, 0)
{
}

ScenePlay::~ScenePlay()
{
	Destroy();
}

int ScenePlay::Init(const std::any& initial_value)
{
	int hr = S_OK;
	hr = CreateMainPlayerModel();
	if (FAILED(hr))
		return hr;
	hr = CreateMobCharModel();
	if (FAILED(hr))
		return hr;

	return S_OK;
}

int ScenePlay::Destroy()
{
	printf("ScenePlay: Destroy\n");

	DeletePlayCharModel();
	DeleteMobCharModel();
	return S_OK;
}

int ScenePlay::Update(const std::any& t)
{
	GameTimer gt = std::any_cast<GameTimer>(t);
	auto dt = gt.DeltaTime();

	if (m_keyEvent[VK_LEFT] == EAPP_INPUT_PRESS)
		m_mainPlayer->MoveLeft(dt);

	if (m_keyEvent[VK_RIGHT] == EAPP_INPUT_PRESS)
		m_mainPlayer->MoveRight(dt);

	if (m_keyEvent[VK_UP] == EAPP_INPUT_PRESS)
		m_mainPlayer->MoveUp(dt);

	if (m_keyEvent[VK_DOWN] == EAPP_INPUT_PRESS)
		m_mainPlayer->MoveDown(dt);



	m_mainPlayer->Update(gt);
	

	return S_OK;
}

int ScenePlay::Render()
{
	m_mainPlayer->Render();
	return S_OK;
}

int ScenePlay::Notify(const std::string& name, const std::any& t)
{
	if(name == "KeyEvent")
	{
		auto keyState = any_cast<const uint8_t*>(t);
		std::copy(keyState, keyState + 256, m_keyEvent.begin());
	}
	else if (name == "MouseUp")
	{
		auto mousePos = any_cast<const ::POINT&>(t);
	}

	return S_OK;
}

int ScenePlay::CreateMainPlayerModel()
{
	m_mainPlayer = g_gameInfo->MainPlayer();
	if (!m_mainPlayer)
		return E_FAIL;

	//           model type  positgion scale  direction
	tuple<EAPP_MODEL, XMFLOAT2, float, float> charModel
	{
		EAPP_MODEL::EMODEL_KNIGHT	, { 0.0F, -300.0F}, 0.5F,  1.0F,
	};
	const auto& [model, pos, scale, direction] = charModel;
	
	SPINE_ATTRIB* att = FactorySpineObject::FindSpineAttribute(model);
	if (!att)
		return E_FAIL;
	auto spineChar = new(std::nothrow) SpineRender;
	if (!spineChar)
		return E_FAIL;

	if (FAILED(spineChar->Init(*att)))
		return E_FAIL;

	m_mainPlayer->Init(model, spineChar);

	return S_OK;
}

int ScenePlay::CreateMobCharModel()
{
	int hr = S_OK;
	return S_OK;
}

int ScenePlay::DeletePlayCharModel()
{
	int hr = S_OK;
	return S_OK;
}

int ScenePlay::DeleteMobCharModel()
{
	int hr = S_OK;
	return S_OK;
}

