
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
}

SceneBegin::~SceneBegin()
{
	Destroy();
}

int SceneBegin::Init(const std::any& initial_value)
{
	return S_OK;
}

int SceneBegin::Destroy()
{
	return S_OK;
}

int SceneBegin::Update(const std::any& t)
{
	return S_OK;
}

int SceneBegin::Render()
{
	return S_OK;
}
