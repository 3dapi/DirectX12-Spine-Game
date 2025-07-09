
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
#include "RenderSpine.h"

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
	printf("SceneEnd: Init\n");
	return S_OK;
}

int SceneEnd::Destroy()
{
	printf("SceneEnd: Destroy\n");
	return S_OK;
}

int SceneEnd::Update(const std::any& t)
{
	GameTimer gt = std::any_cast<GameTimer>(t);
	auto deltaTime = gt.DeltaTime();
	printf("SceneEnd: %f\n", deltaTime);
	return S_OK;
}

int SceneEnd::Render()
{
	printf("SceneEnd: Render\n");
	return S_OK;
}

int SceneEnd::Notify(const std::string& name, const std::any& t)
{
	printf("SceneEnd: Notify\n");
	return S_OK;
}
