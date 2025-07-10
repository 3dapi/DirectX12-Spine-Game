
#pragma once
#ifndef _SceneSample2D_H_
#define _SceneSample2D_H_

#include <map>
#include <vector>
#include <any>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <wrl/client.h>

#include "Common/G2.Constants.h"
#include "Common/G2.Geometry.h"
#include "common/G2.ConstantsWin.h"
#include "common/G2.Util.h"
#include "AppCommon.h"
#include "AppCommonXTK.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;
using namespace G2;

class SceneSample2D: public G2::IG2Scene
{
protected:
	// Direct3D resources for cube geometry.
	ComPtr<ID3D12DescriptorHeap>	m_cbvHeap			{};
	ComPtr<ID3D12RootSignature>		m_rootSignature		{};
	ComPtr<ID3D12PipelineState>		m_pipelineState		{};
	D3D12_VERTEX_BUFFER_VIEW		m_viewVtx			{};
	D3D12_INDEX_BUFFER_VIEW			m_viewIdx			{};
	UINT							m_numVtx			{};
	UINT							m_numIdx			{};
	D3D12_GPU_DESCRIPTOR_HANDLE		m_cbvHandle			{};

	ComPtr<ID3D12Resource>			m_rscVtxGPU			{};			// vertex buffer default heap resource
	ComPtr<ID3D12Resource>			m_rscVtxCPU			{};			// vertex buffer upload heap resource
	ComPtr<ID3D12Resource>			m_rscIdxGPU			{};			// index buffer default heap resource
	ComPtr<ID3D12Resource>			m_rscIdxCPU			{};			// index buffer upload heap resource

	ComPtr<ID3D12Resource>			m_spineTextureRsc	{};		// assets/res_checker.png
	D3D12_GPU_DESCRIPTOR_HANDLE		m_spineTexture		{};		// checker SRV GPU 핸들

	ComPtr<ID3D12Resource>			m_cnstMVP			{};
	uint8_t*						m_ptrMVP			{};

public:
	SceneSample2D();
	virtual ~SceneSample2D();

	int		Type()						override { return (int)EAPP_SCENE::EAPP_SCENE_SPINE; }
	int		Init(const std::any& ={})	override;
	int		Destroy()					override;
	int		Update(const std::any& t)	override;
	int		Render()					override;

protected:
	int		InitResource();
};

#endif
