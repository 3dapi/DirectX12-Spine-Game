
#include <any>
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
#include "DirectXHelpers.h"
#include "ResourceUploadBatch.h"
#include "GraphicsMemory.h"
#include "SpineRender.h"

using namespace std;
using std::any_cast;
using namespace DirectX;
using namespace spine;
using namespace G2;


SpineRender::SpineRender()
{
}

SpineRender::~SpineRender()
{
	Destroy();
}

int SpineRender::Init(const std::any& initial_value)
{
	if (!initial_value.has_value()) {
		return E_FAIL;
	}
	HRESULT hr = S_OK;
	m_attrib = std::any_cast<const SPINE_ATTRIB&>(initial_value);
	hr = InitSpine();
	if (FAILED(hr))
		return hr;
	hr = InitD3DResource();
	if (FAILED(hr))
		return hr;
	return S_OK;
}

int SpineRender::Destroy()
{
	m_cbvHeap		.Reset();
	m_maxVtxCount	= {};
	m_maxIdxCount	= {};
	m_descGpuHandle = {};
	m_descCpuHandle	= {};

	m_cnstMVP		->Unmap(0, nullptr);
	m_cnstMVP		.Reset();
	m_ptrMVP		= {};
	m_textureHandle	= {};

	return S_OK;
}

int SpineRender::Update(const std::any& t)
{
	GameTimer gt = std::any_cast<GameTimer>(t);
	auto deltaTime = gt.DeltaTime();
	deltaTime *= m_attrib.aniSpeed;

	float aspectRatio = *any_cast<float*>(IG2GraphicsD3D::instance()->getAttrib(ATT_ASPECTRATIO));

	XMMATRIX tmPrj = XMMatrixPerspectiveFovLH(XM_PI/3.0F, aspectRatio, 1.0f, 5000.0f);
	static const XMVECTORF32 eye = { 0.0f, 0.0f, -700.0f, 0.0f };
	static const XMVECTORF32  at = { 0.0f, 0.0f, 0.0f, 0.0f };
	static const XMVECTORF32  up = { 0.0f, 1.0f, 0.0f, 0.0f };
	XMMATRIX tmViw = XMMatrixLookAtLH(eye, at, up);
	XMMATRIX tmWld = XMMatrixIdentity();
	XMMATRIX mtMVP = tmWld * tmViw * tmPrj;

	auto currentFrameIndex = *(any_cast<UINT*>(IG2GraphicsD3D::instance()->getAttrib(ATT_DEVICE_CURRENT_FRAME_INDEX)));
	{
		uint8_t* dest = m_ptrMVP + (currentFrameIndex * G2::alignTo256(sizeof(XMMATRIX)));
		memcpy(dest, &mtMVP, sizeof(mtMVP));
	}

	// Update and apply the animation state to the skeleton
	m_spineAniState->update(deltaTime);
	m_spineAniState->apply(*m_spineSkeleton);

	// Update the skeleton time (used for physics)
	m_spineSkeleton->update(deltaTime);

	// Calculate the new pose
	m_spineSkeleton->updateWorldTransform(spine::Physics_None);

	// Update spine draw buffer
	UpdateDrawBuffer();

	return S_OK;
}

int SpineRender::Render()
{
	auto d3d        =  IG2GraphicsD3D::instance();
	auto device     = std::any_cast<ID3D12Device*             >(d3d->getDevice());
	auto cmdList    = std::any_cast<ID3D12GraphicsCommandList*>(d3d->getCommandList());
	UINT frameIndex = *std::any_cast<UINT*                    >(d3d->getAttrib(ATT_DEVICE_CURRENT_FRAME_INDEX));

	auto rsoManager = FactorySignature::instance();
	auto rsoItem    = rsoManager->FindRes("PLS2D_SPINE0");

	auto psoManager = FactoryPipelineState::instance();
	auto psoItem = psoManager->FindRes("PLS2D_SPINE0");

	cmdList->SetGraphicsRootSignature(rsoItem);
	ID3D12DescriptorHeap* descriptorHeaps[] = {m_cbvHeap.Get()};
	cmdList->SetDescriptorHeaps(1, descriptorHeaps);

	CD3DX12_GPU_DESCRIPTOR_HANDLE cbvHandle(m_descGpuHandle, frameIndex, m_descriptorSize);
	cmdList->SetGraphicsRootDescriptorTable(0, cbvHandle);
	cmdList->SetGraphicsRootDescriptorTable(1, m_textureHandle);
	cmdList->SetPipelineState(psoItem);

	for(int i=0; i<m_drawCount; ++i)
	{
		auto& buf = m_drawBuf[i];
		cmdList->IASetVertexBuffers(0, 1, &buf.vbvPos);
		cmdList->IASetVertexBuffers(1, 1, &buf.vbvDif);
		cmdList->IASetVertexBuffers(2, 1, &buf.vbvTex);
		cmdList->IASetIndexBuffer(&buf.ibv);
		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cmdList->DrawIndexedInstanced(buf.ibv.SizeInBytes/sizeof(uint16_t), 1, 0, 0, 0);
	}

	return S_OK;
}

void SpineRender::Position(const XMFLOAT2& v)
{
	m_pos = v;
	auto x = m_attrib.vecOffset.x + m_pos.x;
	auto y = m_attrib.vecOffset.y + m_pos.y;
	m_spineSkeleton->setPosition(x, y);
}
XMFLOAT2 SpineRender::Position() const
{
	return m_pos;
}
void SpineRender::Direction(float v)
{
	m_dir = v;
	auto s = m_attrib.vecScale * m_scale * m_dir;
	m_spineSkeleton->setScaleX(s);
}
float SpineRender::Direction() const
{
	return m_dir;
}
void SpineRender::Scale(float v)
{
	m_scale = v;
	auto x = m_attrib.vecScale * m_scale * m_dir;
	auto y = m_attrib.vecScale * m_scale;
	m_spineSkeleton->setScaleX(x);
	m_spineSkeleton->setScaleY(y);
}
float SpineRender::Scale() const
{
	return m_scale;
}
void SpineRender::Color(const XMFLOAT4& v)
{
	m_color = v;
}
XMFLOAT4 SpineRender::Color() const
{
	return m_color;
}

int SpineRender::UpdateDrawBuffer()
{
	int hr = S_OK;
	auto d3d        =  IG2GraphicsD3D::instance();
	auto device     = std::any_cast<ID3D12Device*             >(d3d->getDevice());
	auto cmdList    = std::any_cast<ID3D12GraphicsCommandList*>(d3d->getCommandList());
	auto cmdAlloc   = std::any_cast<ID3D12CommandAllocator*   >(d3d->getCommandAllocator());
	auto cmdQue     = std::any_cast<ID3D12CommandQueue*       >(d3d->getCommandQueue());

	hr = cmdAlloc->Reset();
	if(FAILED(hr))
		return hr;
	hr = cmdList->Reset(cmdAlloc, nullptr);
	if(FAILED(hr))
		return hr;
	
	m_drawCount = {};
	//printf("------------------------------------------------------------\n\n");
	const auto& drawOrder = m_spineSkeleton->getDrawOrder();
	const auto slotCount = drawOrder.size();
	for(size_t i = 0; i < slotCount; ++i)
	{
		auto* slot = drawOrder[i];
		auto name = slot->getData().getName().buffer();
		auto* attachment = slot->getAttachment();

		if(!attachment)
			continue;

		SPINE_DRAW_BUFFER*		buf  = {};
		MeshAttachment*		meshAttachment   = {};
		RegionAttachment*	regionAttachment = {};

		auto isMesh = attachment->getRTTI().isExactly(spine::MeshAttachment::rtti);
		auto isResion = attachment->getRTTI().isExactly(spine::RegionAttachment::rtti);
		if (!(isMesh || isResion))
			continue;


		//printf("order: %d  name %s\n", (int)i, name);

		if (isMesh)
		{
			auto* mesh = static_cast<spine::MeshAttachment*>(attachment);
			auto* texRegion = mesh->getRegion();
			if (!texRegion)
				continue;
			meshAttachment = mesh;
		}
		else if (isResion)
		{
			auto* region = static_cast<spine::RegionAttachment*>(attachment);
			auto* texRegion = region->getRegion();
			if (!texRegion)
				continue;
			regionAttachment = region;
		}

		buf = &m_drawBuf[m_drawCount++];

		uint32_t    rgba        = 0xFFFFFFFF;
		UINT        idxCount    = {};
		UINT        vtxCount    = {};
		UINT        posSize     = {};
		UINT        difSize     = {};
		UINT        texSize     = {};
		UINT        idxSize     = {};

		if(isMesh)
		{
			float*    spineTex = const_cast<float*>(meshAttachment->getUVs().buffer());
			uint16_t* spineIdx = meshAttachment->getTriangles().buffer();
			
			vtxCount    = (UINT)meshAttachment->getWorldVerticesLength() / 2;
			idxCount    = (UINT)meshAttachment->getTriangles().size();

			spine::Color c = slot->getColor();
			spine::Color m = meshAttachment->getColor();
			rgba =
				((uint32_t)(m_color.w * c.a * m.a * 255) << 24) |
				((uint32_t)(m_color.x * c.r * m.r * 255) << 16) |
				((uint32_t)(m_color.y * c.g * m.g * 255) << 8) |
				((uint32_t)(m_color.z * c.b * m.b * 255) << 0);

			posSize = sizeof(XMFLOAT2) * vtxCount;
			difSize = sizeof(uint32_t) * vtxCount;
			texSize = sizeof(XMFLOAT2) * vtxCount;
			idxSize = sizeof(uint16_t) * idxCount;

			// Upload → GPU 복사 (Position)
			{
				XMFLOAT2* ptrPos = nullptr;
				buf->rscPosCPU->Map(0, nullptr, (void**)&ptrPos);
				meshAttachment->computeWorldVertices(*slot, 0, meshAttachment->getWorldVerticesLength(), (float*)ptrPos, 0, 2);
				buf->rscPosCPU->Unmap(0, nullptr);
			}
			// Upload → GPU 복사 (texture coord)
			{
				XMFLOAT2* ptrTex = nullptr;
				buf->rscTexCPU->Map(0, nullptr, (void**)&ptrTex);
				avx2_memcpy(ptrTex, spineTex, texSize);
				buf->rscTexCPU->Unmap(0, nullptr);
			}
			// Upload → GPU 복사 (diffuse)
			{
				uint32_t* ptrDif = nullptr;
				buf->rscDifCPU->Map(0, nullptr, (void**)&ptrDif);
				avx2_memset32(ptrDif, rgba, vtxCount);
				buf->rscDifCPU->Unmap(0, nullptr);
			}
			// Index
			{
				uint16_t* ptrIdx = nullptr;
				buf->rscIdxCPU->Map(0, nullptr, (void**)&ptrIdx);
				avx2_memcpy(ptrIdx, spineIdx, idxSize);
				buf->rscIdxCPU->Unmap(0, nullptr);
			}
		}
		else if (isResion)
		{
			static const uint16_t spineIdx[] = { 0, 1, 2, 2, 3, 0 };
			float* spineTex = const_cast<float*>(regionAttachment->getUVs().buffer());

			vtxCount    = 4;
			idxCount    = 6;

			auto c = slot->getColor();
			auto r = regionAttachment->getColor();
			rgba =
				((uint32_t)(m_color.w * c.a * r.a * 255) << 24) |
				((uint32_t)(m_color.x * c.r * r.r * 255) << 16) |
				((uint32_t)(m_color.y * c.g * r.g * 255) << 8) |
				((uint32_t)(m_color.z * c.b * r.b * 255) << 0);

			posSize = sizeof(XMFLOAT2) * vtxCount;
			difSize = sizeof(uint32_t) * vtxCount;
			texSize = sizeof(XMFLOAT2) * vtxCount;
			idxSize = sizeof(uint16_t) * idxCount;

			// Position
			{
				XMFLOAT2* ptrPos = nullptr;
				buf->rscPosCPU->Map(0, nullptr, (void**)&ptrPos);
				regionAttachment->computeWorldVertices(*slot, (float*)ptrPos, 0, 2);
				buf->rscPosCPU->Unmap(0, nullptr);
			}
			// texture coord
			{
				XMFLOAT2* ptrTex = nullptr;
				buf->rscTexCPU->Map(0, nullptr, (void**)&ptrTex);
				avx2_memcpy(ptrTex, spineTex, texSize);
				buf->rscTexCPU->Unmap(0, nullptr);
			}
			// diffuse
			{
				uint32_t* ptrDif = nullptr;
				buf->rscDifCPU->Map(0, nullptr, (void**)&ptrDif);
				avx2_memset32(ptrDif, rgba, vtxCount);
				buf->rscDifCPU->Unmap(0, nullptr);
			}
			// Index
			{
				uint16_t* ptrIdx = nullptr;
				buf->rscIdxCPU->Map(0, nullptr, (void**)&ptrIdx);
				avx2_memcpy(ptrIdx, spineIdx, idxSize);
				buf->rscIdxCPU->Unmap(0, nullptr);
			}
		}

		cmdList->CopyBufferRegion(buf->rscPosGPU.Get(), 0, buf->rscPosCPU.Get(), 0, posSize);
		CD3DX12_RESOURCE_BARRIER barPos = CD3DX12_RESOURCE_BARRIER::Transition(buf->rscPosGPU.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		cmdList->ResourceBarrier(1, &barPos);

		cmdList->CopyBufferRegion(buf->rscDifGPU.Get(), 0, buf->rscDifCPU.Get(), 0, difSize);
		CD3DX12_RESOURCE_BARRIER barDif = CD3DX12_RESOURCE_BARRIER::Transition(buf->rscDifGPU.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		cmdList->ResourceBarrier(1, &barDif);

		cmdList->CopyBufferRegion(buf->rscTexGPU.Get(), 0, buf->rscTexCPU.Get(), 0, texSize);
		CD3DX12_RESOURCE_BARRIER barTex = CD3DX12_RESOURCE_BARRIER::Transition(buf->rscTexGPU.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		cmdList->ResourceBarrier(1, &barTex);

		cmdList->CopyBufferRegion(buf->rscIdxGPU.Get(), 0, buf->rscIdxCPU.Get(), 0, idxSize);
		CD3DX12_RESOURCE_BARRIER barIdx = CD3DX12_RESOURCE_BARRIER::Transition(buf->rscIdxGPU.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);
		cmdList->ResourceBarrier(1, &barIdx);

		// Bind
		buf->numVb  = vtxCount;
		buf->numIb  = idxCount;
		buf->vbvPos = {buf->rscPosGPU->GetGPUVirtualAddress(), posSize, sizeof(XMFLOAT2)};
		buf->vbvDif = {buf->rscDifGPU->GetGPUVirtualAddress(), difSize, sizeof(uint32_t)};
		buf->vbvTex = {buf->rscTexGPU->GetGPUVirtualAddress(), texSize, sizeof(XMFLOAT2)};
		buf->ibv    = {buf->rscIdxGPU->GetGPUVirtualAddress(), idxSize, DXGI_FORMAT_R16_UINT };
	}

	hr = cmdList->Close();
	if(FAILED(hr))
		return hr;
	ID3D12CommandList* ppCmdLists[] = {cmdList};
	cmdQue->ExecuteCommandLists(_countof(ppCmdLists), ppCmdLists);
	d3d->command(CMD_WAIT_GPU);
	return S_OK;
}

int SpineRender::InitSpine()
{
	auto spineManager = FactorySpine::instance();
	auto resourceRoot = string("asset/spine/");
	decltype(auto) atlasPath = resourceRoot + m_attrib.spine_name + string("/") + m_attrib.atlasPath;
	decltype(auto) skelPath = resourceRoot + m_attrib.spine_name + string("/") + m_attrib.skelPath;
	auto itemSpine = spineManager->Load(m_attrib.spine_name, atlasPath, skelPath);
	if (!itemSpine)
		return E_FAIL;

	m_spineRsc = itemSpine;
	m_spineSkeleton = new Skeleton(m_spineRsc->skelData);
	m_spineAnimation = FactorySpine::getAnimationList(m_spineSkeleton);


#if 0	//DEBUG_PRINT
	printf("%s\n", m_attrib.spine_name.c_str());
	for (const auto& n : m_spineAnimation)
	{
		printf("\t%s\n", n.c_str());
	}
#endif

	if (!m_attrib.skinName.empty())
	{
		m_spineSkeleton->setSkin(m_attrib.skinName.c_str());		// 스킨 변경
		m_spineSkeleton->setSlotsToSetupPose();						// 슬롯(attachment) 갱신
		m_spineSkeleton->updateWorldTransform(spine::Physics_None);	// 본 transform 계산
		for (size_t i = 0; i < m_attrib.detachSlot.size(); ++i)
		{
			const auto& slotName = m_attrib.detachSlot[i];
			auto* slot = m_spineSkeleton->findSlot(slotName.c_str());
			if (slot)
				slot->setAttachment(nullptr);
		}
	}

	AnimationStateData animationStateData(m_spineRsc->skelData);
	animationStateData.setDefaultMix(0.2f);
	m_spineAniState = new AnimationState(&animationStateData);

	auto itr = std::find_if(m_spineAnimation.begin(), m_spineAnimation.end(), [&](const string& it)      { return m_attrib.aniName == it; });
	// 애니메이션 못찾으면 0 번째 실행.
	if(itr == m_spineAnimation.end())
	{
		m_spineAniState->setAnimationByIndex(0, 0, true);
	}
	else
	{
		m_spineAniState->setAnimation(0, m_attrib.aniName.c_str(), true);
	}

	m_spineSkeleton->setPosition(m_attrib.vecOffset.x, m_attrib.vecOffset.y);
	m_spineSkeleton->setScaleX(m_attrib.vecScale);
	m_spineSkeleton->setScaleY(m_attrib.vecScale);
	m_spineAniState->update(m_attrib.aniBegin);
	m_spineAniState->apply(*m_spineSkeleton);

#if 0 //DEBUG
	auto& slotList  = m_spineSkeleton->getSlots();
	for (size_t i = 0; i < slotList.size(); ++i)
	{
		spine::Slot* slot= slotList[i];
		spine::Attachment* att = slot->getAttachment();
		if (att)
		{
			printf("Slot: %s , Attachment: %s\n", slot->getData().getName().buffer(), att->getName().buffer());
		}
	}
#endif

	SetupDrawBuffer();
	return S_OK;
}

int SpineRender::InitD3DResource()
{
	HRESULT hr = S_OK;
	auto d3d        =  IG2GraphicsD3D::instance();
	auto device     = std::any_cast<ID3D12Device*             >(d3d->getDevice());
	m_descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// ★★★★★★★★★★★★★★★
	// 1. 상수 + 텍스처 버퍼 heap 생성
	// Create a descriptor heap for the constant buffers.
	const UINT NUM_CB = 1;						// 셰이더 상수 레지스터 숫자
	const UINT NUM_TX = 1;						// 셰이더 텍스처 텍스처 레지스터
	const UINT NUM_CB_TX = NUM_CB + NUM_TX;
	{
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.NumDescriptors = FRAME_BUFFER_COUNT * NUM_CB  + NUM_TX;	// 프레임당 상수 버퍼 1개 (b0) * 프레임 수 + 텍스처용 SRV(t0) 1개
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		hr = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_cbvHeap));
		if(FAILED(hr))
			return hr;
		m_descGpuHandle = m_cbvHeap->GetGPUDescriptorHandleForHeapStart();
		m_descCpuHandle = m_cbvHeap->GetCPUDescriptorHandleForHeapStart();
	}

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//2. Create a root signature with a single constant buffer slot.
	auto rsoManager = FactorySignature::instance();
	auto rsoItem = rsoManager->FindRes("PLS2D_SPINE0");
	if (!rsoItem)
	{
		ID3D12RootSignature*	rootSignature	{};
		// sampler register 갯수는 상관 없음.
		CD3DX12_STATIC_SAMPLER_DESC staticSampler[] =
		{
			{ 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP},
			{ 1, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP},
			{ 2, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP},
			{ 3, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP},
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
			4,					// sampler register 숫자.
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
		hr = device->CreateRootSignature(0, pSignature->GetBufferPointer(), pSignature->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
		if(FAILED(hr))
			return hr;

		rsoManager->Add("PLS2D_SPINE0", rootSignature);
	}

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 3. pipe line 설정
	rsoItem = rsoManager->FindRes("PLS2D_SPINE0");
	auto psoManager = FactoryPipelineState::instance();
	auto psoItem = psoManager->FindRes("PLS2D_SPINE0");
	if (!psoItem)
	{
		ID3D12PipelineState* pipelineState{};
		auto shader_manager = FactoryShader::instance();
		auto vs = shader_manager->Load("spine_shader_vertex", "asset/shader/spine.hlsl", "vs_5_0", "main_vs");
		auto ps = shader_manager->Load("spine_shader_pixel", "asset/shader/spine.hlsl", "ps_5_0", "main_ps");
		// Create the pipeline state once the shaders are loaded.
		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
			psoDesc.InputLayout = { (D3D12_INPUT_ELEMENT_DESC*)VTX2D_DT::INPUT_LAYOUT_SPLIT.data(), (UINT)VTX2D_DT::INPUT_LAYOUT_SPLIT.size() };
			psoDesc.pRootSignature = rsoItem;
			psoDesc.VS = CD3DX12_SHADER_BYTECODE(vs->r.Get());
			psoDesc.PS = CD3DX12_SHADER_BYTECODE(ps->r.Get());
			psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;	// 우-> 좌 변경 대응.

			psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			// 추가 코드:알파 블렌딩 켜고 src-alpha inv-src-alpha
			psoDesc.BlendState.RenderTarget[0].BlendEnable = TRUE;
			psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
			psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
			psoDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
			psoDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
			psoDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
			psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

			psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
			// 추가 코드: Z-write, Z-test 끄기:
			psoDesc.DepthStencilState.DepthEnable = TRUE;
			psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
			psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
			psoDesc.DepthStencilState.StencilEnable = FALSE;

			psoDesc.SampleMask = UINT_MAX;
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			psoDesc.NumRenderTargets = 1;
			psoDesc.RTVFormats[0] = *std::any_cast<DXGI_FORMAT*>(d3d->getAttrib(ATT_DEVICE_BACKBUFFER_FORAT));
			psoDesc.DSVFormat = *std::any_cast<DXGI_FORMAT*>(d3d->getAttrib(ATT_DEVICE_DEPTH_STENCIL_FORAT));
			psoDesc.SampleDesc.Count = 1;

			hr = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState));
			if (FAILED(hr))
				return hr;

			psoManager->Add("PLS2D_SPINE0", pipelineState);
		}
	}

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 4. 상수 버퍼용 리소스 생성
	{
		CD3DX12_HEAP_PROPERTIES uploadHeapProperties(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC constantBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(FRAME_BUFFER_COUNT * G2::alignTo256(sizeof XMMATRIX ));
		hr = device->CreateCommittedResource(&uploadHeapProperties
												, D3D12_HEAP_FLAG_NONE
												, &constantBufferDesc
												, D3D12_RESOURCE_STATE_GENERIC_READ
												, nullptr, IID_PPV_ARGS(&m_cnstMVP));
		if(FAILED(hr))
			return hr;
	}
	{
		UINT d3dDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		const UINT bufferSize = G2::alignTo256(sizeof XMMATRIX);
		// b0: Wld
		{
			D3D12_GPU_VIRTUAL_ADDRESS cbvGpuAddress = m_cnstMVP->GetGPUVirtualAddress();
			D3D12_CPU_DESCRIPTOR_HANDLE cbvCpuHandle = m_descCpuHandle;
			// offset 0
			for(int n = 0; n < FRAME_BUFFER_COUNT * NUM_CB; ++n)
			{
				D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
				desc.BufferLocation = cbvGpuAddress;
				desc.SizeInBytes = bufferSize;
				device->CreateConstantBufferView(&desc, cbvCpuHandle);
				cbvGpuAddress += bufferSize;
				cbvCpuHandle.ptr += d3dDescriptorSize;
			}
			CD3DX12_RANGE readRange(0, 0);
			hr = m_cnstMVP->Map(0, &readRange, reinterpret_cast<void**>(&m_ptrMVP));
			if(FAILED(hr)) return hr;
		}
	}

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// ★★★★★★★★★★★★★★★
	// 5 텍스처 레지스터 SRV 디스크립터 생성
	// rootSigDesc 초기화 부분과 일치해야함.
	// FRAME_BUFFER_COUNT * NUM_CB 상수 레지스터 다음부터 텍스처 레지스터(셰이더 참고)
	{
		UINT baseSRVIndex = FRAME_BUFFER_COUNT * NUM_CB;
		UINT descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv(m_descCpuHandle, baseSRVIndex, descriptorSize);
		CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv(m_descGpuHandle, baseSRVIndex, descriptorSize);

		// texture viewer 생성
		auto textureNames = m_spineRsc->GetTextureList();
		// AFEW::WARNING!!!!: 하나만 하자.. 귀찮다...
		for (size_t i=0; i<1; ++i)
		{
			const auto& name = textureNames[i];
			auto textureRes = FactoryTexture::instance()->FindRes(name);
			if (textureRes)
			{
				hCpuSrv.Offset((INT)i, descriptorSize);			//
				hGpuSrv.Offset((INT)i, descriptorSize);			//
				m_textureHandle = hGpuSrv;					// CPU, GPU OFFSET을 이동후 Heap pointer 위치를 저장 이 핸들 값이 텍스처 핸들

				D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
				srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				srvDesc.Format = textureRes->GetDesc().Format;
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				srvDesc.Texture2D.MipLevels = 1;
				device->CreateShaderResourceView(textureRes, &srvDesc, hCpuSrv);
			}
		}
	}

	d3d->command(CMD_WAIT_GPU);
	return S_OK;
}

void SpineRender::SetupDrawBuffer()
{
	// 최대 버퍼 길이 찾기
	size_t maxVertexCount = 0;
	size_t maxIndexCount = 0;
	auto drawOrder = m_spineSkeleton->getDrawOrder();
	for (size_t i = 0; i < drawOrder.size(); ++i)
	{
		spine::Slot* slot = drawOrder[i];
		spine::Attachment* attachment = slot->getAttachment();
		if (!attachment)
			continue;
		if (attachment->getRTTI().isExactly(spine::MeshAttachment::rtti))
		{
			auto* mesh = static_cast<spine::MeshAttachment*>(attachment);
			size_t vtxCount = mesh->getWorldVerticesLength() / 2;
			if (vtxCount > maxVertexCount)
				maxVertexCount = vtxCount;

			size_t idxCount = mesh->getTriangles().size();
			if (idxCount > maxIndexCount)
				maxIndexCount = idxCount;
		}
	}

	// buffer 최댓값으로 설정.
	m_maxVtxCount = UINT((maxVertexCount > 8) ? maxVertexCount : 8);
	m_maxIdxCount = UINT((maxIndexCount > 8) ? maxIndexCount : 8);

	if (!m_drawBuf.empty())
		m_drawBuf.clear();

	m_drawBuf.resize(drawOrder.size(), {});

	// 7. vertex, index buffer uploader 생성
	const UINT widthVertex = m_maxVtxCount * sizeof(XMFLOAT2);
	const UINT widthIndex = m_maxIdxCount * sizeof(uint16_t);
	CD3DX12_HEAP_PROPERTIES heapPropsGPU(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_HEAP_PROPERTIES heapPropsUpload(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC   vtxBufDesc = CD3DX12_RESOURCE_DESC::Buffer(widthVertex);
	CD3DX12_RESOURCE_DESC	idxBufDesc = CD3DX12_RESOURCE_DESC::Buffer(widthIndex);

	// setup draw buffer
	auto d3d = IG2GraphicsD3D::instance();
	auto device = std::any_cast<ID3D12Device*>(d3d->getDevice());
	for (size_t i = 0; i < m_drawBuf.size(); ++i)
	{
		m_drawBuf[i].Setup(device, widthVertex, widthIndex, heapPropsGPU, heapPropsUpload, vtxBufDesc, idxBufDesc);
	}
}

