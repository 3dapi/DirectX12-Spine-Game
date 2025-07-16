#define INITGUID                      // 이걸 꼭 추가
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mfobjects.h>
#include <mfplay.h>
#include <xaudio2.h>
#include <wrl/client.h>
#include <iostream>

#include <vector>
#include <iostream>

#pragma comment(lib, "xaudio2.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mf.lib")


using Microsoft::WRL::ComPtr;

#define SAFE_RELEASE(x) if (x) { x->Release(); x = nullptr; }

int main()
{
	HRESULT hr = S_OK;

	// Media Foundation 초기화
	MFStartup(MF_VERSION);

	// 파일 열기
	ComPtr<IMFSourceReader> pReader;
	hr = MFCreateSourceReaderFromURL(L"a.mp3", nullptr, &pReader);
	if(FAILED(hr)) return -1;

	// PCM 포맷으로 설정 (float, stereo)
	ComPtr<IMFMediaType> pType;
	MFCreateMediaType(&pType);
	pType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	pType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
	pReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, pType.Get());

	// 오디오 포맷 정보 얻기
	ComPtr<IMFMediaType> pNativeType;
	hr = pReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pNativeType);
	WAVEFORMATEX* pWfx = nullptr;
	UINT32 wfxSize = 0;
	MFCreateWaveFormatExFromMFMediaType(pNativeType.Get(), &pWfx, &wfxSize);

	// XAudio2 초기화
	ComPtr<IXAudio2> pXAudio;
	XAudio2Create(&pXAudio, 0);
	IXAudio2MasteringVoice* pMasterVoice = nullptr;
	pXAudio->CreateMasteringVoice(&pMasterVoice);

	IXAudio2SourceVoice* pSourceVoice = nullptr;
	pXAudio->CreateSourceVoice(&pSourceVoice, pWfx);


	pSourceVoice->SetVolume(0.5f);  // 0.0f ~ 1.0f 사이 (절반 음량)

	// 디코딩된 PCM 데이터를 읽어서 메모리에 저장
	std::vector<BYTE> audioData;
	while(true)
	{
		ComPtr<IMFSample> pSample;
		DWORD dwFlags = 0;
		hr = pReader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr, &dwFlags, nullptr, &pSample);
		if(FAILED(hr) || (dwFlags & MF_SOURCE_READERF_ENDOFSTREAM))
			break;

		if(pSample)
		{
			ComPtr<IMFMediaBuffer> pBuffer;
			pSample->ConvertToContiguousBuffer(&pBuffer);
			BYTE* pData = nullptr;
			DWORD maxLen = 0, curLen = 0;
			pBuffer->Lock(&pData, &maxLen, &curLen);
			audioData.insert(audioData.end(), pData, pData + curLen);
			pBuffer->Unlock();
		}
	}

	// 버퍼 설정
	XAUDIO2_BUFFER buffer = {};
	buffer.AudioBytes = static_cast<UINT32>(audioData.size());
	buffer.pAudioData = audioData.data();
	buffer.Flags = XAUDIO2_END_OF_STREAM;

	//// 구간 반복
	//buffer.LoopBegin = 44100 * 5;      // 5초 위치부터
	//buffer.LoopLength = 44100 * 10;    // 10초 길이 반복
	//buffer.LoopCount = XAUDIO2_LOOP_INFINITE;

	// 바로 이 부분이 무한 반복!
	buffer.LoopCount = XAUDIO2_LOOP_INFINITE;

	pSourceVoice->SubmitSourceBuffer(&buffer);
	pSourceVoice->Start();

	std::wcout << L"[a.mp3 재생 중 - 엔터 키 입력 시 종료]\n";
	std::wcin.get();

	// 정리
	pSourceVoice->DestroyVoice();
	pMasterVoice->DestroyVoice();
	CoTaskMemFree(pWfx);
	MFShutdown();

	return 0;
}
