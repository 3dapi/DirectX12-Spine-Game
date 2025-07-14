
#pragma once
#ifndef __G2_FACTORY_SPINE_H__
#define __G2_FACTORY_SPINE_H__

#include <string>
#include <vector>
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dx12/d3dx12.h>
#include <wrl/client.h>
#include <spine/spine.h>
#include "G2.Factory.h"

using namespace std;
using namespace DirectX;
using Microsoft::WRL::ComPtr;

namespace G2 {

struct TD3D_SPINE
{
	std::string				name		;	// spine name
	std::string				fileAtlas	;	// atlas file
	std::string				fileSkel	;	// skeleton file
	spine::Atlas*			atlas		{};
	spine::SkeletonData*	skelData	{};
	~TD3D_SPINE() {
		if (atlas) {
			// crash 방지.
			atlas->_textureLoader = nullptr;
			delete atlas;
			atlas = {};
		}
		if (skelData) {
			delete skelData;
			skelData = {};
		}
	}
	vector<string> GetTextureList();
};

class FactorySpine : public IG2Factory<FactorySpine, TD3D_SPINE>, public spine::TextureLoader
{
public:
	static FactorySpine* instance();
public:
	TD3D_SPINE* ResourceLoad(const std::string& name, const std::string& fileAtlas, const std::string& fileSkel);
	TD3D_SPINE* ResourceFind(const std::string& name) const override;
	int ResourceUnLoad(const std::string& name)         override;
	int ResourceUnLoadAll()                             override;
	// find ID3D12Resource*
	TD3D_SPINE* FindRes(const std::string& name) const;

protected:
	void	load(spine::AtlasPage& page, const spine::String& path) override;
	void	unload(void* texture) override;

public:
	static vector<string> getAnimationList(spine::Skeleton*);
};


struct SPINE_DRAW_BUFFER {
	UINT						countVtx	{};		// vertex count
	UINT						strideVtx	{};
	UINT						countIdx	{};		// index count

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

	~SPINE_DRAW_BUFFER();
	int Setup(ID3D12Device* d3dDevice, const CD3DX12_RESOURCE_DESC& vtxBufDesc, const CD3DX12_RESOURCE_DESC& idxBufDesc);
	int ResizeVtx(ID3D12Device* d3dDevice, const CD3DX12_RESOURCE_DESC& bufDesc);
	int ResizeIdx(ID3D12Device* d3dDevice, const CD3DX12_RESOURCE_DESC& bufDesc);
};

} // namespace G2

#endif // __G2_FACTORY_SPINE_H__

