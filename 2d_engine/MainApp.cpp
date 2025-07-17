//--------------------------------------------------------------------------------------------------------------------------------------------------------------
// MainApp

#include <string>
#include <tuple>
#include <d3d12.h>
#include "MainApp.h"
#include "ResourceUploadBatch.h"
#include "Common/G2.FactoryMfAudio.h"
#include "Common/G2.FactoryFontResource.h"
#include "Common/G2.FactoryCamera.h"
#include "Common/G2.FactoryTexture.h"
#include "Common/G2.FactoryShader.h"
#include "Common/G2.FactorySIgnature.h"
#include "Common/G2.FactoryPipelineState.h"
#include "Common/G2.FactorySpine.h"
#include "Common/G2.Geometry.h"
#include "Common/G2.Util.h"
#include "SceneBegin.h"
#include "SceneLobby.h"
#include "ScenePlay.h"
#include "SceneEnd.h"
#include "SceneSpine.h"
#include "SceneFont.h"

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
	// GameInfo 객체 생성 확인.
	auto pGameInfo = GameInfo::instance();
}

MainApp::~MainApp()
{
	destroy();
}

std::any MainApp::getAttrib(int nAttrib)
{
	switch((EAPP_ATTRIB)nAttrib)
	{
		case EAPP_ATTRIB::EAPP_ATT_WIN_HWND:		return mhMainWnd;
		case EAPP_ATTRIB::EAPP_ATT_WIN_HINST:		return mhAppInst;
		case EAPP_ATTRIB::EAPP_ATT_XTK_SPRITE:		return m_xtkSprite.get();
		case EAPP_ATTRIB::EAPP_ATT_XTK_GRAPHIC_MEM:	return m_xtkGraphicMem.get();
		case EAPP_ATTRIB::EAPP_ATT_XTK_DESC_HEAP:	return m_xtkDescHeap.get();
		case EAPP_ATTRIB::EAPP_ATT_XTK_BATCH:		return m_batch.get();
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
			if(m_sceneIdxCur != m_sceneIdxNew) {
				m_bChangeScene = true;
			}
			return S_OK;
		}
	}
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

	
	// create XTK Instance
	DirectX::ResourceUploadBatch resourceUpload(device);
	{
		auto formatBackBuffer = *std::any_cast<DXGI_FORMAT*>(d3d->getAttrib(ATT_DEVICE_BACKBUFFER_FORAT));
		auto formatDepthBuffer = *std::any_cast<DXGI_FORMAT*>(d3d->getAttrib(ATT_DEVICE_DEPTH_STENCIL_FORAT));
		const RenderTargetState rtState(formatBackBuffer, formatDepthBuffer);

		// 🔥 알파 블렌드 설정 추가
		CD3DX12_BLEND_DESC alphaBlendDesc(D3D12_DEFAULT);
		alphaBlendDesc.RenderTarget[0].BlendEnable = TRUE;
		alphaBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		alphaBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		alphaBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		alphaBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		alphaBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		alphaBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		alphaBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		SpriteBatchPipelineStateDescription pd(rtState, &alphaBlendDesc); // 🔥 blend desc 전달

		resourceUpload.Begin();
		m_xtkSprite = std::make_unique<SpriteBatch>(device, resourceUpload, pd); // 🔥 알파 블렌딩 적용된 PSO
		auto uploadOp = resourceUpload.End(cmdQue);
		uploadOp.wait();

		//setup viewport
		auto vpt = *std::any_cast<D3D12_VIEWPORT*>(d3d->getAttrib(ATT_DEVICE_VIEWPORT));
		m_xtkSprite->SetViewport(vpt);
	}

	//AFEW::D
	m_xtkGraphicMem = std::make_unique<GraphicsMemory>(device);
	m_xtkDescHeap   = std::make_unique<DescriptorHeap>(device, EAPP_DESC_HEAP_SIZE);
	m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(device);

	FactoryCamera::instance()->Load(IG2Camera::SPINE_2D, EG2CAMERA::EG2CAM_2D);
	{
		auto cameraSpine = FactoryCamera::instance()->FindRes(IG2Camera::SPINE_2D);
		cameraSpine->Position({0.0f, 0.0f,-700.0f});
		cameraSpine->LookAt  ({0.0f, 0.0f,   0.0f});
		cameraSpine->Update  ();
	}

	FactoryFontResource::instance()->Load("고도 B", "asset/font/GodoB.ttf");

	//m_bgMusic = MfAudioPlayer::Create("asset/sound/gametown-_-cutscene-_-narrative-373823.mp3");
	m_bgMusic = MfAudioPlayer::Create("asset/sound/fight-for-the-future-336841.mp3");

	//AFEW::WORK
	this->ChangeScene(EAPP_SCENE::EAPP_SCENE_BEGIN);

	m_bgMusic->Play(true, 0.4F);

	return S_OK;
}

int MainApp::destroy()
{
	if(!m_scene.empty())
		m_scene.clear();

	GameInfo::deleteInstance();

	m_xtkSprite		.reset();
	m_xtkGraphicMem	.reset();
	m_xtkDescHeap	.reset();
	m_batch			.reset();

	SAFE_DELETE(m_bgMusic);

	FactoryMfAudio::instance()->UnLoadAll();
	FactoryMfAudio::instance()->UnLoadAudioLibrary();
	FactoryFontResource::instance()->UnLoadAll();
	FactoryCamera::instance()->UnLoadAll();
	FactorySpine::instance()->UnLoadAll();
	FactoryTexture::instance()->UnLoadAll();
	FactoryShader::instance()->UnLoadAll();
	FactoryPipelineState::instance()->UnLoadAll();
	FactorySignature::instance()->UnLoadAll();

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
	OnKeyboardInput();

	if (m_bChangeScene)
		this->ChangeScene(m_sceneIdxNew);

	if (m_scene[m_sceneIdxCur])
		m_scene[m_sceneIdxCur]->Update(t);

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

	hr = d3dCommandAlloc->Reset();
	ThrowIfFailed(hr);
	if (FAILED(hr))
		return hr;

	hr = d3dCommandList->Reset(d3dCommandAlloc, nullptr);
	ThrowIfFailed(hr);
	if (FAILED(hr))
		return hr;

	d3dCommandList->RSSetViewports(1, d3dViewport);
	d3dCommandList->RSSetScissorRects(1, d3dScissor);
	d3dCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(d3dBackBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	XMFLOAT4 clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	d3dCommandList->ClearRenderTargetView(d3dBackBufferV, (float*)&clearColor, 0, nullptr);
	d3dCommandList->ClearDepthStencilView(d3dDepthV, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	d3dCommandList->OMSetRenderTargets(1, &d3dBackBufferV, true, &d3dDepthV);

	if (m_scene[m_sceneIdxCur])
		m_scene[m_sceneIdxCur]->Render();

	// XTK memory clear.!!!!!!
	m_xtkGraphicMem->Commit(commandQue);

	d3dCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(d3dBackBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	hr = d3dCommandList->Close();
	ThrowIfFailed(hr);
	if (FAILED(hr))
		return hr;

	ID3D12CommandList* cmdsLists[] = { d3dCommandList };
	commandQue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	hr = d3d->command(EG2GRAPHICS_D3D::CMD_PRESENT);
	hr = d3d->command(EG2GRAPHICS_D3D::CMD_WAIT_GPU);
	return S_OK;
}

int MainApp::Pause()
{
	if(m_bgMusic)
		m_bgMusic->Pause();
	return S_OK;
}

int MainApp::Resume()
{
	if(m_bgMusic)
		m_bgMusic->Resume();
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
	//printf("MainApp::OnMouseUp: %d %d\n", p.x, p.y);
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

void MainApp::OnKeyboardInput()
{
	// Keyboard debugging
	//for (int i = 0; i < EAPP_MAX_KEY; ++i)
	//{
	//	if (GetAsyncKeyState(i) & 0x8000)
	//	{
	//		printf("VK %d (0x%02X) is down\n", i, i);
	//	}
	//}

	bool isEvent = false;
	memcpy(m_keyOld, m_keyNew, EAPP_MAX_KEY);
	for (int i = 0; i < EAPP_MAX_KEY; ++i)
	{
		if (i == 21) continue;
		m_keyNew[i] = (GetAsyncKeyState(i) & 0x8000) ? 1 : 0;
		if      (0 == m_keyOld[i] && 0 == m_keyNew[i])		// no event
		{
			m_keyCur[i] = (int)EAPP_INPUT_NONE;
		}
		else if (0 == m_keyOld[i] && 1 == m_keyNew[i])		// down
		{
			m_keyCur[i] = (int)EAPP_INPUT_DOWN;
		}
		else if (1 == m_keyOld[i] && 1 == m_keyNew[i])		// pressed
		{
			m_keyCur[i] = (int)EAPP_INPUT_PRESS;
		}
		else if (1 == m_keyOld[i] && 0 == m_keyNew[i])		// up
		{
			m_keyCur[i] = (int)EAPP_INPUT_UP;
		}

		if (!isEvent && m_keyCur[i] != (int)EAPP_INPUT_NONE)
			isEvent = true;
	}

	if (isEvent && m_scene[m_sceneIdxCur])
	{
		m_scene[m_sceneIdxCur]->Notify("KeyEvent", static_cast<const uint8_t*>(m_keyCur));
	}
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
			case EAPP_SCENE_SPINE:	m_scene[m_sceneIdxCur] = std::make_unique<SceneSpine>();	break;
			case EAPP_SCENE_FONT:	m_scene[m_sceneIdxCur] = std::make_unique<SceneFont>();		break;
		}
	}
	m_scene[m_sceneIdxCur]->Init();
}
