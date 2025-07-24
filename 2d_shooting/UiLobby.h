#pragma once
#ifndef _UiLobby_H_
#define _UiLobby_H_

#include "UiDrawable.h"
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

class UiLobby : public UiDrawable
{
protected:
	ComPtr<ID3D12DescriptorHeap>	m_srvHeapUI{};
	map<string, UI_TEXTURE>			m_uiTex;
	float							m_blend{ 0.0F };
	float							m_blendDir{ 1.0F };

public:
	UiLobby();
	virtual ~UiLobby();

	int		Init()			override;
	int		Update(float)	override;
	int		Destroy()		override;
	int		Draw()			override;
	int		DrawFront()		override;
};

#endif
