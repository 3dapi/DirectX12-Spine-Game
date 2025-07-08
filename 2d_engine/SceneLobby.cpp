
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
#include "SceneLobby.h"
#include "DirectXHelpers.h"
#include "ResourceUploadBatch.h"
#include "GraphicsMemory.h"
#include "SceneLobby.h"
#include "RenderSpine.h"

using namespace std;
using std::any_cast;
using namespace DirectX;
using namespace spine;
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
	return S_OK;
}

int SceneLobby::Destroy()
{
	return S_OK;
}

int SceneLobby::Update(const std::any& t)
{
	return S_OK;
}

int SceneLobby::Render()
{
	return S_OK;
}
