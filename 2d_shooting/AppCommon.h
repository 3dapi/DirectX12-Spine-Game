#pragma once
#ifndef __APPCOMMON_H__
#define __APPCOMMON_H__

#include <intrin.h>

#include <algorithm>
#include <any>
#include <array>
#include <filesystem>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <utility>
#include <vector>
#include <math.h>
#include <Windows.h>
#include <wrl.h>
#include <pix.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dx12/d3dx12.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <wrl/client.h>

#include "Common/G2.Constants.h"
#include "common/G2.ConstantsWin.h"
#include "Common/GameTimer.h"
#include "AppConst.h"

using std::map;
using std::vector;
using std::unique_ptr;
using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace G2;

#endif __APPCOMMON_H__
