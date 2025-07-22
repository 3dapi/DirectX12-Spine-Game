#pragma once
#ifndef __APPCOMMON_H__
#define __APPCOMMON_H__

#include <any>
#include <array>
#include <map>
#include <memory>
#include <vector>
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dx12/d3dx12.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <wrl/client.h>
#include "AppConst.h"
#include "Common/G2.Constants.h"
#include "common/G2.ConstantsWin.h"
#include "Common/GameTimer.h"

using std::vector;
using std::unique_ptr;
using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace G2;

#endif __APPCOMMON_H__
