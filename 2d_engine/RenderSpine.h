
#pragma once
#ifndef _RenderSpine_H_
#define _RenderSpine_H_

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
#include "common/G2.FactorySpine.h"
#include "common/G2.Util.h"
#include "AppCommon.h"
#include "AppCommonXTK.h"

using namespace std;
using namespace DirectX;
using Microsoft::WRL::ComPtr;
using namespace G2;


struct SPINE_ATTRIB
{
	string			spine_name	;
	string			atlasPath	;
	string			skelPath	;
	float			aniBegin	{ 0.0F };	// animation begin time
	float			aniSpeed	{ 1.0F };	// animation speed
	float			vecDir		{ 1.0F };	// left : -1.0F right: 1.0F
	float			vecScale	{ 1.0F };	// transform offset position
	XMFLOAT2		vecOffset	{ };		// transform offset position

	string			aniName		{};
	string			skinName	{};
	vector<string>	detachSlot	{};
};

class RenderSpine: public G2::IG2RenderObject
{
protected:
	// spine resource
	G2::TD3D_SPINE*				m_spineRsc			{};
	// spine rendering instance
	spine::Skeleton*			m_spineSkeleton		{};
	spine::AnimationState*		m_spineAniState		{};

	vector<string>				m_spineAnimation;
	SPINE_ATTRIB				m_attrib			{};

protected:
	// Direct3D resources for cube geometry.
	UINT							m_descriptorSize	{};
	ComPtr<ID3D12DescriptorHeap>	m_cbvHeap			{};
	D3D12_GPU_DESCRIPTOR_HANDLE		m_descGpuHandle		{};
	D3D12_CPU_DESCRIPTOR_HANDLE		m_descCpuHandle		{};
	UINT							m_maxVtxCount		{};
	UINT							m_maxIdxCount		{};

	vector<SPINE_DRAW_BUFFER>		m_drawBuf			;
	int								m_drawCount			{};
	D3D12_GPU_DESCRIPTOR_HANDLE		m_textureHandle		{};		// spine texture handle
	ComPtr<ID3D12Resource>			m_cnstMVP			{};
	uint8_t*						m_ptrMVP			{};

public:
	RenderSpine();
	virtual ~RenderSpine();

	// IG2Scene
	int		Type()						override { return EAPP_SCENE::EAPP_SCENE_SPINE; }
	int		Init(const std::any& ={})	override;
	int		Destroy()					override;
	int		Update(const std::any& t)	override;
	int		Render()					override;

	void	Look(float direction);

protected:
	int		InitSpine();
	int		InitD3DResource();
	int		UpdateDrawBuffer();
	void	SetupDrawBuffer();
};

SPINE_ATTRIB* FindSpineAttribute(const std::string& spineName);

#endif
