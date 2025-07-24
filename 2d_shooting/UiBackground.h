#pragma once
#ifndef _UiBackground_H_
#define _UiBackground_H_

#include "AppCommon.h"
#include "AppCommonXTK.h"
#include "UiDrawable.h"

class UiBackground : public UiDrawable
{
protected:
	ComPtr<ID3D12DescriptorHeap>	m_srvHeapUI{};
	map<string, UI_TEXTURE>			m_uiTex;
	float							m_scrollBegin	;
	float							m_scrollY		{};
	float							m_scrollSpeed	{};
public:
	UiBackground();
	virtual ~UiBackground();

	int		Init()			override;
	int		Update(float)	override;
	int		Destroy()		override;
	int		Draw()			override;
	int		DrawFront()		override;
};

#endif
