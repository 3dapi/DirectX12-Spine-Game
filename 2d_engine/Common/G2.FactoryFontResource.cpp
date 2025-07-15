#include <functional>
#include <dxgi1_6.h>
#include <d3dx12/d3dx12.h>
#include "G2.ConstantsWin.h"
#include "G2.Util.h"
#include "G2.FactoryFontResource.h"

using std::string;
namespace G2 {

FactoryFontResource* FactoryFontResource::instance()
{
	static FactoryFontResource inst;
	return &inst;
}

TD3D_FontResource* FactoryFontResource::ResourceLoad(const string& name, const string& file)
{
	auto itr = this->m_db.find(name);
	if (itr != this->m_db.end())
	{
		return itr->second.get();
	}
	if(0 == AddFontResourceExA(file.c_str(), FR_PRIVATE, nullptr))
		return {};

	auto pItem = std::make_unique<TD3D_FontResource>();
	pItem->name = name;
	pItem->file = file;
	auto [it, success] = m_db.insert({ name, std::move(pItem) });
	auto ret = it->second.get();
	return ret;
}
TD3D_FontResource* FactoryFontResource::ResourceFind(const string& name) const
{
	auto itr = this->m_db.find(name);
	if (itr != this->m_db.end())
	{
		return itr->second.get();
	}
	static TD3D_FontResource dummy{ "<none>", "<none>" };
	return &dummy;
}
int FactoryFontResource::ResourceUnLoad(const string& name)
{
	auto itr = this->m_db.find(name);
	if (itr != this->m_db.end())
	{
		m_db.erase(itr);
		return S_OK;
	}
	return E_FAIL;
}

std::string FactoryFontResource::FindRes(const std::string& name) const
{
	auto itr = this->m_db.find(name);
	if (itr != this->m_db.end())
	{
		return itr->second->name;
	}
	return {};
}

static std::tuple<std::vector<uint32_t>, size_t, size_t>
CreateGdiStringBuffer(const std::string& fontName, int fontHeight, const std::string& text)
{
	auto name = FactoryFontResource::instance()->FindRes(fontName);
	if(name.empty())
		return {};

	SIZE bmpSize{};
	HDC hDC = GetDC(nullptr);
	if(!hDC)
		return {};

	int   dpi     = GetDeviceCaps(hDC, LOGPIXELSY);
	int   nHeight = -MulDiv(fontHeight, dpi, 72);
	HFONT hFont  = CreateFontA(nHeight, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
							  OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
							  DEFAULT_PITCH | FF_DONTCARE, name.c_str());
	if(!hFont)
	{
		ReleaseDC(nullptr, hDC);
		return {};
	}

	auto oldFont = (HFONT)SelectObject(hDC, hFont);
	GetTextExtentPoint32A(hDC, text.c_str(), (int)text.length(), &bmpSize);
	SelectObject(hDC, oldFont);
	ReleaseDC(nullptr, hDC);

	// 4의 배수로 맞춤
	const int width  = (bmpSize.cx +4)/4 * 4;
	const int height = (bmpSize.cy +4)/4 * 4;

	std::vector<uint32_t> pxlBitmap(size_t(width * height), 0);

	BITMAPINFO bmi = {};
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = -height; // top-down
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	void* pBits = nullptr;
	HDC hdcMem = CreateCompatibleDC(nullptr);
	HBITMAP hBitmap = CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS, &pBits, nullptr, 0);
	SelectObject(hdcMem, hBitmap);
	memset(pBits, 0x00, width * height * sizeof(uint32_t)); // BGRA → 검정 배경

	oldFont = (HFONT)SelectObject(hdcMem, hFont);
	SetBkMode(hdcMem, TRANSPARENT);
	SetTextColor(hdcMem, RGB(255, 255, 255));
	//draw text
	TextOutA(hdcMem, 0, 0, text.c_str(), (int)text.size());
	SelectObject(hdcMem, oldFont);

	//copy to buffer
	memcpy(&pxlBitmap[0], pBits, width * height * sizeof(uint32_t));

	// release bitmap dc, font
	DeleteObject(hBitmap);
	DeleteDC(hdcMem);
	DeleteObject(hFont);

	// setup alpha
	uint32_t* pixels = &pxlBitmap[0];
	size_t pixelLen = pxlBitmap.size();
	for(size_t i=0; i < pixelLen; ++i)
	{
		auto& p = pixels[i];
		uint32_t b = (p >> 0) & 0xFF;
		uint32_t g = (p >> 8) & 0xFF;
		uint32_t r = (p >>16) & 0xFF;
		uint32_t a = (b + g + r) /3;
		p         |= ((a<<24) & 0xFF000000);
	}

	// c++14일 경우 명시적으로 move 호출
	// c++17은 NRVO (Named Return Value Optimization) 자동으로 함.
	return std::make_tuple(std::move(pxlBitmap), width, height);
}

ID3D12Resource* FactoryFontResource::CreateStringTexture(const std::string& fontName, int fontHeight, const std::string& text)
{
	ID3D12Resource* retTexture{};
	HRESULT hr = S_OK;
	auto d3d        = IG2GraphicsD3D::instance();
	auto device     = std::any_cast<ID3D12Device*             >(d3d->getDevice());
	auto cmdList    = std::any_cast<ID3D12GraphicsCommandList*>(d3d->getCommandList());

	auto[buf, width, height] = ::G2::CreateGdiStringBuffer(fontName, fontHeight, text);

	// D3D12 리소스 생성
	ComPtr<ID3D12Resource> rscCPU{};
	auto bufDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_B8G8R8A8_UNORM, (UINT64)width, (UINT)height, 1, 1);
	CD3DX12_HEAP_PROPERTIES heapPropsGPU(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_HEAP_PROPERTIES heapPropsUpload(D3D12_HEAP_TYPE_UPLOAD);

	hr = device->CreateCommittedResource(&heapPropsGPU, D3D12_HEAP_FLAG_NONE, &bufDesc, D3D12_RESOURCE_STATE_COPY_DEST,
									nullptr, IID_PPV_ARGS(&retTexture));
	if(FAILED(hr))
		return {};

	D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint = {};
	UINT numRows = 0;
	UINT64 rowSizeInBytes = 0, totalBytes = 0;
	device->GetCopyableFootprints(&bufDesc, 0, 1, 0, &footprint, &numRows, &rowSizeInBytes, &totalBytes);

	size_t rowPitch = footprint.Footprint.RowPitch;
	size_t paddedSize = rowPitch * height;

	std::vector<uint8_t> paddedBuf(paddedSize, 0);

	// 각 row 복사
	for(UINT y = 0; y < height; ++y)
	{
		avx2_memcpy(&paddedBuf[y * rowPitch], &buf[y * width], width * sizeof(uint32_t)); // 4바이트
	}

	// subresource
	D3D12_SUBRESOURCE_DATA sub = {};
	sub.pData = paddedBuf.data();
	sub.RowPitch = rowPitch;
	sub.SlicePitch = rowPitch * height;

	auto bufSize = CD3DX12_RESOURCE_DESC::Buffer(totalBytes);
	device->CreateCommittedResource(&heapPropsUpload, D3D12_HEAP_FLAG_NONE, &bufSize, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&rscCPU));

	hr = d3d->command(CMD_COMMAND_BEGIN);
	if(FAILED(hr))
	{
		SAFE_RELEASE(retTexture);
		return {};
	}
	UpdateSubresources<1>(cmdList, retTexture, rscCPU.Get(), 0, 0, 1, &sub);
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(retTexture, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	cmdList->ResourceBarrier(1, &barrier);

	d3d->command(CMD_COMMAND_END);

	return retTexture;
}


TD3D_FontResource::~TD3D_FontResource()
{
	if(!file.empty())
	{
		RemoveFontResourceExA(file.c_str(), FR_PRIVATE, nullptr);
		file.clear();
		name.clear();
	}
}

} // namespace G2
