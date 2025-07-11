#pragma once
#ifndef _UiEnd_H_
#define _UiEnd_H_

#include <memory>
#include "UiBase.h"
#include <wrl/client.h>
#include "AppCommonXTK.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

class UiEnd : public UiBase
{
protected:
	ComPtr<ID3D12DescriptorHeap>	m_srvHeapUI{};
	map<string, UI_TEXTURE>			m_uiTex;
	unique_ptr<SpriteFont>			m_font{};
	float							m_blend{ 0.0F };
	float							m_blendDir{ 1.0F };

public:
	UiEnd();
	virtual ~UiEnd();

	int		Init()			override;
	int		Update(float)	override;
	int		Destroy()		override;
	int		Draw()			override;
	int		DrawFront();
};

#endif
