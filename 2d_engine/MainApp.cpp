﻿//--------------------------------------------------------------------------------------------------------------------------------------------------------------
// MainApp

#include <string>
#include <tuple>
#include <d3d12.h>
#include "MainApp.h"
#include "ResourceUploadBatch.h"
#include "Common/G2.FactoryTexture.h"
#include "Common/G2.FactoryShader.h"
#include "Common/G2.FactorySIgnature.h"
#include "Common/G2.FactoryPipelineState.h"
#include "Common/G2.FactorySpine.h"
#include "Common/G2.Geometry.h"
#include "Common/G2.Util.h"
#include "SceneGameMesh.h"
#include "SceneXtkGame.h"
#include "SceneSpine.h"
#include "SceneSample2D.h"
#include "SceneBegin.h"
#include "SceneLobby.h"
#include "ScenePlay.h"
#include "SceneEnd.h"

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
		case EAPP_ATTRIB::EAPP_ATT_XTK_SPRITE:					return m_xtkSprite.get();
		case EAPP_ATTRIB::EAPP_ATT_XTK_GRAPHIC_MEM:				return m_xtkGraphicMem.get();
		case EAPP_ATTRIB::EAPP_ATT_XTK_DESC_HEAP:				return m_xtkDescHeap.get();
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
	switch ((EAPP_CMD)nCmd)
	{
		case EAPP_CMD::EAPP_CMD_CHANGE_SCENE:
		{
			m_sceneIdxNew = any_cast<EAPP_SCENE>(v);
			if(m_sceneIdxCur != m_sceneIdxNew)
			{
				m_bChangeScene = true;
			}
			return S_OK;
		}
	}

	IG2AppFrame::instance()->command(EAPP_CMD_CHANGE_SCENE, EAPP_SCENE_PLAY);

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

	auto d3d    =  IG2GraphicsD3D::instance();
	auto device = std::any_cast<ID3D12Device*       >(d3d->getDevice());
	auto cmdQue = std::any_cast<ID3D12CommandQueue* >(d3d->getCommandQueue());

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
		shader_manager->Load("standardVS"   , "assets/shaders/Default.hlsl"   , "vs_5_0", "VS"                  );
		shader_manager->Load("opaquePS"     , "assets/shaders/Default.hlsl"   , "ps_5_0", "PS", defines         );
		shader_manager->Load("alphaTestedPS", "assets/shaders/Default.hlsl"   , "ps_5_0", "PS", alphaTestDefines);
		auto psoManager = FactoryPipelineState::instance();
	d3d->command(CMD_COMMAND_END);

	// create XTK Instance
	DirectX::ResourceUploadBatch resourceUpload(device);
	{
		auto formatBackBuffer  = *std::any_cast<DXGI_FORMAT*>(d3d->getAttrib(ATT_DEVICE_BACKBUFFER_FORAT));
		auto formatDepthBuffer = *std::any_cast<DXGI_FORMAT*>(d3d->getAttrib(ATT_DEVICE_DEPTH_STENCIL_FORAT));
		const RenderTargetState rtState(formatBackBuffer, formatDepthBuffer);
		SpriteBatchPipelineStateDescription pd(rtState);

		resourceUpload.Begin();
		m_xtkSprite = std::make_unique<SpriteBatch>(device, resourceUpload, pd);
		auto uploadOp = resourceUpload.End(cmdQue);
		uploadOp.wait();

		//setup viewport
		auto vpt = *std::any_cast<D3D12_VIEWPORT*>(d3d->getAttrib(ATT_DEVICE_VIEWPORT));
		m_xtkSprite->SetViewport(vpt);
	}

	m_xtkGraphicMem = std::make_unique<GraphicsMemory>(device);
	m_xtkDescHeap   = std::make_unique<DescriptorHeap>(device, EAPP_DESC_HEAP_SIZE);
	m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(device);


	//AFEW::WORK
	this->ChangeScene(EAPP_SCENE_LOBBY);


	// create XTK Instance
	//{
	//	auto scene = std::make_unique<SceneGameMesh>();
	//	if (scene)
	//	{
	//		//if (SUCCEEDED(scene->Init()))
	//		{
	//			//m_pSceneMesh = std::move(scene);
	//		}
	//	}
	//}
	//{
	//	auto scene = std::make_unique<SceneXtkGame>();
	//	if(scene)
	//	{
	//		if(SUCCEEDED(scene->Init()))
	//		{
	//			m_pSceneXKT = std::move(scene);
	//		}
	//	}
	//}
	//{
	//	auto scene = std::make_unique<SceneSample2D>();
	//	if (scene)
	//	{
	//		//if(SUCCEEDED(scene->Init()))
	//		//{
	//		//	m_pSceneSample = std::move(scene);
	//		//}
	//	}
	//}
	//{
	//	auto scene=std::make_unique<SceneSpine>();
	//	if(scene)
	//	{
	//		if(SUCCEEDED(scene->Init()))
	//		{
	//			m_pSceneSpine = std::move(scene);
	//		}
	//	}
	//}

	return S_OK;
}

int MainApp::destroy()
{
	if(!m_scene.empty())
		m_scene.clear();
	m_pSceneMesh	= {};
	m_pSceneXKT		= {};
	m_pSceneSpine	= {};

	FactorySpine::instance()->UnLoadAll();
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
	int hr = S_OK;
	GameTimer gt = std::any_cast<GameTimer>(t);
	OnKeyboardInput(gt);

	if (m_bChangeScene)
		this->ChangeScene(m_sceneIdxNew);

	if (m_scene[m_sceneIdxCur])
		m_scene[m_sceneIdxCur]->Update(t);


	if(m_pSceneMesh)
		m_pSceneMesh->Update(t);
	if(m_pSceneXKT)
		m_pSceneXKT->Update(t);
	if (m_pSceneSpine)
		m_pSceneSpine->Update(t);
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
	auto psoManager      = FactoryPipelineState::instance();

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
	XMFLOAT4 clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	d3dCommandList->ClearRenderTargetView(d3dBackBufferV, (float*)&clearColor, 0, nullptr);
	d3dCommandList->ClearDepthStencilView(d3dDepthV, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	// Specify the buffers we are going to render to.
	d3dCommandList->OMSetRenderTargets(1, &d3dBackBufferV, true, &d3dDepthV);

	if (m_scene[m_sceneIdxCur])
		m_scene[m_sceneIdxCur]->Render();


	if(m_pSceneMesh)
		m_pSceneMesh->Render();
	if (m_pSceneXKT)
		m_pSceneXKT->Render();
	if (m_pSceneSpine)
		m_pSceneSpine->Render();
	if(m_pSceneSample)
		m_pSceneSample->Render();


	m_xtkGraphicMem->Commit(commandQue);

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

	if (m_scene[m_sceneIdxCur])
	{
		m_scene[m_sceneIdxCur]->Notify("MouseUp", p);
	}
	printf("MainApp::OnMouseUp: %d %d\n", p.x, p.y);
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

void MainApp::ChangeScene(EAPP_SCENE target)
{
	if (EAPP_SCENE_BEGIN > target || target > EAPP_SCENE_COUNT)
		return;

	if (m_scene.empty())
		m_scene.resize(EAPP_SCENE_COUNT);

	if(m_scene[m_sceneIdxCur])
		m_scene[m_sceneIdxCur]->Destroy();

	m_sceneIdxCur = target;
	m_bChangeScene = false;
	if(!m_scene[m_sceneIdxCur])
	{
		switch (target)
		{
			case EAPP_SCENE_BEGIN:	m_scene[m_sceneIdxCur] = std::make_unique<SceneBegin>();	break;
			case EAPP_SCENE_LOBBY:	m_scene[m_sceneIdxCur] = std::make_unique<SceneLobby>();	break;
			case EAPP_SCENE_PLAY:	m_scene[m_sceneIdxCur] = std::make_unique<ScenePlay>();		break;
			case EAPP_SCENE_END:	m_scene[m_sceneIdxCur] = std::make_unique<SceneEnd>();		break;
		}
	}
	m_scene[m_sceneIdxCur]->Init();
}
