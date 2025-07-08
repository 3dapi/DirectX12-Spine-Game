﻿#include <d3d12.h>
#include "Common/G2.Geometry.h"
#include "Common/GameTimer.h"
#include <pix.h>
#include "CommonStates.h"
#include "DirectXHelpers.h"
#include "ResourceUploadBatch.h"
#include "GraphicsMemory.h"

#include "Common/G2.Constants.h"
#include "Common/G2.Geometry.h"
#include "common/G2.ConstantsWin.h"
#include "common/G2.Util.h"
#include "AppCommon.h"
#include "AppCommonXTK.h"
#include "SceneSample2D.h"

using std::any_cast;
using namespace DirectX;

namespace
{
	struct VS_IN
	{
		XMFLOAT2 pos;
		XMFLOAT4 col;
		XMFLOAT2 tex;
	};

	constexpr VS_IN quad[]=
	{
		{{-1.0f,  1.0f}, {1, 1, 1, 1}, {0, 0}},
		{{ 1.0f,  1.0f}, {1, 1, 1, 1}, {1, 0}},
		{{-1.0f, -1.0f}, {1, 1, 1, 1}, {0, 1}},
		{{ 1.0f, -1.0f}, {1, 1, 1, 1}, {1, 1}},
	};

	constexpr uint16_t indices[]={0, 1, 2, 2, 1, 3};

	ComPtr<ID3D12Resource>           m_texture;
	ComPtr<ID3D12DescriptorHeap>     m_srvHeap;
	ComPtr<ID3D12Resource>           m_constBuffer;
	ComPtr<ID3D12PipelineState>      m_pipelineState;
	ComPtr<ID3D12RootSignature>      m_rootSignature;
	ComPtr<ID3D12Resource>           m_vertexBuffer;
	ComPtr<ID3D12Resource>           m_indexBuffer;
}

SceneSample2D::SceneSample2D() {}
SceneSample2D::~SceneSample2D() { Destroy(); }

int SceneSample2D::Init(const std::any&)
{
	HRESULT hr = S_OK;
	hr = InitResource();
	if(FAILED(hr))
		return hr;
	return S_OK;
}

int SceneSample2D::Destroy()
{
	m_cbvHeap		.Reset();
	m_rootSignature	.Reset();
	m_pipelineState	.Reset();

	m_viewVtx		= {};
	m_viewIdx		= {};
	m_numVtx		= {};
	m_numIdx		= {};
	m_cbvHandle		= {};

	m_rscVtxGPU		.Reset();
	m_rscIdxGPU		.Reset();

	if(m_ptrMVP)
	{
		m_cnstMVP	->Unmap(0, nullptr);
		m_cnstMVP	.Reset();
		m_ptrMVP	= {};
		}
	m_spineTextureRsc	.Reset();
	m_spineTexture	= {};

	return S_OK;
}

int SceneSample2D::Update(const std::any& t)
{
	int hr = S_OK;
	auto d3d                =  IG2GraphicsD3D::instance();
	auto currentFrameIndex	= *(std::any_cast<UINT*        >(d3d->getAttrib(ATT_DEVICE_CURRENT_FRAME_INDEX)));

	GameTimer gt = std::any_cast<GameTimer>(t);
	auto deltaTime = gt.DeltaTime();
	auto mvp=XMMatrixIdentity();

	float angle= 0.0F;
	float aspectRatio = 1280.0F / 640.0F;
	XMMATRIX tmPrj = XMMatrixPerspectiveFovLH(XM_PIDIV4 * 1.2F, aspectRatio, 1.0f, 5000.0f);
	static const XMVECTORF32 eye = {0.0f, 10.0f, -900.0f, 0.0f};
	static const XMVECTORF32 at = {0.0f, 0.0f, 0.0f, 0.0f};
	static const XMVECTORF32 up = {0.0f, 1.0f, 0.0f, 0.0f};
	XMMATRIX tmViw = XMMatrixLookAtLH(eye, at, up);
	XMMATRIX tmWld = XMMatrixRotationY((float)angle);
	XMMATRIX mtMVP = tmWld * tmViw * tmPrj;

	{
		uint8_t* dest = m_ptrMVP + (currentFrameIndex * G2::alignTo256(sizeof(XMMATRIX)));
		memcpy(dest, &mtMVP, sizeof(mtMVP));
	}

	return S_OK;
}

int SceneSample2D::Render()
{
	int hr = S_OK;
	auto d3d                =  IG2GraphicsD3D::instance();
	auto d3dDevice          =  std::any_cast<ID3D12Device*              >(d3d->getDevice());
	auto cmdList            =  std::any_cast<ID3D12GraphicsCommandList* >(d3d->getCommandList());
	auto currentFrameIndex	= *(std::any_cast<UINT*                     >(d3d->getAttrib(ATT_DEVICE_CURRENT_FRAME_INDEX)));
	UINT descriptorSize     = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// 1. 루트 시그너처
	cmdList->SetGraphicsRootSignature(m_rootSignature.Get());

	// 2. 디스크립터 힙 설정
	ID3D12DescriptorHeap* descriptorHeaps[] = {m_cbvHeap.Get()};
	cmdList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	// 3. CBV 핸들 계산 및 바인딩 (root parameter index 0 = b0)
	CD3DX12_GPU_DESCRIPTOR_HANDLE handleMVP(m_cbvHeap->GetGPUDescriptorHandleForHeapStart(), 0 * FRAME_BUFFER_COUNT + currentFrameIndex, descriptorSize);

	cmdList->SetGraphicsRootDescriptorTable(0, handleMVP);

	// 4. SRV 핸들 바인딩 (root parameter index 상수 레지스터 다음 3 = t0, 4= t1)
	cmdList->SetGraphicsRootDescriptorTable(1, m_spineTexture);

	// 5. 파이프라인 연결
	cmdList->SetPipelineState(m_pipelineState.Get());


	// 토폴로지
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// 버택스, 인덱스 버퍼
	cmdList->IASetVertexBuffers(0, 1, &m_viewVtx);
	cmdList->IASetIndexBuffer(&m_viewIdx);

	// draw
	cmdList->DrawIndexedInstanced(m_numIdx, 1, 0, 0, 0);

	return S_OK;
}


int SceneSample2D::InitResource()
{
	int hr = S_OK;
	auto d3d                =  IG2GraphicsD3D::instance();
	auto d3dDevice          =  std::any_cast<ID3D12Device*              >(d3d->getDevice());

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// ★★★★★★★★★★★★★★★
	const UINT NUM_CB = 1;						// 셰이더 상수 레지스터 숫자
	const UINT NUM_TX = 1;						// 셰이더 텍스처 텍스처 레지스터
	const UINT NUM_CB_TX = NUM_CB + NUM_TX;
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 1. 상수 + 텍스처 버퍼 heap 생성
	// Create a descriptor heap for the constant buffers.
	{
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.NumDescriptors = FRAME_BUFFER_COUNT * NUM_CB  + NUM_TX;	// 프레임당 상수 버퍼 3개 (b0~b2) * 프레임 수 + 텍스처용 SRV(t0,t0) 2개
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		hr = d3dDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_cbvHeap));
		if(FAILED(hr))
			return hr;
		m_cbvHandle = m_cbvHeap->GetGPUDescriptorHandleForHeapStart();
	}

	//2. Create a root signature with a single constant buffer slot.
	{
		// sampler register 갯수는 상관 없음.
		CD3DX12_STATIC_SAMPLER_DESC staticSampler[] =
		{
			{ 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP},
			{ 1, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP},
		};

		// 2 = 상수 레지스터 1 + 텍스처 레지스터 1
		CD3DX12_DESCRIPTOR_RANGE descRange[NUM_CB_TX];
		descRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0); // b0
		descRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0

		CD3DX12_ROOT_PARAMETER rootParams[2];
		rootParams[0].InitAsDescriptorTable(1, &descRange[0], D3D12_SHADER_VISIBILITY_VERTEX);		// cbv
		rootParams[1].InitAsDescriptorTable(1, &descRange[1], D3D12_SHADER_VISIBILITY_PIXEL);		// src

		CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc;
		rootSigDesc.Init(
			_countof(rootParams),
			rootParams,
			2,					// sampler register 숫자.
			staticSampler,		// sampler register desc
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS
		);

		ComPtr<ID3DBlob> pSignature{}, pError{};
		HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pSignature, &pError);
		if(FAILED(hr))
			return hr;
		hr = d3dDevice->CreateRootSignature(0, pSignature->GetBufferPointer(), pSignature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));
		if(FAILED(hr))
			return hr;
	}
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 2. pipe line 설정
	// compile shader
	ID3DBlob* shaderVtx = {};
	ID3DBlob* shaderPxl = {};
	{
		shaderVtx = G2::DXCompileShaderFromFile("assets/shaders/spine.hlsl", "vs_5_0", "main_vs");
		if(!shaderVtx)
			return E_FAIL;
		shaderPxl = G2::DXCompileShaderFromFile("assets/shaders/spine.hlsl", "ps_5_0", "main_ps");
		if(!shaderPxl)
			return E_FAIL;
	}
	// Create the pipeline state once the shaders are loaded.
	{
		const D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT    , 0, 0                                  , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"COLOR"   , 0, DXGI_FORMAT_R8G8B8A8_UNORM  , 0, sizeof(XMFLOAT2)                   , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT    , 0, sizeof(XMFLOAT2) + sizeof(uint32_t), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		};

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.InputLayout = {inputElementDescs, _countof(inputElementDescs)};
		psoDesc.pRootSignature = m_rootSignature.Get();
		psoDesc.VS = CD3DX12_SHADER_BYTECODE(shaderVtx);
		psoDesc.PS = CD3DX12_SHADER_BYTECODE(shaderPxl);
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = *std::any_cast<DXGI_FORMAT*>(d3d->getAttrib(ATT_DEVICE_BACKBUFFER_FORAT));
		psoDesc.DSVFormat     = *std::any_cast<DXGI_FORMAT*>(d3d->getAttrib(ATT_DEVICE_DEPTH_STENCIL_FORAT));
		psoDesc.SampleDesc.Count = 1;
		hr = d3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState));
		if(FAILED(hr))
			return hr;
	}

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 3. 상수 버퍼용 리소스 생성
	{
		CD3DX12_HEAP_PROPERTIES uploadHeapProperties(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC constantBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(FRAME_BUFFER_COUNT * G2::alignTo256(sizeof XMMATRIX ));
		hr = d3dDevice->CreateCommittedResource(&uploadHeapProperties
												, D3D12_HEAP_FLAG_NONE
												, &constantBufferDesc
												, D3D12_RESOURCE_STATE_GENERIC_READ
												, nullptr, IID_PPV_ARGS(&m_cnstMVP));
		if(FAILED(hr))
			return hr;
	}

	{
		UINT d3dDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		const UINT bufferSize = G2::alignTo256(sizeof XMMATRIX);
		// b0: Wld
		{
			D3D12_GPU_VIRTUAL_ADDRESS cbvGpuAddress = m_cnstMVP->GetGPUVirtualAddress();
			D3D12_CPU_DESCRIPTOR_HANDLE cbvCpuHandle = m_cbvHeap->GetCPUDescriptorHandleForHeapStart();
			// offset 0
			for(int n=0; n<FRAME_BUFFER_COUNT; ++n)
			{
				D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
				desc.BufferLocation = cbvGpuAddress;
				desc.SizeInBytes = bufferSize;
				d3dDevice->CreateConstantBufferView(&desc, cbvCpuHandle);
				cbvGpuAddress += bufferSize;
				cbvCpuHandle.ptr += d3dDescriptorSize;
			}
			CD3DX12_RANGE readRange(0, 0);
			hr = m_cnstMVP->Map(0, &readRange, reinterpret_cast<void**>(&m_ptrMVP));
			if(FAILED(hr)) return hr;
		}
	}

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 5 텍스처 생성 및 업로드 (CreateWICTextureFromFile + ResourceUploadBatch)
	{
		DirectX::ResourceUploadBatch resourceUpload(d3dDevice);
		{
			resourceUpload.Begin();
			hr = DirectX::CreateWICTextureFromFile(d3dDevice, resourceUpload, L"assets/texture/res_checker.png", m_spineTextureRsc.GetAddressOf());
			if(FAILED(hr))
				return hr;
			auto commandQueue = std::any_cast<ID3D12CommandQueue*>(d3d->getCommandQueue());
			auto uploadOp = resourceUpload.End(commandQueue);
			uploadOp.wait();  // GPU 업로드 완료 대기
		}
	}

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 6 텍스처 레지스터 SRV 디스크립터 생성
	// 실수 많이함. rootSigDesc 초기화 부분과 일치해야함.
	// ★★★★★★★★★★★★★★★
	// 3개 상수 레지스터 다음부터 텍스처 레지스터(셰이더 참고)
	//FRAME_BUFFER_COUNT * NUM_CB
	{
		UINT baseSRVIndex = FRAME_BUFFER_COUNT * NUM_CB;
		UINT descriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv(m_cbvHeap->GetCPUDescriptorHandleForHeapStart(), baseSRVIndex, descriptorSize);
		CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv(m_cbvHeap->GetGPUDescriptorHandleForHeapStart(), baseSRVIndex, descriptorSize);

		// texture viewer 생성
		{
			hCpuSrv.Offset(0, descriptorSize);			//
			hGpuSrv.Offset(0, descriptorSize);			//
			m_spineTexture = hGpuSrv;					// CPU, GPU OFFSET을 이동후 Heap pointer 위치를 저장 이 핸들 값이 텍스처 핸들

			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.Format = m_spineTextureRsc->GetDesc().Format;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = 1;
			d3dDevice->CreateShaderResourceView(m_spineTextureRsc.Get(), &srvDesc, hCpuSrv);
		}
	}

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 7. 리소스 버텍스 버퍼 업로드
	// 버텍스 버퍼는 CreateCommittedResource 내부에서 heap 사용?
	G2::VTX2D_DT rectangle[] =
	{
		{{-200.0f,  200.0f}, rgba2uint32(255,   0,   0, 255), {0.0f, 0.0f}},
		{{ 200.0f,  200.0f}, rgba2uint32(  0, 255,   0, 255), {1.0f, 0.0f}},
		{{ 200.0f, -200.0f}, rgba2uint32(  0,   0, 255, 255), {1.0f, 1.0f}},
		{{-200.0f, -200.0f}, rgba2uint32(255,   0, 255, 255), {0.0f, 1.0f}},
	};
	uint16_t indices[] = { 0, 1, 2, 0, 2, 3, };

	{
		m_numVtx = sizeof(rectangle) / sizeof(rectangle[0]);
		// vertex buffer
		CD3DX12_HEAP_PROPERTIES vtxHeapPropsGPU		(D3D12_HEAP_TYPE_DEFAULT);
		CD3DX12_HEAP_PROPERTIES vtxHeapPropsUpload	(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC   vtxBufDesc			= CD3DX12_RESOURCE_DESC::Buffer(m_numVtx * sizeof(G2::VTX2D_DT));
		// GPU용 버텍스 버퍼
		hr = d3dDevice->CreateCommittedResource(&vtxHeapPropsGPU, D3D12_HEAP_FLAG_NONE, &vtxBufDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_rscVtxGPU));
		if (FAILED(hr))
			return hr;
		// CPU 업로드 버퍼
		hr = d3dDevice->CreateCommittedResource(&vtxHeapPropsUpload, D3D12_HEAP_FLAG_NONE, &vtxBufDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_rscVtxCPU));
		if (FAILED(hr))
			return hr;
		// Create vertex/index buffer views.
		m_viewVtx.BufferLocation = m_rscVtxGPU->GetGPUVirtualAddress();
		m_viewVtx.StrideInBytes = sizeof(G2::VTX2D_DT );
		m_viewVtx.SizeInBytes = sizeof(rectangle);
	}

	{
		m_numIdx = sizeof(indices) / sizeof(indices[0]);

		// index buffer
		CD3DX12_HEAP_PROPERTIES idxHeapPropsGPU		(D3D12_HEAP_TYPE_DEFAULT);
		CD3DX12_HEAP_PROPERTIES idxHeapPropsUpload	(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC	idxBufDesc			= CD3DX12_RESOURCE_DESC::Buffer(m_numIdx * sizeof(uint16_t));
		// GPU용 인덱스 버퍼
		hr = d3dDevice->CreateCommittedResource(&idxHeapPropsGPU, D3D12_HEAP_FLAG_NONE, &idxBufDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_rscIdxGPU));
		if (FAILED(hr))
			return hr;
		// CPU 업로드 버퍼
		hr = d3dDevice->CreateCommittedResource(&idxHeapPropsUpload, D3D12_HEAP_FLAG_NONE, &idxBufDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_rscIdxCPU));
		if (FAILED(hr))
			return hr;
		// Create vertex/index buffer views.
		m_viewIdx.BufferLocation = m_rscIdxGPU->GetGPUVirtualAddress();
		m_viewIdx.SizeInBytes = sizeof(indices);
		m_viewIdx.Format = DXGI_FORMAT_R16_UINT;
	}

	auto cmdList    =  std::any_cast<ID3D12GraphicsCommandList* >(d3d->getCommandList());
	auto cmdAlloc   =  std::any_cast<ID3D12CommandAllocator*    >(d3d->getCommandAllocator());
	auto cmdQueue   =  std::any_cast<ID3D12CommandQueue*        >(d3d->getCommandQueue());

	hr = cmdAlloc->Reset();
	if(FAILED(hr))
		return hr;
	hr = cmdList->Reset(cmdAlloc, nullptr);  // PSO는 루프 내에서 설정 예정
	if(FAILED(hr))
		return hr;
	{
		// 업로드 수행
		// upload 요청하면 완료될때까지 모든 데이터가 유효해야 한다.
		{
			D3D12_SUBRESOURCE_DATA vertexData = {};
			vertexData.pData      = reinterpret_cast<const BYTE*>(rectangle);
			vertexData.RowPitch   = m_numVtx * sizeof(G2::VTX2D_DT);
			vertexData.SlicePitch = m_numVtx * sizeof(G2::VTX2D_DT);
			UpdateSubresources(cmdList, m_rscVtxGPU.Get(), m_rscVtxCPU.Get(), 0, 0, 1, &vertexData);
			auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_rscVtxGPU.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
			cmdList->ResourceBarrier(1, &barrier);
		}

			// 업로드 수행
		// upload 요청하면 완료될때까지 모든 데이터가 유효해야 한다.
		{
			D3D12_SUBRESOURCE_DATA indexData = {};
			indexData.pData      = reinterpret_cast<const BYTE*>(indices);
			indexData.RowPitch   = m_numIdx * sizeof(uint16_t);
			indexData.SlicePitch = m_numIdx * sizeof(uint16_t);
			UpdateSubresources(cmdList, m_rscIdxGPU.Get(), m_rscIdxCPU.Get(), 0, 0, 1, &indexData);
			auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_rscIdxGPU.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);
			cmdList->ResourceBarrier(1, &barrier);
		}
	}
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 마지막으로 cmdList를 닫고
	hr = cmdList->Close();
	if(FAILED(hr))
		return hr;

	// 실행하고
	ID3D12CommandList* ppCommandLists[] = {cmdList};
	cmdQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// gpu 완료 될 때 까지 기다림
	d3d->command(CMD_WAIT_GPU);

	return S_OK;
}
