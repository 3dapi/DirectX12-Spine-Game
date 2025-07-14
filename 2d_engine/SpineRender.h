
#pragma once
#ifndef _SpineRender_H_
#define _SpineRender_H_

#include <functional>
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

class SpineAnimationListener;

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

class SpineRender : public G2::IG2Object
{
	friend class SpineAnimationListener;
protected:
	// spine resource
	G2::TD3D_SPINE*				m_spineRsc			{};
	// spine rendering instance
	spine::Skeleton*			m_spineSkeleton		{};
	spine::AnimationStateData*	m_spineAniStateData	{};
	spine::AnimationState*		m_spineAniState		{};
	SpineAnimationListener*		m_spineListener		{};
	vector<string>				m_spineAnimation	;
	SPINE_ATTRIB				m_attrib			{};

protected:
	// Direct3D resources for cube geometry.
	UINT							m_descriptorSize	{};
	ID3D12DescriptorHeap*			m_cbvHeap			{};
	D3D12_GPU_DESCRIPTOR_HANDLE		m_descGpuHandle		{};
	D3D12_CPU_DESCRIPTOR_HANDLE		m_descCpuHandle		{};

	map<int, SPINE_DRAW_BUFFER*>	m_drawBuf			;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_textureHandle		{};		// spine texture handle
	ID3D12Resource*					m_cnstMVP			{};
	uint8_t*						m_ptrMVP			{};

protected:
	XMFLOAT2	m_pos		{ 0.0F, 0.0F};
	float		m_dir		{ 1.0F };
	float		m_scale		{ 1.0F };
	XMFLOAT4	m_color		{ 1.0F, 1.0F, 1.0F, 1.0F };		// x->r, y->g, z->b, w->a
	string		m_ani		= "idle";

public:
	SpineRender();
	virtual ~SpineRender();

	// IG2Scene
	int		Type()						override { return (int)EAPP_SCENE::EAPP_SCENE_SPINE; }
	int		Init(const std::any& ={})	override;
	int		Destroy()					override;
	int		Update(const std::any& t)	override;
	int		Render()					override;

	void		Position		(const XMFLOAT2&);
	XMFLOAT2	Position		() const ;
	void		Direction		(float);
	float		Direction		() const;
	void		Scale			(float);
	float		Scale			() const;
	void		Color			(const XMFLOAT4&);		// x->r, y->g, z->b, w->a
	XMFLOAT4	Color			() const;				// x->r, y->g, z->b, w->a
	void		Animation		(const string& aniName);
	string		Animation		() const;

protected:
	int		InitSpine();
	int		InitD3DResource();
	int		UpdateDrawBuffer();
	void	SetupDrawBuffer();

	void	AnimationEvent(spine::AnimationState* state, spine::EventType type, spine::TrackEntry* entry, spine::Event* event);
};

class SpineAnimationListener : public spine::AnimationStateListenerObject
{
public:
	SpineRender* m_thzz{};
public:
	void callback(spine::AnimationState* state, spine::EventType type, spine::TrackEntry* entry, spine::Event* event) override;
};

#endif
