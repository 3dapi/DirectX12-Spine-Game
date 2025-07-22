
#include <d3d12.h>
#include "Common/G2.Util.h"
#include "Common/GameTimer.h"
#include "CommonStates.h"
#include "SceneNone.h"

using namespace std;
using std::any_cast;
using namespace DirectX;
using namespace G2;


SceneNone::SceneNone()
{
	printf("SceneNone: Create\n");
}

SceneNone::~SceneNone()
{
	Destroy();
}

int SceneNone::Init(const std::any& initial_value)
{
	printf("SceneNone: Init\n");
	return S_OK;
}

int SceneNone::Destroy()
{
	printf("SceneNone: Destroy\n");
	return S_OK;
}

int SceneNone::Update(const std::any& t)
{
	GameTimer gt = std::any_cast<GameTimer>(t);
	auto deltaTime = gt.DeltaTime();
	printf("SceneNone: %f\n", deltaTime);
	return S_OK;
}

int SceneNone::Render()
{
	printf("SceneNone: Render\n");
	return S_OK;
}

int SceneNone::Notify(const std::string& name, const std::any& t)
{
	printf("ScenePplay: Notify\n");
	return S_OK;
}
