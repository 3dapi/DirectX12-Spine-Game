
# 🎧 Audio Playback (XAudio2 + Media Foundation) Summary

## ✅ 기술 스택
- **XAudio2**: 오디오 출력 (재생)
- **Media Foundation (MF)**: MP3/WAV 등의 미디어 파일 디코딩
- **DirectXTK12 사용 중**

---

## ✅ 초기화 흐름

### 🔹 Media Foundation 초기화 (1회만)
```cpp
MFStartup(MF_VERSION);
```

> 단일 실행 보장:
```cpp
static const int once = [](){ MFStartup(MF_VERSION); return 0; }();
```

### 🔹 XAudio2 초기화 (1회만)
```cpp
ComPtr<IXAudio2> xaudio;
XAudio2Create(&xaudio);
xaudio->CreateMasteringVoice(&masterVoice);
```

---

## ✅ 파일 로딩 (`MF_Load`)

- `MFCreateSourceReaderFromURL()`로 소스 리더 생성
- 디코딩된 PCM 데이터를 `ReadSample()`로 순차적으로 수집
- 데이터를 `std::vector<uint8_t>`에 저장

```cpp
while (true) {
    pReader->ReadSample(..., &pSample);
    pSample->ConvertToContiguousBuffer(&pBuffer);
    pBuffer->Lock(...); // → audioData.insert(...)
    pBuffer->Unlock();
}
```

> 📌 `audioData`는 move 시 복사 없음

---

## ✅ 재생 구조

- `MfAudioPlayer` 객체가 `sourceVoice`를 내부에 보유
- `SubmitSourceBuffer()`로 데이터 제출
- `Start()`로 재생

```cpp
sourceVoice->SubmitSourceBuffer(&buffer);
sourceVoice->Start();
```

- 무한 반복은 `XAUDIO2_END_OF_STREAM` 처리 후 루프 재등록

---

## ✅ 볼륨 조절
```cpp
sourceVoice->SetVolume(float volume); // 0.0f ~ 1.0f
```

---

## ✅ 재생 상태 확인
```cpp
XAUDIO2_VOICE_STATE state;
sourceVoice->GetState(&state);
bool isPlaying = state.BuffersQueued > 0;
```

---

## ✅ 정지 및 Flush
```cpp
sourceVoice->Stop();
sourceVoice->FlushSourceBuffers(); // 완전 정지
```

---

## ✅ 유틸 함수
- `CLEAR_VECTOR(vec)` : vector 초기화 (swap 기반)
- `SAFE_DELETE_VECTOR(vec)` : 포인터 vector 삭제 후 초기화

---

## 🧩 기타 메모

- `.wav`도 `MFCreateSourceReaderFromURL`로 재생 가능
- 앱이 백그라운드로 나가도 자동 정지는 안됨 → 직접 구현 필요

---

© Audio System 구현 요약 – Powered by XAudio2 + MF (2025)

---

## ⏸ Pause / ▶️ Resume 기능

### 🔹 일시정지 (Pause)
```cpp
sourceVoice->Stop(); // 현재 재생 위치 기억됨
```
- 단순히 `Stop()`만 호출하면 **버퍼는 유지됨**
- 이후 Resume 시 같은 지점부터 재생 가능

### 🔹 재개 (Resume)
```cpp
sourceVoice->Start(); // 이어서 재생
```

---

## 🆚 Pause vs Stop 차이

| 기능 | Pause (`Stop`) + Resume | Stop + Flush |
|------|--------------------------|--------------|
| 현재 위치 저장 | ✅ | ❌ |
| Resume 가능 | ✅ | ❌ |
| 버퍼 유지 | ✅ | ❌ (Flush로 제거됨) |
| 완전 정지 | ❌ | ✅ |
| 반복 처리 시 재등록 필요 | ❌ | ✅ (다시 Submit 필요) |

> `FlushSourceBuffers()`를 호출하면 **완전한 정지 상태**가 되어, 다시 데이터를 Submit해야 재생 가능.

---
