﻿
#pragma once
#ifndef __G2_FACTORY_TEXTURE_H__
#define __G2_FACTORY_TEXTURE_H__

#include <string>
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include "G2.Factory.h"

using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;

namespace G2 {

struct TD3D_TEXTURE
{
	string				name;	// texture name
	string				file;	// texture file
	XMUINT2				size;	// texture size
	ID3D12Resource*		r{};	// resource
	~TD3D_TEXTURE() {
		if(r) {
			r->Release();
			r = {};
		}
	}
};

class FactoryTexture : public IG2Factory<FactoryTexture, TD3D_TEXTURE>
{
public:
	static FactoryTexture* instance();
public:
	TD3D_TEXTURE* ResourceLoad(const std::string& name, const std::string& file);
	TD3D_TEXTURE* ResourceFind(const std::string& name) const override;
	int ResourceUnLoad(const std::string& name)         override;
	int ResourceUnLoadAll()                             override;
	// find ID3D12Resource*
	ID3D12Resource* FindRes(const std::string& name) const;

	static XMUINT2 GetTextureSize(ID3D12Resource* texture);
};

} // namespace G2

#endif // __G2_FACTORY_TEXTURE_H__
