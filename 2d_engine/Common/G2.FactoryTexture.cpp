#include <algorithm>
#include <filesystem>
#include <tuple>
#include <string>
#include "DirectXHelpers.h"
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
	auto itr = this->m_db.find(name);
	if (itr != this->m_db.end())
	{
		return itr->second.get();
	}

	auto tex = DXCreateTextureFromFile(file);
	if (!tex)
		return {};

	// load
	ComPtr<ID3D12Resource> rs_tx = tex;
	auto pItem = std::make_unique<TD3D_TEXTURE>();
	pItem->name = name;
	pItem->file = file;
	pItem->size = DirectX::GetTextureSize(tex);
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
