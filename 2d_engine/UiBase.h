#pragma once
#ifndef _UiBase_H_
#define _UiBase_H_

#include <map>
#include <string>
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl/client.h>

using namespace std;
using namespace DirectX;

struct UI_TEXTURE
{
	ID3D12Resource*					res;
	XMUINT2							size;
	D3D12_GPU_DESCRIPTOR_HANDLE		hGpu;
};
typedef map<string, UI_TEXTURE>		UI_MAP;

class UiBase
{
protected:
public:
	virtual ~UiBase() = default;
	virtual int		Init()                  = 0;
	virtual int		Update(float deltaTime) = 0;
	virtual int		Destroy()               = 0;
	virtual int		Draw()                  = 0;
	virtual int		DrawFront()             = 0;
};

#endif
