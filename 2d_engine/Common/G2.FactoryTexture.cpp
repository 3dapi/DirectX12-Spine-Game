#include <algorithm>
#include <filesystem>
#include <tuple>
#include <string>
#include "DDSTextureLoader.h"
#include "WICTextureLoader.h"
#include "ResourceUploadBatch.h"
#include "GraphicsMemory.h"
#include "G2.ConstantsWin.h"
#include "G2.FactoryTexture.h"
#include "G2.Util.h"

using namespace std;
namespace G2 {

FactoryTexture* FactoryTexture::instance()
{
	static FactoryTexture inst;
	return &inst;
}

TD3D_TEXTURE* FactoryTexture::ResourceLoad(const std::string& name, const std::string& file)
{
	auto d3d    =  IG2GraphicsD3D::instance();
	auto device = std::any_cast<ID3D12Device*       >(d3d->getDevice());
	auto cmdQue = std::any_cast<ID3D12CommandQueue* >(d3d->getCommandQueue());

	auto itr = this->m_db.find(name);
	if (itr != this->m_db.end())
	{
		return itr->second.get();
	}

	// load
	auto pItem = std::make_unique<TD3D_TEXTURE>();
	pItem->name = name;
	pItem->file = file;

	ComPtr<ID3D12Resource> rs_tx{};
	std::wstring wFile = ansiToWstr(file);

	DirectX::ResourceUploadBatch resourceUpload(device);
	{
		resourceUpload.Begin();

		int hr = S_OK;
		filesystem::path str_path(file);
		auto ext = toLower(str_path.extension().generic_string());
		if(0 == ext.compare(".dds"))
		{
			hr = DirectX::CreateDDSTextureFromFile(device, resourceUpload, wFile.c_str(), rs_tx.GetAddressOf());
		}
		else
		{
			hr = DirectX::CreateWICTextureFromFile(device, resourceUpload, wFile.c_str(), rs_tx.GetAddressOf());
		}
		ThrowIfFailed(hr);
		if(FAILED(hr))
			return {};

		auto uploadOp = resourceUpload.End(cmdQue);
		uploadOp.wait();  // GPU 업로드 완료 대기
		// 뷰는 descriptor heap 생성 후에 만듦.
	}

	pItem->r = std::move(rs_tx);

	//c++17
	auto [it, success] = m_db.insert({ name, std::move(pItem) });
	auto ret = it->second.get();
	return ret;
	//c++14
	//auto it = m_db.insert({ name, std::move(pItem) });
	//auto ret = it.first;
	//bool success = it.second;
	//return ret->second.get(); // TD3D_TEXTURE*
}


TD3D_TEXTURE* FactoryTexture::ResourceFind(const std::string& name) const
{
	auto itr = this->m_db.find(name);
	if (itr != this->m_db.end())
	{
		return itr->second.get();
	}
	static TD3D_TEXTURE dummy{ "<none>", "<none>" };
	return &dummy;
}
int FactoryTexture::ResourceUnLoad(const std::string& name)
{
	auto itr = this->m_db.find(name);
	if (itr != this->m_db.end())
	{
		m_db.erase(itr);
		return S_OK;
	}
	return E_FAIL;
}
int FactoryTexture::ResourceUnLoadAll()
{
	m_db.clear();
	return S_OK;
}

ID3D12Resource* FactoryTexture::FindRes(const std::string& name) const
{
	auto itr = this->m_db.find(name);
	if (itr != this->m_db.end())
	{
		return itr->second.get()->r.Get();
	}
	return {};
}

} // namespace G2
