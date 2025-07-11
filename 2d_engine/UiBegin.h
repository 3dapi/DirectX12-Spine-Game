#pragma once
#ifndef _UiBegin_H_
#define _UiBegin_H_

#include "UiBase.h"
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

class UiBegin : public UiBase
{
protected:
	ComPtr<ID3D12DescriptorHeap>	m_srvHeapUI{};
	map<string, UI_TEXTURE>			m_uiTex;
	unique_ptr<SpriteFont>			m_font{};
	float							m_blend{ 0.0F };
	float							m_blendDir{ 1.0F };
public:
	UiBegin();
	virtual ~UiBegin();

	int		Init()			override;
	int		Update(float)	override;
	int		Destroy()		override;
	int		Draw()			override;
	int		DrawFront()		;
};

#endif
