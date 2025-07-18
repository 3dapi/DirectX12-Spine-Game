﻿
#include <any>
#include <filesystem>
#include <tuple>
#include <d3d12.h>
#include "Common/G2.FactoryCamera.h"
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
#include "SceneSpine.h"
#include "DirectXHelpers.h"
#include "ResourceUploadBatch.h"
#include "GraphicsMemory.h"
#include "SceneSpine.h"
#include "SpineRender.h"

using namespace std;
using std::any_cast;
using namespace DirectX;
using namespace G2;


SceneSpine::SceneSpine()
{
}

SceneSpine::~SceneSpine()
{
	Destroy();
}

int SceneSpine::Init(const std::any& initial_value)
{
	auto cameraSpine = FactoryCamera::instance()->FindRes(IG2Camera::SPINE_2D);
	if(cameraSpine)
	{
		cameraSpine->Position({0.0f, +200.0f, -700.0f});
		cameraSpine->LookAt  ({0.0f, +200.0f,    0.0f});
		cameraSpine->Update  ();
	}

	vector<string> detachSlotSpineBoy = {
		"spineboy-torso", "head", "eyes-open", "mouth-smile", "visor",
		"gun", "front-arm", "front-bracer", "front-hand",
		"back-arm", "back-hand", "back-bracer", "back-knee",
		"lower-leg", "front-thigh",
		"stirrup-front", "stirrup-back", "stirrup-strap",
		"raptor-saddle",
		"raptor-saddle-strap-front",
		"raptor-saddle-strap-back",
		//"raptor-jaw-inside",
		//"raptor-mouth-inside",
		//"raptow-jaw-tooth",
		"raptor-horn-back",
		//"raptor-tongue",
		//"raptor-hindleg-back",
		//"raptor-back-arm",
		"back-thigh",
		//"raptor-body",
		//"raptor-jaw",
		//"raptor-front-arm",
		//"raptor-front-leg",
		"neck",
		"raptor-horn",

	};
	vector<string> detachSlotHero = {
		"weapon-morningstar-path",
		"chain-ball", "chain-round", "chain-round2", "chain-round3",
		"chain-flat", "chain-flat2", "chain-flat3", "chain-flat4", "handle"
	};

	vector< SPINE_ATTRIB> spine_rsc =
	{
		{"raptor"       , "raptor.atlas"           , "raptor-pro.json"       , randomRange(0.0F, 1.0F), 1.0F, 1.0F, 0.45F, {-800.0F, 0.0F}, "walk", "default", detachSlotSpineBoy },
		{"goblins"      , "goblins-pma.atlas"      , "goblins-pro.json"      , randomRange(0.0F, 1.0F), 1.0F, 1.0F, 1.25F, {-350.0F, 0.0F}, "walk", "goblin" , {} },
		{"hero"         , "hero-pro.atlas"         , "hero-pro.json"         , randomRange(0.0F, 1.0F), 1.0F, 1.0F, 1.00F, {-100.0F, 0.0F}, "walk", "weapon/sword", detachSlotHero },
		{"spineboy"     , "spineboy-pma.atlas"     , "spineboy-pro.json"     , randomRange(0.0F, 1.0F), 1.0F, 1.0F, 0.49F, { 250.0F, 0.0F}, "walk", "default", {} },
		{"stretchyman"  , "stretchyman-pma.atlas"  , "stretchyman-pro.json"  , randomRange(0.0F, 1.0F), 6.0F, 1.0F, 0.90F, { 500.0F, 0.0F}, "walk", "default", {} },
		{"alien"        , "alien.atlas"            , "alien-pro.json"        , randomRange(0.0F, 1.0F), 1.0F, 1.0F, 0.60F, { 750.0F, 0.0F}, "run" , "default", {} },
	};

	for(size_t i=0; i< spine_rsc.size(); ++i)
	{
		auto scene = std::make_unique<SpineRender>();
		if(scene)
		{
			const auto& initArgs = spine_rsc[i];
			if(SUCCEEDED(scene->Init(initArgs)))
			{
				m_objSpine.push_back(std::move(scene));
			}
		}
	}

	return S_OK;
}

int SceneSpine::Destroy()
{
	return S_OK;
}

int SceneSpine::Update(const std::any& t)
{
	for (size_t i = 0; i < m_objSpine.size(); ++i)
	{
		if(m_objSpine[i])
			m_objSpine[i]->Update(t);
	}
	return S_OK;
}

int SceneSpine::Render()
{
	for (size_t i = 0; i < m_objSpine.size(); ++i)
	{
		if (m_objSpine[i])
			m_objSpine[i]->Render();
	}
	return S_OK;
}

int SceneSpine::Notify(const std::string& name, const std::any& t)
{
	//if (name == "MouseUp")
	//{
	//	auto mousePos = any_cast<const ::POINT&>(t);

	//}

	return S_OK;
}
