
#pragma once
#ifndef __G2_FACTORY_FONTRESOURCE_H__
#define __G2_FACTORY_FONTRESOURCE_H__

#include <string>
#include <tuple>
#include <vector>
#include <Windows.h>
#include <d3d12.h>
#include <wrl/client.h>
#include "G2.Factory.h"

namespace G2 {

struct TD3D_FontResource
{
	std::string			name;	// FontResource name
	std::string			file;	// FontResource file
	~TD3D_FontResource();
};

class FactoryFontResource : public IG2Factory<FactoryFontResource, TD3D_FontResource>
{
public:
	static FactoryFontResource* instance();
	TD3D_FontResource* ResourceLoad(const std::string& name, const std::string& file);
public:
	TD3D_FontResource* ResourceFind(const std::string& name) const override;
	int ResourceUnLoad(const std::string& name) override;
	std::string FindRes(const std::string& name) const;

	static ID3D12Resource*
		CreateStringTexture(const std::string& fontName, int fontHeight, const std::string& text);
};

} // namespace G2

#endif // __G2_FACTORY_FONTRESOURCE_H__
