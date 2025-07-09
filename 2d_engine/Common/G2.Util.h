
#pragma once
#ifndef _G2_Util_H_
#define _G2_Util_H_

#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <random>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>
#include <Windows.h>
#ifdef max
  #undef max
#endif
#include <d3dcommon.h>
#include <d3d12.h>
#include <wrl.h>

using namespace Microsoft::WRL;

namespace G2 {


template<typename T>
inline void SAFE_DELETE(T*& p)
{
	if (p)
	{
		delete p;
		p = {};
	}
}
template<typename T>
inline void SAFE_DELETE_ARR(T*& p)
{
	if (p)
	{
		delete[] p;
		p = {};
	}
}

template<typename T>
inline void SAFE_RELEASE(T*& p)
{
	if (p)
	{
		p->Release();
		p = {};
	}
}
// NOTE: for new T*[count] raw pointer array
template<typename T>
inline void SAFE_RELEASE_ARRAY(T*& p, size_t count)
{
	if (p)
	{
		for (size_t i = 0; i < count; ++i)
		{
			if (p[i])
				p[i]->Release();
		}
		delete[] p;
		p = {};
	}
}
// NOTE: for std::vector 
template<typename T>
inline void SAFE_RELEASE_VECTOR(std::vector<T*>& vec) {
	for (auto& p : vec) {
		if (p) {
			p->Release();
			p = {};
		}
	}
	vec.clear();
}

template<typename T>
inline T randomRange(T begin, T end)
{
	static std::mt19937 rnd(std::random_device{}());
	if (begin > end)
		std::swap(begin, end);
	if (begin == end)
		return begin;
	if constexpr (std::is_integral_v<T>)
	{
		std::uniform_int_distribution<T> dist(begin, end); // [begin, end]
		return dist(rnd);
	}
	else if constexpr (std::is_floating_point_v<T>)
	{
		std::uniform_real_distribution<T> dist(begin, std::nextafter(end, std::numeric_limits<T>::max())); // [begin, end]
		return dist(rnd);
	}
	else
	{
		static_assert(std::is_arithmetic_v<T>, "randomRange() only supports arithmetic types.");
		return T{};
	}
}

inline uint32_t rgba2uint32(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) { return (r) | (g<<8) | (b<<16) | (a<<24); }
inline uint32_t bgra2uint32(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) { return (b) | (g<<8) | (r<<16) | (a<<24); }

inline bool chckPointInRect(int pointX, int pointY, int x0, int y0, int x1, int y1) {
	if (x0 > x1)
		std::swap(x0, x1);
	if (y0 > y1)
		std::swap(y0, y1);

	if (x0 > pointX || pointX > x1
		||
		y0 > pointY || pointY > y1
		)
	{
		return false;
	}
	return true;
}


inline std::string toLower(const std::string& str_t) {
	std::string ret = str_t;
	std::transform(ret.begin(), ret.end(), ret.begin(),
				   [](unsigned char c) { return std::tolower(c); });
	return ret;
}

inline std::wstring utf8ToWstr(const std::string& str)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
	if (1 >= len)
		return L"";
	std::wstring wstr(len - 1, 0);
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], len);
	return wstr;
}

inline std::wstring ansiToWstr(const std::string& str)
{
	int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, nullptr, 0);
	if (1 >= len)
		return  L"";
	std::wstring wstr(len - 1, 0);
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, &wstr[0], len);
	return wstr;
}

inline void debugToOutputWindow(const char* format, ...)
{
	va_list args;
	va_start(args, format);

	va_list args_copy;
	va_copy(args_copy, args);
	int length = std::vsnprintf(nullptr, 0, format, args_copy);
	va_end(args_copy);

	if (length > 0) {
		++length;		// for null char
		std::string buffer(length, 0);
		std::vsnprintf(&buffer[0], length, format, args);
		OutputDebugStringA(buffer.c_str());
	}

	va_end(args);
}

void avx2_memcpy(void* dst, const void* src, size_t size);	// avx2 메모리 복사: memcpy 대신
void avx2_memset32(void* dst, int32_t val, size_t count);	// 32bit 값 채우기: memset 대신.

inline UINT alignTo256(UINT byteSize)
{
	// Constant buffers must be a multiple of the minimum hardware
	// allocation size (usually 256 bytes).  So round up to nearest
	// multiple of 256.  We do this by adding 255 and then masking off
	// the lower 2 bytes which store all bits < 256.
	// Example: Suppose byteSize = 300.
	// (300 + 255) & ~255
	// 555 & ~255
	// 0x022B & ~0x00ff
	// 0x022B & 0xff00
	// 0x0200
	// 512
	return (byteSize + 255) & ~255;
}

ID3DBlob*           DXCompileShaderFromFile(const std::string& fileName, const std::string& shaderModel, const std::string& entryPoint, const void* macros = {});
ID3D12Resource*     DXCreateTextureFromFile(const std::string& szFileName);

std::pair<std::vector<uint8_t>, int>	// buffer, result
readFileBinary(const std::string& fileName);
ComPtr<ID3DBlob> readFileToBlob(const std::string& fileName);

class DXException
{
public:
	DXException() = default;
	DXException(HRESULT hr, const std::string& functionName, const std::string& filename, int lineNumber);
	std::wstring ToString()const;

	HRESULT     errorCode = S_OK;
	std::string functionName;
	std::string fileName;
	int         lineNumber = -1;
};

#ifndef ThrowIfFailed
#define ThrowIfFailed(x) {                              \
    if(FAILED(x)) {                                     \
		throw DXException(x, __func__ , __FILE__, __LINE__);  \
	}                                                   \
}
#endif


} // namespace G2

#endif // _G2_Util_H_
