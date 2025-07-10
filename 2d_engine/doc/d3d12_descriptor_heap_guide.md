# 📘 Direct3D 12 디스크립터 힙과 Offset 사용 정리

---

## 📌 1. 디스크립터 힙이란?

디스크립터 힙(Descriptor Heap)은 GPU가 리소스를 참조할 수 있도록 구성된 **디스크립터들의 배열**입니다.

```cpp
D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
heapDesc.NumDescriptors = 250; // 0~249 슬롯 사용 가능
heapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
heapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_cbvHeapFont));
```

> 🎯 힙 크기를 `NumDescriptors`만큼 잡고, **중복되지 않는 인덱스**로 디스크립터를 할당해 쓰는 것이 핵심.

---

## 📌 2. 디스크립터 핸들 가져오기

```cpp
auto hCpu = m_cbvHeapFont->GetCPUDescriptorHandleForHeapStart(); // 0번 슬롯
auto hGpu = m_cbvHeapFont->GetGPUDescriptorHandleForHeapStart(); // GPU용
```

---

## 📌 3. Offset을 통한 인덱스 계산

```cpp
UINT descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

CD3DX12_CPU_DESCRIPTOR_HANDLE fontHandle(hCpu);
fontHandle.Offset(0, descriptorSize); // 0번 슬롯

CD3DX12_CPU_DESCRIPTOR_HANDLE spineHandle(hCpu);
spineHandle.Offset(1, descriptorSize); // 1번 슬롯

CD3DX12_GPU_DESCRIPTOR_HANDLE spineGpuHandle(hGpu);
spineGpuHandle.Offset(1, descriptorSize); // GPU에서도 동일한 인덱스
```

또는 Offset 없이 직접 포인터 연산으로:

```cpp
D3D12_CPU_DESCRIPTOR_HANDLE spineHandle = hCpu;
spineHandle.ptr += descriptorSize;

D3D12_GPU_DESCRIPTOR_HANDLE spineGpuHandle = hGpu;
spineGpuHandle.ptr += descriptorSize;
```

---

## 📌 4. 리소스 뷰 만들기 (예: 텍스처 SRV)

```cpp
device->CreateShaderResourceView(texture, &srvDesc, fontHandle); // 0번 슬롯에 SRV 생성
device->CreateShaderResourceView(spineTex, &srvDesc, spineHandle); // 1번 슬롯
```

---

## 📌 5. GPU 바인딩 시에도 동일한 인덱스를 사용

```cpp
cmdList->SetDescriptorHeaps(1, &m_cbvHeapFont);
cmdList->SetGraphicsRootDescriptorTable(0, spineGpuHandle);
```

---

## ✅ 실전 요약

| 개념 | 설명 |
|------|------|
| 디스크립터 힙 크기 | `NumDescriptors` 개수만큼 슬롯 사용 가능 (ex. 250 → 0~249) |
| 핸들 계산 | `Offset(index, descriptorSize)` 또는 `ptr += descriptorSize * index` |
| 중복 인덱스 사용 | ❌ 기존 디스크립터를 덮어씀 |
| 사용 방식 | 배열처럼 인덱스를 순서대로 지정해서 쓰면 됨 |
| 디스크립터 종류 | CBV / SRV / UAV / Sampler 등 모두 동일 원칙 적용 |

---

## ✅ 예제 전체 흐름 요약

```cpp
// 1. 디스크립터 힙 생성
heapDesc.NumDescriptors = 250;
device->CreateDescriptorHeap(...);

// 2. 핸들 준비
auto hCpu = m_cbvHeapFont->GetCPUDescriptorHandleForHeapStart();
auto hGpu = m_cbvHeapFont->GetGPUDescriptorHandleForHeapStart();
UINT descriptorSize = device->GetDescriptorHandleIncrementSize(...);

// 3. Offset 또는 ptr += 연산
auto fontHandleCpu = CD3DX12_CPU_DESCRIPTOR_HANDLE(hCpu, 0, descriptorSize);
auto spineHandleCpu = CD3DX12_CPU_DESCRIPTOR_HANDLE(hCpu, 1, descriptorSize);
auto spineHandleGpu = CD3DX12_GPU_DESCRIPTOR_HANDLE(hGpu, 1, descriptorSize);

// 또는
D3D12_CPU_DESCRIPTOR_HANDLE spineHandle = hCpu;
spineHandle.ptr += descriptorSize;

// 4. SRV 등 생성
device->CreateShaderResourceView(..., fontHandleCpu);
device->CreateShaderResourceView(..., spineHandleCpu);

// 5. 바인딩
cmdList->SetDescriptorHeaps(1, &m_cbvHeapFont);
cmdList->SetGraphicsRootDescriptorTable(0, spineHandleGpu);
```

---

## 🔚 결론

> ✔️ **디스크립터 힙은 배열이다.**  
> ✔️ **인덱스만 겹치지 않게 Offset 또는 += 연산으로 접근하면 된다.**  
> ✔️ **포인터처럼 생각하지 말고 배열처럼 생각하라.**
---

## 🎁 보너스: 루프에서 인덱스별 디스크립터 핸들 생성

여러 디스크립터를 루프 안에서 연속적으로 생성할 때는 다음과 같이 사용할 수 있습니다:

```cpp
for (UINT i = 0; i < 5; ++i) {
    // 방법 1: Offset 사용
    CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle(hCpu);
    cpuHandle.Offset(i, descriptorSize);

    CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(hGpu);
    gpuHandle.Offset(i, descriptorSize);

    // 방법 2: 직접 포인터 연산
    D3D12_CPU_DESCRIPTOR_HANDLE cpuManual = hCpu;
    cpuManual.ptr += i * descriptorSize;

    D3D12_GPU_DESCRIPTOR_HANDLE gpuManual = hGpu;
    gpuManual.ptr += i * descriptorSize;

    // 사용 예
    device->CreateShaderResourceView(textures[i], &srvDesc, cpuHandle);
}
```

> ✔️ 루프에서는 `Offset(i, descriptorSize)` 또는 `ptr += i * descriptorSize` 두 가지 방식 모두 사용 가능  
> ✔️ 인덱스만 겹치지 않게 잘 관리하면 안전하고 효율적입니다
