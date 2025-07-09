## Spine D3D12 Debugging & Fix Log

### 문제 요약
- Spine 애니메이션에서 무기 부착 문제 발생: 'weapon-sword' 슬롯이 본에 제대로 부착되지 않음
- drawOrder 갱신 이후에도 '체인 무기'가 여전히 출력됨
- `updateWorldTransform()` 호출 시 `Physics` 인자를 전달하지 않으면 동작 이상
- `cmdList->Close()` 실패 발생

---

### 주요 원인 및 해결

#### 1. `updateWorldTransform()` 함수에 인자 필수
```cpp
void updateWorldTransform(Physics physics = Physics_None);
```
- 공식 문서에는 인자 없이 호출하는 예제 많지만 실제 spine-runtime 코드에서는 필수 인자
- 따라서 반드시 `updateWorldTransform(spine::Physics_None)` 형식으로 호출

---

#### 2. drawOrder는 setSkin 이후 갱신됨
- draw buffer를 잡는 시점이 `setSkin()` 이후여야 함
```cpp
m_spineSkeleton->setSkin("goblin");
m_spineSkeleton->setSlotsToSetupPose();
m_spineSkeleton->updateWorldTransform(spine::Physics_None);
// 이후 drawOrder 기준으로 버퍼 설정
```

---

#### 3. 무기 출력 문제 해결 흐름
- chain weapon이 여전히 출력되는 이유는 drawOrder에 해당 slot이 살아있기 때문
- `setSkin("weapon/morningstar")` 호출 후에도 sword가 숨겨지지 않음

해결:
- 필요 없는 슬롯 이름들 조건 걸어 제거
```cpp
if (strcmp(slot->getData().getName().buffer(), "weapon-sword") == 0)
    continue;
```

---

#### 4. DX12 CommandList->Close() 실패 원인
- 잘못된 draw buffer 메모리 접근
- 원인: `setSkin()` 이후 drawOrder가 바뀌었는데 이전 버퍼 기준으로 접근하여 메모리 범위 초과

해결:
- `setSkin()` 이후 drawOrder 기준으로 버퍼 max 길이 재설정
```cpp
size_t maxVertexCount = 0;
size_t maxIndexCount = 0;
auto drawOrder = m_spineSkeleton->getDrawOrder();
for (size_t i = 0; i < drawOrder.size(); ++i) {
    // ... (attachment 검사 및 size 갱신)
}
m_drawBuf.resize(...);
```

---

#### 5. 좌우 반전
```cpp
psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
```
- Cull 제거로 양면 렌더링

---

#### 6. 기타 처리 사항
- `TrackEntry* setAnimation(...)` 모호성 해결 위해 인덱스가 아닌 이름으로 애니메이션 지정하도록 변경
```cpp
m_spineAniState->setAnimation(0, "walk", true);
```

---

### 결과
- goblins 캐릭터가 정상 렌더링되고 무기도 정확히 출력됨
- 'weapon-sword' 제거, morningstar 출력, 좌우 반전 등 모두 기대한대로 작동

![최종결과](feaf871a-e09d-43a8-98cd-94d693326b13.png)

---

### 기타
- 코드 성능 개선 위해 AVX2 memcpy 사용 중
- 커맨드 리스트 오류 원인은 대부분 버퍼 접근 범위 초과로 확인됨
- 셰이더는 별다른 관련 없음, 순수 CPU 측 처리 문제였음

---

고생 많으셨습니다.

