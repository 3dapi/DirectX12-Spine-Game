#define INITGUID			// 이걸 꼭 추가
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mfobjects.h>
#include <mfplay.h>

#include <functional>
#include <tuple>
#include <vector>
#include <xaudio2.h>
#include <wrl/client.h>
#include <dxgi1_6.h>
#include <d3dx12/d3dx12.h>

#include "G2.ConstantsWin.h"
#include "G2.Util.h"
#include "G2.FactoryMfAudio.h"

using namespace std;
using Microsoft::WRL::ComPtr;

namespace G2 {

static IXAudio2*				m_pxAudio			{};
static IXAudio2MasteringVoice*	m_pXMasteringVoice	{};

struct MF_BUFFER
{
	IMFSourceReader*	mReader	{};
	IMFMediaType*		mType	{};
	WAVEFORMATEX*		mWfx	{};
	vector<uint8_t>		mBuf	;

	~MF_BUFFER()
	{
		if(!mBuf.empty())
		{
			//std::vector<uint8_t>().swap(mBuf);
			CLEAR_VECTOR(mBuf);
		}
		SAFE_RELEASE(mReader);
		SAFE_RELEASE(mType);
		if(mWfx)
		{
			CoTaskMemFree(mWfx);
			mWfx = {};
		}
	}
};
struct MF_PLAYER
{
	MF_BUFFER*				mfSrc		{};
	IXAudio2SourceVoice*	sourceVoice {};

	void Destroy()
	{
		if(sourceVoice)
		{
			sourceVoice->DestroyVoice();
			sourceVoice = {};
		}
	}
};

static MF_BUFFER* MF_Load(const std::string& file)
{
	static bool initialized = [&]()
	{
		// Media Foundation 초기화. // 한번만 실행.
		MFStartup(MF_VERSION);
		// XAudio2 초기화
		XAudio2Create(&m_pxAudio, 0);
		if(FAILED(XAudio2Create(&m_pxAudio, 0)) || !G2::m_pxAudio)
			return false;
		if(FAILED(G2::m_pxAudio->CreateMasteringVoice(&G2::m_pXMasteringVoice)) || !G2::m_pXMasteringVoice)
			return false;
		return true;
	}();

	HRESULT hr = S_OK;
	IMFSourceReader* pReader{};

	auto wFile = ansiToWstr(file);
	if(hr = MFCreateSourceReaderFromURL( wFile.c_str(), {}, &pReader); FAILED(hr))
	{
		return {};
	}
	
	// PCM 포맷으로 설정 (float, stereo)
	IMFMediaType* pType{};
	MFCreateMediaType(&pType);
	pType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	pType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
	pReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, pType);

	// 디코딩된 PCM 데이터를 읽어서 메모리에 저장
	std::vector<uint8_t> audioData;
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
			uint8_t* pData = nullptr;
			DWORD maxLen = 0, curLen = 0;
			pBuffer->Lock(&pData, &maxLen, &curLen);

			// 데이터 누적.
			audioData.insert(audioData.end(), pData, pData + curLen);

			pBuffer->Unlock();
		}
	}
	// 오디오 포맷 정보 얻기
	IMFMediaType* pNativeType{};
	hr = pReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pNativeType);
	if(FAILED(hr))
		return {};

	WAVEFORMATEX* pWfx = nullptr;
	UINT32 wfxSize = 0;
	MFCreateWaveFormatExFromMFMediaType(pNativeType, &pWfx, &wfxSize);
	if(!pWfx)
		return {};

	return new MF_BUFFER{pReader, pNativeType, pWfx, audioData};
}

static MF_PLAYER* MF_Create(MF_BUFFER* mfSrc)
{
	IXAudio2SourceVoice* sourceVoice = nullptr;
	G2::m_pxAudio->CreateSourceVoice(&sourceVoice, mfSrc->mWfx);
	if(!sourceVoice)
		return {};

	return new MF_PLAYER{mfSrc, sourceVoice};
}

static void MF_Play(MF_PLAYER* player, bool bLoop=true, float fVolume=1.0F)
{
	if(!player || !player->sourceVoice)
		return;
	player->sourceVoice->SetVolume(fVolume);  // 0.0f ~ 1.0f

	// 버퍼 설정
	XAUDIO2_BUFFER buffer = {};
	buffer.AudioBytes = static_cast<UINT32>(player->mfSrc->mBuf.size());
	buffer.pAudioData = player->mfSrc->mBuf.data();
	buffer.Flags = XAUDIO2_END_OF_STREAM;

	//// 구간 반복
	//buffer.LoopBegin = 44100 * 5;      // 5초 위치부터
	//buffer.LoopLength = 44100 * 10;    // 10초 길이 반복
	//buffer.LoopCount = XAUDIO2_LOOP_INFINITE;

	// 무한 반복?
	buffer.LoopCount = bLoop? XAUDIO2_LOOP_INFINITE : XAUDIO2_NO_LOOP_REGION;

	player->sourceVoice->SubmitSourceBuffer(&buffer);
	player->sourceVoice->SetVolume(fVolume);
	player->sourceVoice->Start();
}

static void MF_Stop(MF_PLAYER* player)
{
	if(!player || !player->sourceVoice)
		return;
	player->sourceVoice->Stop();
	player->sourceVoice->FlushSourceBuffers();
}

static void MF_Pause(MF_PLAYER* player)
{
	if(!player || !player->sourceVoice)
		return;
	player->sourceVoice->Stop();
}
static void MF_Resume(MF_PLAYER* player)
{
	if(!player || !player->sourceVoice)
		return;
	player->sourceVoice->Start(0);
}

static bool MF_Isplaying(MF_PLAYER* player)
{
	if(!player || !player->sourceVoice)
		return false;
	XAUDIO2_VOICE_STATE state{};
	player->sourceVoice->GetState(&state);
	return (state.BuffersQueued > 0);
}

static void UnloadMF()
{
	if(G2::m_pXMasteringVoice)
	{
		G2::m_pXMasteringVoice->DestroyVoice();
		G2::m_pXMasteringVoice = {};
	}
	SAFE_RELEASE(G2::m_pxAudio);
	MFShutdown();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------

TD3D_MFAUDIO::~TD3D_MFAUDIO()
{
	CLEAR_VECTOR(	name	);
	SAFE_DELETE(r);
}

FactoryMfAudio* FactoryMfAudio::instance()
{
	static FactoryMfAudio inst;
	return &inst;
}

TD3D_MFAUDIO* FactoryMfAudio::ResourceLoad(const string& file)
{
	auto itr = this->m_db.find(file);
	if (itr != this->m_db.end())
	{
		return itr->second.get();
	}

	auto mf = MF_Load(file);
	if(!mf)
		return {};

	auto pItem = std::make_unique<TD3D_MFAUDIO>();
	pItem->name = file;
	pItem->r    = mf;
	auto [it, success] = m_db.insert({ file, std::move(pItem) });
	auto ret = it->second.get();
	return ret;
}
TD3D_MFAUDIO* FactoryMfAudio::ResourceFind(const string& name) const
{
	auto itr = this->m_db.find(name);
	if (itr != this->m_db.end())
	{
		return itr->second.get();
	}
	static TD3D_MFAUDIO dummy{ "<none>", {}};
	return &dummy;
}
int FactoryMfAudio::ResourceUnLoad(const string& name)
{
	auto itr = this->m_db.find(name);
	if (itr != this->m_db.end())
	{
		m_db.erase(itr);
		return S_OK;
	}
	return E_FAIL;
}

struct MF_BUFFER* FactoryMfAudio::FindRes(const std::string& name) const
{
	auto itr = this->m_db.find(name);
	if (itr != this->m_db.end())
	{
		return itr->second->r;
	}
	return {};
}

void FactoryMfAudio::UnLoadAudioLibrary()
{
	UnloadMF();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------

PG2AUDIOPLAYER MfAudioPlayer::Create(const std::string& file)
{
	auto* ret = new(std::nothrow) MfAudioPlayer;
	if(ret && 0 <= ret->Init(file))
		return ret;
	delete ret;
	return {};
}

int MfAudioPlayer::Init(const std::string& file)
{
	auto mf = FactoryMfAudio::instance()->Load(file);
	if(!mf->r)
		return E_FAIL;
	auto player = MF_Create(mf->r);
	if(!player)
		return E_FAIL;
	m_player = player;
	return S_OK;
}

int MfAudioPlayer::Destroy()
{
	if (m_player)
	{
		MF_Stop(m_player);
		m_player->Destroy();  // 내부 리소스 해제
		delete m_player;
		m_player = {};
	}
	return S_OK;
}

void MfAudioPlayer::Play(bool bLoop, float fVolume)
{
	if(m_player)
		MF_Play(m_player, bLoop, fVolume);
}

void MfAudioPlayer::Stop()
{
	if(m_player)
		MF_Stop(m_player);
}

bool MfAudioPlayer::IsPlaying()
{
	if(!m_player)
		return false;
	return MF_Isplaying(m_player);
}

void MfAudioPlayer::Pause()
{
	if(m_paused || !m_player)
		return;
	MF_Pause(m_player);
	m_paused = true;
}

void MfAudioPlayer::Resume()
{
	if(!m_paused || !m_player)
		return;
	MF_Resume(m_player);
	m_paused = false;
}

} // namespace G2
