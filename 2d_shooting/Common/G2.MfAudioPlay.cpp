#define INITGUID			// 이걸 꼭 추가
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mfobjects.h>
#include <mfplay.h>

#include <tuple>
#include <vector>
#include <xaudio2.h>
#include <wrl/client.h>
#include "G2.Util.h"

using Microsoft::WRL::ComPtr;
using namespace G2;
using namespace std;

#define SAFE_RELEASE(x) if (x) { x->Release(); x = nullptr; }

