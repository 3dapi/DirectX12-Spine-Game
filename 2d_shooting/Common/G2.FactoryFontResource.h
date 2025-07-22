
#pragma once
#ifndef __G2_FACTORY_FONTRESOURCE_H__
#define __G2_FACTORY_FONTRESOURCE_H__

#include <string>
#include <tuple>
#include <vector>
#include <Windows.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include "G2.Factory.h"

using namespace DirectX;

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
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------

class StringTexture
{
public:
	inline static const UINT TEX_W = 2048;
	inline static const UINT TEX_H = 128;
	inline static const UINT FONT_H = 12;

public:
	static StringTexture* Create(const std::string& fontName, int fontHeight, const std::string& text);

	static std::tuple<ID3D12Resource*, XMUINT2, XMUINT2>	// texture resource, size texture, size source
		CreateStringTexture(const std::string& fontName, int fontHeight, const std::string& text);

	static std::tuple<ID3D12Resource*, XMUINT2, XMUINT2>	// texture resource, size texture, size source
		UpdateStringTexture(ID3D12Resource* tex, const std::string& fontName, int fontHeight, const std::string& text);

public:
	StringTexture();
	~StringTexture();
	int		Init(const std::string& fontName, int fontHeight=FONT_H, const std::string& text={});
	int		Destroy();
	int		Update(const std::string& text, int fontHeight=0, const std::string& fontName="");

	ID3D12Resource*	TextureResource () const { return m_texRsc; }
	XMUINT2			SizeTex			() const { return m_sizeTex;}
	XMUINT2			SizeSrc			() const { return m_sizeSrc;}

protected:
	std::string		m_text			;
	std::string		m_fontName		;
	int				m_fontHeight	{};
	ID3D12Resource* m_texRsc		{};
	XMUINT2			m_sizeTex		{TEX_W, TEX_H};		// d3d texture size 2048, 128 for draw
	XMUINT2			m_sizeSrc		{};					// source size
public:
};

} // namespace G2

#endif // __G2_FACTORY_FONTRESOURCE_H__
