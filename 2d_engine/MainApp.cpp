﻿//--------------------------------------------------------------------------------------------------------------------------------------------------------------
// MainApp

#include <string>
#include <tuple>
#include <d3d12.h>
#include "MainApp.h"
#include "DDSTextureLoader.h"
#include "Common/D12DDSTextureLoader.h"
#include "Common/G2.FactoryTexture.h"
#include "Common/G2.FactoryShader.h"
#include "Common/G2.FactorySIgnature.h"
#include "Common/G2.FactoryPipelineState.h"
#include "Common/G2.Geometry.h"
#include "Common/G2.Util.h"
#include "SceneGameMesh.h"
#include "SceneXtkGame.h"
#include "SceneSpine.h"
#include "SceneSample2D.h"

using namespace std;

static MainApp* g_pMain{};
G2::IG2AppFrame* G2::IG2AppFrame::instance()
{
	if (!g_pMain)
	{
		g_pMain = new MainApp;
	}
	return g_pMain;
}

MainApp::MainApp()
{
}

MainApp::~MainApp()
{
	destroy();
}

std::any MainApp::getAttrib(int nAttrib)
{
	switch((EAPP_ATTRIB)nAttrib)
	{
		case EAPP_ATTRIB::EAPP_ATT_WIN_HWND:					return mhMainWnd;
		case EAPP_ATTRIB::EAPP_ATT_WIN_HINST:					return mhAppInst;
		case EAPP_ATTRIB::EAPP_ATT_XTK_GRAPHICS_MEMORY:			return m_graphicsMemory.get();
		case EAPP_ATTRIB::EAPP_ATT_XTK_BATCH:					return m_batch.get();
	}
	return D3DWinApp::getAttrib(nAttrib);
}

int MainApp::setAttrib(int nAttrib,const std::any& v)
{
	 return D3DWinApp::setAttrib(nAttrib);
}

int MainApp::command(int nCmd,const std::any& v)
{
	return D3DWinApp::command(nCmd, v);
}

bool IsKeyDown(int vkeyCode)
{
	return (GetAsyncKeyState(vkeyCode) & 0x8000) != 0;
}


int MainApp::init(const std::any& initialValue /* = */)
{
	int hr = D3DWinApp::init(initialValue);
	if (FAILED(hr))
		return hr;

	auto d3d = IG2GraphicsD3D::instance();
	auto d3dDevice       = std::any_cast<ID3D12Device*              >(d3d->getDevice());
	
	d3d->command(CMD_COMMAND_BEGIN);

	// 1. load texutre
	auto tex_manager = FactoryTexture::instance();
	tex_manager->Load("grassTex", "assets/texture/grass.dds");
	tex_manager->Load("fenceTex", "assets/texture/WireFence.dds");

	// 3. Build Shaders And Input Layouts
	const D3D_SHADER_MACRO defines[] =
	{
		"FOG", "1",
		NULL, NULL
	};
	const D3D_SHADER_MACRO alphaTestDefines[] =
	{
		"FOG", "1",
		"ALPHA_TEST", "1",
		NULL, NULL
	};

	auto shader_manager = FactoryShader::instance();
	shader_manager->Load("standardVS"   , "Shaders/Default.hlsl"   , "vs_5_0", "VS"                  );
	shader_manager->Load("opaquePS"     , "Shaders/Default.hlsl"   , "ps_5_0", "PS", defines         );
	shader_manager->Load("alphaTestedPS", "Shaders/Default.hlsl"   , "ps_5_0", "PS", alphaTestDefines);

	auto pls_manager = FactoryPipelineState::instance();

	d3d->command(CMD_COMMAND_END);

	// create XTK Instance
	m_graphicsMemory = std::make_unique<GraphicsMemory>(d3dDevice);
	m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(d3dDevice);
	{
		auto scene = std::make_unique<SceneGameMesh>();
		if (scene)
		{
			//if (SUCCEEDED(scene->Init()))
			{
				//m_pSceneMesh = std::move(scene);
			}
		}
	}
	{
		auto scene = std::make_unique<SceneXtkGame>();
		if(scene)
		{
			//if(SUCCEEDED(scene->Init()))
			{
				//m_pSceneXKT = std::move(scene);
			}
		}
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
	};
	vector<string> detachSlotHero = {
		"weapon-morningstar-path",
		"chain-ball", "chain-round", "chain-round2", "chain-round3",
		"chain-flat", "chain-flat2", "chain-flat3", "chain-flat4", "handle"
	};

	vector< SPINE_ATTRIB> spine_rsc =
	{
		{"raptor"       , "raptor.atlas"           , "raptor-pro.json"       , randomRange(0.0F, 1.0F), 1.0F, 1.0F, 0.45F, {-700.0F, -300.0F}, "walk", "default", detachSlotSpineBoy },
		{"goblins"      , "goblins-pma.atlas"      , "goblins-pro.json"      , randomRange(0.0F, 1.0F), 1.0F, 1.0F, 1.25F, {-300.0F, -300.0F}, "walk", "goblin" , {} },
		{"hero"         , "hero-pro.atlas"         , "hero-pro.json"         , randomRange(0.0F, 1.0F), 1.0F, 1.0F, 1.00F, {   0.0F, -300.0F}, "walk", "weapon/sword", detachSlotHero },
		{"stretchyman"  , "stretchyman-pma.atlas"  , "stretchyman-pro.json"  , randomRange(0.0F, 1.0F), 1.0F, 1.0F, 0.90F, { 300.0F, -300.0F}, "idle", "default", {} },
		{"alien"        , "alien.atlas"            , "alien-pro.json"        , randomRange(0.0F, 1.0F), 1.0F, 1.0F, 0.60F, { 600.0F, -300.0F}, "run" , "default", {} },
	};

	for(const auto& rsc: spine_rsc)
	{
		auto scene = std::make_unique<SceneSpine>();
		if(scene)
		{
			auto initArgs = rsc;
			if(SUCCEEDED(scene->Init(initArgs)))
			{
				m_pSceneSpine.push_back(std::move(scene));
			}
		}
	}
	{
		auto scene=std::make_unique<SceneSample2D>();
		if(scene)
		{
			//if(SUCCEEDED(scene->Init()))
			//{
			//	m_pSceneSample = std::move(scene);
			//}
		}
	}

	return S_OK;
}

int MainApp::destroy()
{
	m_pSceneMesh	= {};
	m_pSceneXKT		= {};
	if(!m_pSceneSpine.empty())
		m_pSceneSpine.clear();

	FactoryPipelineState::instance()->UnLoadAll();
	FactorySignature::instance()->UnLoadAll();
	FactoryShader::instance()->UnLoadAll();
	FactoryTexture::instance()->UnLoadAll();
	return S_OK;
}

int MainApp::Resize(bool up)
{
	int hr = D3DWinApp::Resize(up);
	return S_OK;
}

int MainApp::Update(const std::any& t)
{
	// Cycle through the circular frame resource array.
	//UpdateUploadChain();

	int hr = S_OK;
	GameTimer gt = std::any_cast<GameTimer>(t);
	OnKeyboardInput(gt);
	//UpdateCamera(gt);
	//UpdateBox(gt);

	if(m_pSceneMesh)
		m_pSceneMesh->Update(t);
	if(m_pSceneXKT)
		m_pSceneXKT->Update(t);
	for (auto& spine : m_pSceneSpine)
	{
		spine->Update(t);
	}

	if(m_pSceneSample)
		m_pSceneSample->Update(t);

	return S_OK;
}

int MainApp::Render()
{
	int hr = S_OK;
	auto d3d = IG2GraphicsD3D::instance();
	auto d3dDevice       = std::any_cast<ID3D12Device*              >(d3d->getDevice());
	auto d3dCommandList  = std::any_cast<ID3D12GraphicsCommandList* >(d3d->getCommandList());
	auto d3dCommandAlloc = std::any_cast<ID3D12CommandAllocator*    >(d3d->getCommandAllocator());
	auto commandQue      = std::any_cast<ID3D12CommandQueue*        >(d3d->getCommandQueue());
	auto d3dViewport     = std::any_cast<D3D12_VIEWPORT*            >(d3d->getAttrib(ATT_DEVICE_VIEWPORT));
	auto d3dScissor      = std::any_cast<D3D12_RECT*                >(d3d->getAttrib(ATT_DEVICE_SCISSOR_RECT));
	auto d3dBackBuffer   = std::any_cast<ID3D12Resource*            >(d3d->getCurrentBackBuffer());
	auto d3dBackBufferV  = std::any_cast<CD3DX12_CPU_DESCRIPTOR_HANDLE>(d3d->getBackBufferView());
	auto d3dDepthV       = std::any_cast<D3D12_CPU_DESCRIPTOR_HANDLE>(d3d->getDepthStencilView());
	auto pls_manager     = FactoryPipelineState::instance();

	// 0. d3d 작업 완료 대기.
	hr = d3d->command(EG2GRAPHICS_D3D::CMD_FENCE_WAIT);

	// Reuse the memory associated with command recording.
	// We can only reset when the associated command lists have finished execution on the GPU.
	hr = d3dCommandAlloc->Reset();
	if (FAILED(hr))
		return hr;

	// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
	// Reusing the command list reuses memory.
	hr = d3dCommandList->Reset(d3dCommandAlloc, nullptr);
	if (FAILED(hr))
		return hr;

	d3dCommandList->RSSetViewports(1, d3dViewport);
	d3dCommandList->RSSetScissorRects(1, d3dScissor);

	// Indicate a state transition on the resource usage.
	d3dCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(d3dBackBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	// Clear the back buffer and depth buffer.
	XMFLOAT4 clearColor = { 0.0f, 0.4f, 0.6f, 1.0f };
	d3dCommandList->ClearRenderTargetView(d3dBackBufferV, (float*)&clearColor, 0, nullptr);
	d3dCommandList->ClearDepthStencilView(d3dDepthV, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	// Specify the buffers we are going to render to.
	d3dCommandList->OMSetRenderTargets(1, &d3dBackBufferV, true, &d3dDepthV);


	if(m_pSceneMesh)
		m_pSceneMesh->Render();
	if(m_pSceneXKT)
		m_pSceneXKT->Render();
	for (auto& spine : m_pSceneSpine)
	{
		spine->Render();
	}
	if(m_pSceneSample)
		m_pSceneSample->Render();


	// Indicate a state transition on the resource usage.
	d3dCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(d3dBackBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	// Done recording commands.
	hr = d3dCommandList->Close();
	if (FAILED(hr))
		return hr;

	// Add the command list to the queue for execution.
	ID3D12CommandList* cmdsLists[] = { d3dCommandList };
	commandQue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// 1. 화면 표시.
	hr = d3d->command(EG2GRAPHICS_D3D::CMD_PRESENT);
	// 2. GPU가 해당 작업을 완료할 때까지 대기.
	hr = d3d->command(EG2GRAPHICS_D3D::CMD_WAIT_GPU);

	return S_OK;
}

void MainApp::OnMouseDown(WPARAM btnState, const ::POINT& p)
{
}

void MainApp::OnMouseUp(WPARAM btnState, const ::POINT& p)
{
	ReleaseCapture();
}

void MainApp::OnMouseMove(WPARAM btnState, const ::POINT& p)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
	}
}

void MainApp::OnKeyboardInput(const GameTimer& gt)
{
}
