
#include <tuple>
#include <string>
#include "G2.ConstantsWin.h"
#include "G2.Util.h"
#include "G2.Geometry.h"

using namespace Microsoft::WRL;
using namespace DirectX;

namespace G2 {

//--------------------------------------------------------------------------------------------------------------------------------------------------------------

ResBuf::~ResBuf()
{
	Destroy();
}

int ResBuf::Setup(ID3D12Device* device, UINT countVtx, UINT stride, const wstring& debugName)
{
	std::wstring strDbugName = debugName + L"_rscVtx";
	int hr = S_OK;

	Destroy();

	this->count = countVtx;
	this->stride = stride;
	auto bufSize = this->Size();
	CD3DX12_HEAP_PROPERTIES heapPropsGPU(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_HEAP_PROPERTIES heapPropsUpload(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC   bufDesc = CD3DX12_RESOURCE_DESC::Buffer(bufSize);
	// GPU position buffer
	hr = device->CreateCommittedResource(&heapPropsGPU, D3D12_HEAP_FLAG_NONE, &bufDesc, D3D12_RESOURCE_STATE_COMMON
										, nullptr, IID_PPV_ARGS(&rscGPU));
	if(FAILED(hr))
		return hr;

	// CPU upload position buffer
	hr = device->CreateCommittedResource(&heapPropsUpload, D3D12_HEAP_FLAG_NONE, &bufDesc, D3D12_RESOURCE_STATE_GENERIC_READ
										, nullptr, IID_PPV_ARGS(&rscCPU));
	if(FAILED(hr))
		return hr;

	rscCPU->Map(0, nullptr, (void**)&ptr);

	return S_OK;
}

void ResBuf::Destroy()
{
	if(rscGPU && ptr) {
		rscCPU->Unmap(0, nullptr);
	}
	SAFE_RELEASE(rscGPU);
	SAFE_RELEASE(rscCPU);
}

void ResBuf::UploadToGpu(ID3D12GraphicsCommandList* cmdList)
{
	auto bufSize = this->Size();
	CD3DX12_RESOURCE_BARRIER toCopy = CD3DX12_RESOURCE_BARRIER::Transition(rscGPU, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
	cmdList->ResourceBarrier(1, &toCopy);

	cmdList->CopyBufferRegion(rscGPU, 0, rscCPU, 0, bufSize);
	CD3DX12_RESOURCE_BARRIER barPos = CD3DX12_RESOURCE_BARRIER::Transition(rscGPU, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	cmdList->ResourceBarrier(1, &barPos);
}

void ResBuf::Copy(void* src, UINT64 size)
{
	if(!ptr)
	{
		rscCPU->Map(0, nullptr, (void**)&ptr);
	}
	avx2_memcpy(ptr, src, size);
}

void ResBufVtx::SetupBufferView()
{
	this->bv = {
		rscGPU->GetGPUVirtualAddress()
		, this->Size()
		, stride
	};
}

D3D12_VERTEX_BUFFER_VIEW* ResBufVtx::GetVertexBufferView()
{
	return &this->bv;
}

void ResBufIdx::SetupBufferView()
{
	this->bv = {
		rscGPU->GetGPUVirtualAddress()
		, this->Size()
		, DXGI_FORMAT_R16_UINT
	};
}

D3D12_INDEX_BUFFER_VIEW* ResBufIdx::GetIndexBufferView()
{
	return &this->bv;
}

} // namespace G2
