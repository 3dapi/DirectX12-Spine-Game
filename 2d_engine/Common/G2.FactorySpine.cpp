#include <algorithm>
#include <filesystem>
#include <mutex>
#include <tuple>
#include <string>

#include <Windows.h>
#include <d3d12.h>
#include <wrl/client.h>
#include <spine/spine.h>

#include "DDSTextureLoader.h"
#include "WICTextureLoader.h"
#include "ResourceUploadBatch.h"
#include "GraphicsMemory.h"
#include "G2.ConstantsWin.h"
#include "G2.FactoryTexture.h"
#include "G2.FactorySpine.h"
#include "G2.Util.h"

using namespace std;
namespace G2 {

vector<string> TD3D_SPINE::GetTextureList()
{
	vector<string> ret;
	auto atlasPage = atlas->getPages();
	for (size_t i = 0; i < atlasPage.size(); ++i)
		ret.push_back(std::string(atlasPage[i]->texturePath.buffer()));
	return ret;
}

FactorySpine* FactorySpine::instance()
{
	static FactorySpine inst;
	return &inst;
}

TD3D_SPINE* FactorySpine::ResourceLoad(const std::string& name, const std::string& fileAtlas, const std::string& fileSkel)
{
	auto itr = this->m_db.find(name);
	if (itr != this->m_db.end())
	{
		return itr->second.get();
	}

	// get skeleton file extention
	filesystem::path strSkel(fileSkel);
	auto ext = toLower(strSkel.extension().generic_string());

	// load
	spine::Atlas*        spineAtlas = new spine::Atlas(fileAtlas.c_str(), this);
	spine::SkeletonData* spineSkel = {};

	if (!spineAtlas)
		return {};

	if (0 == ext.compare(".json"))
	{
		spine::SkeletonJson skl(spineAtlas);
		spineSkel = skl.readSkeletonDataFile(fileSkel.c_str());
	}
	else
	{
		spine::SkeletonBinary skl(spineAtlas);
		spineSkel = skl.readSkeletonDataFile(fileSkel.c_str());
	}

	// failed
	if (!spineAtlas)
	{
		delete spineAtlas;
		return {};
	}

	auto pItem = std::make_unique<TD3D_SPINE>();
	pItem->name = name;
	pItem->fileAtlas = fileAtlas;
	pItem->fileSkel  = fileSkel;
	pItem->atlas     = spineAtlas;
	pItem->skelData  = spineSkel;

	auto [it, success] = m_db.insert({ name, std::move(pItem) });
	auto ret = it->second.get();

	return ret;
}


TD3D_SPINE* FactorySpine::ResourceFind(const std::string& name) const
{
	auto itr = this->m_db.find(name);
	if (itr != this->m_db.end())
	{
		return itr->second.get();
	}
	static TD3D_SPINE dummy{ "<none>", "<none>" };
	return &dummy;
}
int FactorySpine::ResourceUnLoad(const std::string& name)
{
	auto itr = this->m_db.find(name);
	if (itr != this->m_db.end())
	{
		m_db.erase(itr);
		return S_OK;
	}
	return E_FAIL;
}
int FactorySpine::ResourceUnLoadAll()
{
	m_db.clear();
	return S_OK;
}

TD3D_SPINE* FactorySpine::FindRes(const std::string& name) const
{
	auto itr = this->m_db.find(name);
	if (itr != this->m_db.end())
	{
		return itr->second.get();
	}
	return {};
}

void FactorySpine::load(spine::AtlasPage& page, const spine::String& path) {
	auto fileName = path.buffer();
	TD3D_TEXTURE* texRes = FactoryTexture::instance()->Load(fileName, fileName);
	page.texture = texRes;

}

void FactorySpine::unload(void* texture) {
	TD3D_TEXTURE* texRes = reinterpret_cast<TD3D_TEXTURE*>(texture);
	int c;
	c = 0;
}

SPINE_DRAW_BUFFER::~SPINE_DRAW_BUFFER()
{
	rscPosGPU	.Reset();
	rscPosCPU	.Reset();
	vbvPos		= {};

	rscDifGPU	.Reset();
	rscDifCPU	.Reset();
	vbvDif		= {};

	rscTexGPU	.Reset();
	rscTexCPU	.Reset();
	vbvTex		= {};

	rscIdxGPU	.Reset();
	rscIdxCPU	.Reset();
	ibv			= {};
}

int SPINE_DRAW_BUFFER::Setup(ID3D12Device* device, UINT widthVertex, UINT widthIndex
				, const CD3DX12_HEAP_PROPERTIES& heapPropsGPU, const CD3DX12_HEAP_PROPERTIES& heapPropsUpload
				, const CD3DX12_RESOURCE_DESC& vtxBufDesc, const CD3DX12_RESOURCE_DESC& idxBufDesc)
{
	int hr = S_OK;
	// GPU position buffer
	hr = device->CreateCommittedResource(&heapPropsGPU, D3D12_HEAP_FLAG_NONE, &vtxBufDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&rscPosGPU));
	if(FAILED(hr))
		return hr;
	// CPU upload position buffer
	hr = device->CreateCommittedResource(&heapPropsUpload, D3D12_HEAP_FLAG_NONE, &vtxBufDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&rscPosCPU));
	if(FAILED(hr))
		return hr;
	// GPU diffuse buffer
	hr = device->CreateCommittedResource(&heapPropsGPU, D3D12_HEAP_FLAG_NONE, &vtxBufDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&rscDifGPU));
	if(FAILED(hr))
		return hr;
	// CPU upload diffuse buffer
	hr = device->CreateCommittedResource(&heapPropsUpload, D3D12_HEAP_FLAG_NONE, &vtxBufDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&rscDifCPU));
	if(FAILED(hr))
		return hr;
	// GPU texture coord buffer
	hr = device->CreateCommittedResource(&heapPropsGPU, D3D12_HEAP_FLAG_NONE, &vtxBufDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&rscTexGPU));
	if(FAILED(hr))
		return hr;
	// CPU upload texture coord buffer
	hr = device->CreateCommittedResource(&heapPropsUpload, D3D12_HEAP_FLAG_NONE, &vtxBufDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&rscTexCPU));
	if(FAILED(hr))
		return hr;
	// GPU index buffer
	hr = device->CreateCommittedResource(&heapPropsGPU, D3D12_HEAP_FLAG_NONE, &idxBufDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&rscIdxGPU));
	if(FAILED(hr))
		return hr;
	// CPU upload index buffer
	hr = device->CreateCommittedResource(&heapPropsUpload, D3D12_HEAP_FLAG_NONE, &idxBufDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&rscIdxCPU));
	if(FAILED(hr))
		return hr;

	return S_OK;
}

} // namespace G2


namespace spine {
	spine::SpineExtension* getDefaultExtension()
	{
		static bool ____dummy____ = []() { spine::Bone::setYDown(false); return true; }();
		static spine::SpineExtension* _default_spineExtension = new spine::DefaultSpineExtension;
		return _default_spineExtension;
	}
}

