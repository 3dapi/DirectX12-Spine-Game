
#pragma once
#ifndef __G2_GEOMETRY_H__
#define __G2_GEOMETRY_H__

#include <array>
#include <string>
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12/d3dx12.h>

using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;

namespace G2 {

// vertex
// signature:
// 	VTX: 3d vertex
// 	VTX2D: 2d vertex
//  P:position (생략), N: normal, D: diffuse T: texture I:seperate texture index

//--------------------------------------------------------------------------------------------------------------------------------------------------------------
// vertex format

struct VTX2D_DT			// P:position, D: diffuse T: texture coord
{
	XMFLOAT2 p;
	uint32_t d;
	XMFLOAT2 t;

	VTX2D_DT() = default;
	VTX2D_DT(const XMFLOAT2& _p, const uint32_t _d, const XMFLOAT2& _t) : p{ _p }, d{ _d }, t{ _t } {}

	inline static const std::array<const D3D12_INPUT_ELEMENT_DESC, 3> INPUT_LAYOUT
	{{
		{ "POSITION" , 0, DXGI_FORMAT_R32G32_FLOAT   , 0, 0                                                     , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR"    , 0, DXGI_FORMAT_R8G8B8A8_UNORM , 0, 0+sizeof(XMFLOAT2)                                    , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD" , 0, DXGI_FORMAT_R32G32_FLOAT   , 0, 0+sizeof(XMFLOAT2)+sizeof(uint32_t)                   , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	}};
	inline static const std::array<const D3D12_INPUT_ELEMENT_DESC, 3> INPUT_LAYOUT_SPLIT
	{{
		{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT    , 0, 0 , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"COLOR"   , 0, DXGI_FORMAT_R8G8B8A8_UNORM  , 1, 0 , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT    , 2, 0 , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	}};
};

struct VTX2D_DTI		// P:position, D: diffuse T: texture I:seperate texture index
{
	XMFLOAT2 p;
	uint32_t d;
	XMFLOAT2 t;
	uint32_t i;

	VTX2D_DTI() = default;
	VTX2D_DTI(const XMFLOAT2& _p, const uint32_t _d, const XMFLOAT2& _t, const uint32_t _i) : p{ _p }, d{ _d }, t{ _t }, i{ _i } {}

	inline static const std::array<const D3D12_INPUT_ELEMENT_DESC, 4> INPUT_LAYOUT
	{{
		{ "POSITION" , 0, DXGI_FORMAT_R32G32_FLOAT   , 0, 0                                                     , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR"    , 0, DXGI_FORMAT_R8G8B8A8_UNORM , 0, 0+sizeof(XMFLOAT2)                                    , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD" , 0, DXGI_FORMAT_R32G32_FLOAT   , 0, 0+sizeof(XMFLOAT2)+sizeof(uint32_t)                   , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD" , 0, DXGI_FORMAT_R32_UINT       , 0, 0+sizeof(XMFLOAT2)+sizeof(uint32_t)+sizeof(XMFLOAT2)  , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	}};
};


struct VTX_N		// position, + nomal
{
	XMFLOAT3 p;
	XMFLOAT3 n;

	VTX_N() = default;
	VTX_N(const XMFLOAT3& _p, const XMFLOAT3& _n) : p{ _p }, n{ _n } {}

	inline static const std::array<const D3D12_INPUT_ELEMENT_DESC, 2> INPUT_LAYOUT
	{{
		{ "POSITION" , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0                                                     , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL"   , 0, DXGI_FORMAT_R8G8B8A8_UNORM , 0, 0+sizeof(XMFLOAT3)                                    , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	}};
};


struct VTX_ND		// position + nomal + diffuse
{
	XMFLOAT3 p;
	XMFLOAT3 n;
	uint32_t d;

	VTX_ND() = default;
	VTX_ND(const XMFLOAT3& _p, const XMFLOAT3& _n, const uint32_t _d) : p{ _p }, n{ _n }, d{ _d } {}

	inline static const std::array<const D3D12_INPUT_ELEMENT_DESC, 3> INPUT_LAYOUT
	{{
		{ "POSITION" , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0                                                     , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL"   , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0+sizeof(XMFLOAT3)                                    , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR"    , 0, DXGI_FORMAT_R8G8B8A8_UNORM , 0, 0+sizeof(XMFLOAT3)+sizeof(XMFLOAT3)                   , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	}};
};

struct VTX_NT		// position + normal + texture
{
	XMFLOAT3 p;
	XMFLOAT3 n;
	XMFLOAT2 t;

	VTX_NT() = default;
	VTX_NT(const XMFLOAT3& _p, const XMFLOAT3& _n, const XMFLOAT2& _t) : p{ _p }, n{ _n }, t{ _t } {}

	inline static const std::array<const D3D12_INPUT_ELEMENT_DESC, 3> INPUT_LAYOUT
	{{
		{ "POSITION" , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0                                                     , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL"   , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0+sizeof(XMFLOAT3)                                    , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD" , 0, DXGI_FORMAT_R32G32_FLOAT   , 0, 0+sizeof(XMFLOAT3)+sizeof(XMFLOAT3)                   , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	}};
};

struct VTX_NDT		// position + normal + diffuse + texture
{
	XMFLOAT3 p;
	XMFLOAT3 n;
	uint32_t d;
	XMFLOAT2 t;

	VTX_NDT() = default;
	VTX_NDT(const XMFLOAT3& _p, const XMFLOAT3& _n, const uint32_t _d, const XMFLOAT2& _t) : p{ _p }, n{ _n }, d{ _d }, t{ _t } {}

	inline static const std::array<const D3D12_INPUT_ELEMENT_DESC, 4> INPUT_LAYOUT
	{{
		{ "POSITION" , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0                                                     , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL"   , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0+sizeof(XMFLOAT3)                                    , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR"    , 0, DXGI_FORMAT_R8G8B8A8_UNORM , 0, 0+sizeof(XMFLOAT3)+sizeof(XMFLOAT3)                   , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD" , 0, DXGI_FORMAT_R32G32_FLOAT   , 0, 0+sizeof(XMFLOAT3)+sizeof(XMFLOAT3)+sizeof(uint32_t)  , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	}};
};

struct VTX_NGT		// position + normal + tangent + texture
{
	XMFLOAT3 p;
	XMFLOAT3 n;
	XMFLOAT3 g;	// tangent
	XMFLOAT2 t;

	VTX_NGT() = default;
	VTX_NGT(const XMFLOAT3& _p, const XMFLOAT3& _n, const XMFLOAT3& _g, const XMFLOAT2& _t) : p{ _p }, n{ _n }, g{ _g }, t{ _t } {}
	inline static const std::array<const D3D12_INPUT_ELEMENT_DESC, 4> INPUT_LAYOUT
	{{
		{ "POSITION" , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0                                                     , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL"   , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0+sizeof(XMFLOAT3)                                    , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT"  , 0, DXGI_FORMAT_R8G8B8A8_UNORM , 0, 0+sizeof(XMFLOAT3)+sizeof(XMFLOAT3)                   , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD" , 0, DXGI_FORMAT_R32G32_FLOAT   , 0, 0+sizeof(XMFLOAT3)+sizeof(XMFLOAT3)+sizeof(XMFLOAT3)  , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	}};
};

struct VTX_POINT		// point sprite: position + size
{
	XMFLOAT3 p;
	float    s;

	VTX_POINT() = default;
	VTX_POINT(const XMFLOAT3& _p, float _s) : p{ _p }, s{ _s } {}

	inline static const std::array<const D3D12_INPUT_ELEMENT_DESC, 2> INPUT_LAYOUT
	{{
		{ "POSITION" , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0                                                     , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "SIZE"     , 0, DXGI_FORMAT_R32G32_FLOAT   , 0, 0+sizeof(XMFLOAT3)                                    , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	}};
};


//--------------------------------------------------------------------------------------------------------------------------------------------------------------
// resource vertex buffer

struct ResBuf
{
	UINT				count		{};
	UINT				stride		{};
	ID3D12Resource*		rscGPU		{};		// buffer default heap resource
	ID3D12Resource*		rscCPU		{};		// buffer upload heap resource
	void*				ptr			{};		// mapping pointer

	~ResBuf();
	int		Setup(ID3D12Device* device, UINT count, UINT stride, const wstring& debugName= L"ResBuf");
	void	Destroy();
	void	UploadToGpu(ID3D12GraphicsCommandList* cmdList);
	void	Copy(void* src, UINT64 size);
	UINT	Size() const { return count * stride; }
	void*	Ptr() { return ptr; }
};
struct ResBufVtx : public ResBuf
{
	D3D12_VERTEX_BUFFER_VIEW	bv;
	void						SetupBufferView();
	D3D12_VERTEX_BUFFER_VIEW*	GetVertexBufferView();
};
struct ResBufIdx: public ResBuf
{
	D3D12_INDEX_BUFFER_VIEW		bv;
	D3D12_PRIMITIVE_TOPOLOGY	primitive {D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST};
	void						SetupBufferView();
	D3D12_INDEX_BUFFER_VIEW*	GetIndexBufferView();
};

} // namespace G2

#endif // __G2_GEOMETRY_H__
