
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
{
	printf("ScenePlay: Create\n");
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
	auto deltaTime = gt.DeltaTime();
	

	// 죽음이 끝난 몹을 재생

	return S_OK;
}

int ScenePlay::Render()
{
	printf("ScenePlay: Render\n");
	return S_OK;
}

int ScenePlay::Notify(const std::string& name, const std::any& t)
{
	printf("ScenePplay: Notify\n");
	return S_OK;
}

int ScenePlay::CreateMainPlayerModel()
{
	auto mainPlayer = g_gameInfo->MainPlayer();
	if (!mainPlayer)
		return E_FAIL;
	m_mainPlayer = mainPlayer;

	//SpineFactgory
	//m_mainPlayer->Init();
	// new char model
	//PG2OBJECT newSpine = 
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

