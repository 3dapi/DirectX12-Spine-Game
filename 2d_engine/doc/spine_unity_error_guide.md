
# Spine.Unity 에러 해결 가이드

## 🧩 문제
`using Spine.Unity;` 코드에서 다음과 같은 컴파일 에러가 발생합니다:

```
The type or namespace name 'Spine' or 'Spine.Unity' could not be found...
```

## 📌 원인
Unity 프로젝트에 Spine용 런타임 코드가 포함되지 않았기 때문입니다.

---

## ✅ 해결 방법

### 1. Spine Unity Runtime 설치

#### ▶ Spine 공식 GitHub에서 수동 설치:
1. [Spine Unity Runtime GitHub 링크](https://github.com/EsotericSoftware/spine-runtimes/tree/4.1/spine-unity) 접속
2. 다음 폴더 2개를 다운로드:
   - `spine-unity`
   - `spine-csharp`
3. 둘 다 Unity 프로젝트의 `Assets/` 하위에 복사

```
Assets/
 ├─ spine-unity/
 └─ spine-csharp/
```

4. Unity 에디터에서 자동으로 컴파일됨 → `using Spine.Unity` 정상 작동

---

### 2. Unity Package Manager로 설치 (권장하지 않음)
Spine 공식 에디터 사용자는 직접 다운받아 설치해야 합니다. Unity Package Manager에서는 제공되지 않음.

---

### 3. 기타 점검 사항

#### 📌 네임스페이스 확인
- `Spine.Unity` 철자 및 대소문자 정확히 확인

#### 🔁 캐시 문제 해결
- Unity Editor 재시작
- `Library/` 폴더 삭제 후 Unity 재실행 (재빌드됨)

---

## 💡 참고

- Spine Unity Runtime은 Spine Editor를 정품 구매한 사용자만 사용 가능합니다.
- Spine Unity Runtime의 버전은 사용하는 Spine Editor 버전과 반드시 일치해야 합니다 (예: Spine 4.1 → Runtime 4.1).

---

## 📎 링크

- Spine Unity Runtime GitHub:  
  https://github.com/EsotericSoftware/spine-runtimes/tree/4.1/spine-unity

- Spine 공식 홈페이지:  
  https://esotericsoftware.com/spine-unity-download
