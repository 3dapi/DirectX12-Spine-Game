
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
#include "SceneBegin.h"
#include "DirectXHelpers.h"
#include "ResourceUploadBatch.h"
#include "GraphicsMemory.h"
#include "SceneBegin.h"
#include "RenderSpine.h"

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
	printf("SceneBegin: Init\n");
	return S_OK;
}

int SceneBegin::Destroy()
{
	printf("SceneBegin: Destroy\n");
	return S_OK;
}

int SceneBegin::Update(const std::any& t)
{
	GameTimer gt = std::any_cast<GameTimer>(t);
	auto deltaTime = gt.DeltaTime();
	printf("SceneBegin: %f\n", deltaTime);
	return S_OK;
}

int SceneBegin::Render()
{
	printf("SceneBegin: Render\n");
	return S_OK;
}

int SceneBegin::Notify(const std::string& name, const std::any& t)
{
	printf("SceneBegin: Notify\n");
	return S_OK;
}
