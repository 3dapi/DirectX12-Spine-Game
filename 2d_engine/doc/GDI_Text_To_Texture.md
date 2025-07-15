# GDI 문자열을 D3D12 텍스처로 업로드하는 흐름 정리

## 개요

- **목표**: GDI로 문자열을 렌더링하여 `std::vector<uint32_t>` 버퍼를 만든 후, Direct3D 12 텍스처에 업로드
- **포맷**: `DXGI_FORMAT_B8G8R8A8_UNORM`
- **방식**: 텍스트 → GDI → CPU 버퍼 → GPU 텍스처 리소스
- **주의사항**:
  - `RowPitch` 패딩 필요
  - `UpdateSubresources` 사용 시 패딩 처리 필수

---

## 주요 단계

### 1. 텍스트 버퍼 생성
```cpp
auto [buf, width, height] = ::G2::CreateGdiStringBuffer(fontName, fontHeight, text);
```

### 2. 텍스처 리소스 생성
```cpp
auto bufDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_B8G8R8A8_UNORM, (UINT64)width, (UINT)height, 1, 1);
device->CreateCommittedResource(...);
```

---

### 3. GetCopyableFootprints 사용

```cpp
D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint = {};
UINT numRows = 0;
UINT64 rowSizeInBytes = 0, totalBytes = 0;
device->GetCopyableFootprints(&bufDesc, 0, 1, 0, &footprint, &numRows, &rowSizeInBytes, &totalBytes);
```

**설명**:
- GPU에 텍스처를 업로드할 때 `RowPitch`는 패딩된 row 크기
- `totalBytes`는 업로드 버퍼 크기
- `footprint.Footprint.RowPitch`를 기준으로 버퍼를 생성해야 글자가 잘리지 않음

---

### 4. 버퍼 복사 및 RowPitch 정렬
```cpp
std::vector<uint8_t> paddedBuf(rowPitch * height, 0);
for(UINT y = 0; y < height; ++y)
{
    avx2_memcpy(&paddedBuf[y * rowPitch], &buf[y * width], width * sizeof(uint32_t));
}
```

---

### 5. UpdateSubresources 사용

```cpp
D3D12_SUBRESOURCE_DATA sub = {};
sub.pData = paddedBuf.data();
sub.RowPitch = rowPitch;
sub.SlicePitch = rowPitch * height;

UpdateSubresources<1>(cmdList, retTexture, rscCPU.Get(), 0, 0, 1, &sub);
```

**설명**:
- `UpdateSubresources`는 버퍼 복사 + 상태 전이 바리어 처리를 헬퍼로 처리
- `CopyTextureRegion` 등을 직접 쓸 필요 없이 서브리소스 한 번에 업데이트 가능
- 내부적으로 `ResourceBarrier`, `Copy*`, `Unmap` 등을 처리함

---

## 6. 후처리: 상태 전이 및 명령 제출
```cpp
auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(retTexture, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
cmdList->ResourceBarrier(1, &barrier);

d3d->command(CMD_COMMAND_END);
```

---

## 결론

- `GetCopyableFootprints`: 업로드 버퍼 크기 및 정렬된 row pitch 확보
- `UpdateSubresources`: 헬퍼 함수로 전체 복사 및 전이를 간단하게 처리
- 텍스트 렌더링 → 정렬된 복사 → 리소스 전이 흐름을 잘 지켜야 이미지가 깨지지 않음