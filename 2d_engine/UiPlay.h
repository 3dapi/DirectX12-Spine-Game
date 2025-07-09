#pragma once
#ifndef _UiPlay_H_
#define _UiPlay_H_

#include "UiBase.h"
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

class UiPlay : public UiBase
{
protected:
	ComPtr<ID3D12DescriptorHeap>	m_srvHeapUI;
	map<string, UI_TEXTURE>			m_uiTex;

public:
	UiPlay();
	virtual ~UiPlay();

	int		Init()			override;
	int		Update(float)	override;
	int		Destroy()		override;
	int		Draw()			override;
};

#endif
