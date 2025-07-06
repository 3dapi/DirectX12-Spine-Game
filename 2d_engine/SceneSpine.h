﻿
#pragma once
#ifndef _SceneSpine_D3D12_H_
#define _SceneSpine_D3D12_H_

#include <map>
#include <vector>
#include <any>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <wrl/client.h>
#include <spine/spine.h>

#include "Common/G2.Constants.h"
#include "Common/G2.Geometry.h"
#include "common/G2.ConstantsWin.h"
#include "common/G2.Util.h"
#include "AppCommon.h"
#include "AppCommonXTK.h"

using namespace std;
using namespace DirectX;
using Microsoft::WRL::ComPtr;
using namespace G2;

struct DRAW_BUFFER {
	UINT						numVb		{};		// vertex count
	UINT						numIb		{};		// index count
	ComPtr<ID3D12Resource>		rscPosGPU	{};		// position buffer default heap resource
	ComPtr<ID3D12Resource>		rscPosCPU	{};		// position buffer upload heap resource
	D3D12_VERTEX_BUFFER_VIEW	vbvPos		{};		// position buffer view

	ComPtr<ID3D12Resource>		rscDifGPU	{};		// diffuse buffer default heap resource
	ComPtr<ID3D12Resource>		rscDifCPU	{};		// diffuse buffer upload heap resource
	D3D12_VERTEX_BUFFER_VIEW	vbvDif		{};		// diffuse buffer view

	ComPtr<ID3D12Resource>		rscTexGPU	{};		// texture coord buffer default heap resource
	ComPtr<ID3D12Resource>		rscTexCPU	{};		// texture coord buffer upload heap resource
	D3D12_VERTEX_BUFFER_VIEW	vbvTex		{};		// texture buffer view

	ComPtr<ID3D12Resource>		rscIdxGPU	{};		// index buffer default heap resource
	ComPtr<ID3D12Resource>		rscIdxCPU	{};		// index buffer upload heap resource
	D3D12_INDEX_BUFFER_VIEW		ibv			{};		// index buffer view

	~DRAW_BUFFER();
	int Setup(ID3D12Device* d3dDevice, UINT widthVertex, UINT widthIndex
				, const CD3DX12_HEAP_PROPERTIES& heapPropsGPU, const CD3DX12_HEAP_PROPERTIES& heapPropsUpload
				, const CD3DX12_RESOURCE_DESC& vtxBufDesc, const CD3DX12_RESOURCE_DESC& idxBufDesc);
};

class SceneSpine: public spine::TextureLoader,  public G2::IG2Scene
{
protected:
	// Direct3D resources for cube geometry.
	UINT							m_descriptorSize	{};
	ComPtr<ID3D12DescriptorHeap>	m_cbvHeap			{};
	ComPtr<ID3D12RootSignature>		m_rootSignature		{};
	ComPtr<ID3D12PipelineState>		m_pipelineState		{};
	D3D12_GPU_DESCRIPTOR_HANDLE		m_cbvHandle			{};
	UINT							m_maxVtxCount		{};
	UINT							m_maxIdxCount		{};

	vector<DRAW_BUFFER>				m_drawBuf			;
	int								m_drawCount			{};

	ComPtr<ID3D12Resource>			m_spineTextureRsc	{};		// spine texture resource
	D3D12_GPU_DESCRIPTOR_HANDLE		m_spineTextureHandle{};		// spine texture handle

	ComPtr<ID3D12Resource>			m_cnstMVP			{};
	uint8_t*						m_ptrMVP			{};

	// spine instance
	vector<string>				m_spineAnimations	;
	string						m_spineTextureName	;
	spine::Skeleton*			m_spineSkeleton		{};
	spine::AnimationState*		m_spineAniState		{};
	spine::SkeletonData*		m_spineSkeletonData	{};
	spine::Atlas*				m_spineAtlas		{};

public:
	SceneSpine();
	virtual ~SceneSpine();

	// IG2Scene
	int		Type()									override { return EAPP_SCENE::EAPP_SCENE_SPINE; }
	int		Init(const std::any& ={})	override;
	int		Destroy()					override;
	int		Update(const std::any& t)	override;
	int		Render()					override;

protected:
	int		InitSpine(const string& str_atlas, const string& str_skel);
	int		InitForDevice();
	int		UpdateDrawBuffer();
	void*	TextureLoad(const string& fileName);
	void	TextureUnload(void* texture);
	void	load(spine::AtlasPage& page, const spine::String& path) override;
	void	unload(void* texture) override;
};

#endif
