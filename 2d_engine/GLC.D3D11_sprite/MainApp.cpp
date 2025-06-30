﻿#include <any>
#include <Windows.h>
#include <d3d11.h>
#include <DirectxColors.h>
#include "G2Base.h"
#include "MainApp.h"
#include "G2Util.h"
using namespace DirectX;

// for lighting
struct SimpleVertex
{
	DirectX::XMFLOAT3 p;
	DirectX::XMFLOAT2 t;
};
// constant buffer for the vertex shader
struct ConstBufLight
{
	DirectX::XMFLOAT4 vLightDir[2];
	DirectX::XMFLOAT4 vLightColor[2];
	DirectX::XMFLOAT4 vOutputColor;
};

MainApp::MainApp()
{
}

MainApp::~MainApp()
{
	Destroy();
}

int MainApp::Init()
{
	auto d3dDevice  = std::any_cast<ID3D11Device*>(IG2GraphicsD3D::getInstance()->GetDevice());
	auto d3dContext = std::any_cast<ID3D11DeviceContext*>(IG2GraphicsD3D::getInstance()->GetContext());


	// create vertex shader
	// 1.1 Compile the vertex shader
	ID3DBlob* pBlob{};
	HRESULT hr = G2::DXCompileShaderFromFile("assets/simple.fxh", "main_vtx", "vs_4_0", &pBlob);
	if (FAILED(hr))
	{
		MessageBox({}, "The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", "Error", MB_OK);
		return hr;
	}
	// 1.2 Create the vertex shader
	hr = d3dDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), {}, &m_shaderVtx);
	if (FAILED(hr))
		return hr;

	// 1.3 create vertexLayout
	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT   , 0, 0 + sizeof(DirectX::XMFLOAT3), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);
	hr = d3dDevice->CreateInputLayout(layout, numElements, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &m_vtxLayout);
	G2::SAFE_RELEASE(pBlob);
	if (FAILED(hr))
		return hr;


	// 2.1 Compile the pixel shader
	hr = G2::DXCompileShaderFromFile("assets/simple.fxh", "main_pxl", "ps_4_0", &pBlob);
	if (FAILED(hr))
	{
		MessageBox({}, "Failed ComplThe FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", "Error", MB_OK);
		return hr;
	}
	// 2.2 Create the pixel shader
	hr = d3dDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), {}, &m_shaderPxl);
	G2::SAFE_RELEASE(pBlob);
	if (FAILED(hr))
		return hr;

	// 3. Create vertex buffer
	SimpleVertex vertices[] =
	{
		{ { -1.0f,  1.0f, -1.0f },  { 1.0f, 0.0f } },
		{ {  1.0f,  1.0f, -1.0f },  { 0.0f, 0.0f } },
		{ {  1.0f,  1.0f,  1.0f },  { 0.0f, 1.0f } },
		{ { -1.0f,  1.0f,  1.0f },  { 1.0f, 1.0f } },
   
		{ { -1.0f, -1.0f, -1.0f },  { 0.0f, 0.0f } },
		{ {  1.0f, -1.0f, -1.0f },  { 1.0f, 0.0f } },
		{ {  1.0f, -1.0f,  1.0f },  { 1.0f, 1.0f } },
		{ { -1.0f, -1.0f,  1.0f },  { 0.0f, 1.0f } },
   
		{ { -1.0f, -1.0f,  1.0f },  { 0.0f, 1.0f } },
		{ { -1.0f, -1.0f, -1.0f },  { 1.0f, 1.0f } },
		{ { -1.0f,  1.0f, -1.0f },  { 1.0f, 0.0f } },
		{ { -1.0f,  1.0f,  1.0f },  { 0.0f, 0.0f } },
   
		{ {  1.0f, -1.0f,  1.0f },  { 1.0f, 1.0f } },
		{ {  1.0f, -1.0f, -1.0f },  { 0.0f, 1.0f } },
		{ {  1.0f,  1.0f, -1.0f },  { 0.0f, 0.0f } },
		{ {  1.0f,  1.0f,  1.0f },  { 1.0f, 0.0f } },
   
		{ { -1.0f, -1.0f, -1.0f },  { 0.0f, 1.0f } },
		{ {  1.0f, -1.0f, -1.0f },  { 1.0f, 1.0f } },
		{ {  1.0f,  1.0f, -1.0f },  { 1.0f, 0.0f } },
		{ { -1.0f,  1.0f, -1.0f },  { 0.0f, 0.0f } },
   
		{ { -1.0f, -1.0f,  1.0f },  { 1.0f, 1.0f } },
		{ {  1.0f, -1.0f,  1.0f },  { 0.0f, 1.0f } },
		{ {  1.0f,  1.0f,  1.0f },  { 0.0f, 0.0f } },
		{ { -1.0f,  1.0f,  1.0f },  { 1.0f, 0.0f } },
    };
	m_bufVtxCount = sizeof(vertices) / sizeof(vertices[0]);
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * m_bufVtxCount;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData = {};
	InitData.pSysMem = vertices;
	hr = d3dDevice->CreateBuffer(&bd, &InitData, &m_bufVtx);
	if (FAILED(hr))
		return hr;

	// 4. Create Index buffer
	WORD indices[] =
	{
		 3, 1, 0,  2, 1, 3,
		 6, 4, 5,  7, 4, 6,
		11, 9, 8, 10, 9,11,
		14,12,13, 15,12,14,
		19,17,16, 18,17,19,
		22,20,21, 23,20,22
	};
	m_bufIdxCount = sizeof(indices) / sizeof(indices[0]);

	bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * m_bufIdxCount;        // 36 vertices needed for 12 triangles in a triangle list
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = indices;
	hr = d3dDevice->CreateBuffer(&bd, &InitData, &m_bufIdx);

	// 5. Create the constant buffer
	// world
	bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(m_mtWorld);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = d3dDevice->CreateBuffer(&bd, {}, &m_cnstWorld);
	if (FAILED(hr))
		return hr;

	// view
	bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(m_mtView);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = d3dDevice->CreateBuffer(&bd, {}, &m_cnstView);
	if (FAILED(hr))
		return hr;

	// projection matrtix
	bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(m_mtProj);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = d3dDevice->CreateBuffer(&bd, {}, &m_cnstProj);
	if (FAILED(hr))
		return hr;

	// 6. setup the world, view, projection matrix
	// View, Projection Matrix
	// Initialize the view matrix
	XMVECTOR Eye = XMVectorSet( 0.0f, 4.0f, -10.0f, 0.0f );
	XMVECTOR At = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
	XMVECTOR Up = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
	m_mtView = XMMatrixLookAtLH(Eye, At, Up);

	// Initialize the projection matrix
	auto screeSize = std::any_cast<::SIZE*>(IG2GraphicsD3D::getInstance()->GetAttrib(ATTRIB_CMD::ATTRIB_SCREEN_SIZE));
	m_mtProj = XMMatrixPerspectiveFovLH(XM_PIDIV4, screeSize->cx / (FLOAT)screeSize->cy, 1.0f, 5000.0f);

	// 7. Initialize the world matrix
	m_mtWorld = XMMatrixIdentity();


	// 8. create texture sampler state
	// Load the Texture
	std::tie(hr, m_srvTexture, std::ignore) = G2::DXCreateDDSTextureFromFile("assets/seafloor.dds");
	if (FAILED(hr))
		return hr;

	// Create the sample state
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = d3dDevice->CreateSamplerState(&sampDesc, &m_sampLinear);
	if (FAILED(hr))
		return hr;

	// Diffuse color for the mesh
	bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(m_difMesh);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = d3dDevice->CreateBuffer(&bd, {}, &m_cnstDiff);
	if (FAILED(hr))
		return hr;

	return S_OK;
}

int MainApp::Destroy()
{
	G2::SAFE_RELEASE(m_shaderVtx);
	G2::SAFE_RELEASE(m_shaderPxl);
	G2::SAFE_RELEASE(m_vtxLayout);
	G2::SAFE_RELEASE(m_bufVtx	);
	G2::SAFE_RELEASE(m_bufIdx	);
	G2::SAFE_RELEASE(m_cnstWorld);
	G2::SAFE_RELEASE(m_cnstView	);
	G2::SAFE_RELEASE(m_cnstProj	);

	G2::SAFE_RELEASE(m_srvTexture);
	G2::SAFE_RELEASE(m_sampLinear);
	G2::SAFE_RELEASE(m_cnstDiff	);
	return S_OK;
}

int MainApp::Update()
{
	// Update our time
	static float t = 0.0f;

	static ULONGLONG timeStart = 0;
	ULONGLONG timeCur = GetTickCount64();
	if (timeStart == 0)
		timeStart = timeCur;
	t = (timeCur - timeStart) / 1000.0f;

	// Rotate cube around the origin
	//m_mtWorld = XMMatrixRotationY(t);

	m_difMesh.x = (sinf(t * 1.0f) + 1.0f) * 0.5f;
	m_difMesh.y = (cosf(t * 3.0f) + 1.0f) * 0.5f;
	m_difMesh.z = (sinf(t * 5.0f) + 1.0f) * 0.5f;

	return S_OK;
}

int MainApp::Render()
{
	auto d3dContext = std::any_cast<ID3D11DeviceContext*>(IG2GraphicsD3D::getInstance()->GetContext());

	// 1. Update constant value
	d3dContext->UpdateSubresource(m_cnstWorld, 0, {}, &m_mtWorld, 0, 0);
	d3dContext->UpdateSubresource(m_cnstView , 0, {}, &m_mtView , 0, 0);
	d3dContext->UpdateSubresource(m_cnstProj , 0, {}, &m_mtProj , 0, 0);
	d3dContext->UpdateSubresource(m_cnstDiff , 0, {}, &m_difMesh , 0, 0);

	// 2. set the constant buffer
	d3dContext->VSSetConstantBuffers(0, 1, &m_cnstWorld);
	d3dContext->VSSetConstantBuffers(1, 1, &m_cnstView);
	d3dContext->VSSetConstantBuffers(2, 1, &m_cnstProj);
	d3dContext->PSSetConstantBuffers(3, 1, &m_cnstDiff);

	// 3. set vertex shader
	d3dContext->VSSetShader(m_shaderVtx, {}, 0);

	// 4. set the input layout
	d3dContext->IASetInputLayout(m_vtxLayout);

	// 5. set the pixel shader
	d3dContext->PSSetShader(m_shaderPxl, {}, 0);

	// 6. set the vertex buffer
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	d3dContext->IASetVertexBuffers(0, 1, &m_bufVtx, &stride, &offset);

	// 7. set the index buffer
	d3dContext->IASetIndexBuffer(m_bufIdx, DXGI_FORMAT_R16_UINT, 0);

	// 8. set the texture
	d3dContext->PSSetShaderResources(0, 1, &m_srvTexture);

	// 9. set the sampler state
	d3dContext->PSSetSamplers(0, 1, &m_sampLinear);

	// 10. primitive topology
	d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 11.draw cube
	d3dContext->DrawIndexed(m_bufIdxCount, 0, 0);

	return S_OK;
}

