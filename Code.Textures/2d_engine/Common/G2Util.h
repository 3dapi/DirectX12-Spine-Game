﻿// Interface for the D3DUtil class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#ifndef _G2Util_H_
#define _G2Util_H_

#include <string>
#include <tuple>
#include <vector>
#include <Windows.h>
#include <d3dcommon.h>
#include <d3d12.h>
#include <wrl.h>

using namespace Microsoft::WRL;

namespace G2 {
template<typename T>
inline void SAFE_DELETE(T*& p)
{
	if (p)
	{
		delete p;
		p = {};
	}
}
template<typename T>
inline void SAFE_DELETE_ARR(T*& p)
{
	if (p)
	{
		delete[] p;
		p = {};
	}
}

template<typename T>
inline void SAFE_RELEASE(T*& p)
{
	if (p)
	{
		p->Release();
		p = {};
	}
}
// NOTE: for new T*[count] raw pointer array
template<typename T>
inline void SAFE_RELEASE_ARRAY(T*& p, size_t count)
{
	if (p)
	{
		for (size_t i = 0; i < count; ++i)
		{
			if (p[i])
				p[i]->Release();
		}
		delete[] p;
		p = {};
	}
}
// NOTE: for std::vector 
template<typename T>
inline void SAFE_RELEASE_VECTOR(std::vector<T*>& vec) {
	for (auto& p : vec) {
		if (p) {
			p->Release();
			p = {};
		}
	}
	vec.clear();
}

std::wstring stringMultiByte2WString(const std::string& str);
HRESULT		DXCompileShaderFromFile(const std::string& szFileName, const std::string& szEntryPoint, const std::string& szShaderModel, ID3DBlob** ppBlobOut = nullptr);
HRESULT		DXCreateDDSTextureFromFile(ID3D12Device* device , ID3D12GraphicsCommandList* cmdList, const std::string& szFileName , ComPtr<ID3D12Resource>& texture, ComPtr<ID3D12Resource>& uploadHeap);
} // namespace G2
#endif
