
#pragma once
#ifndef __G2_FACTORY_MFAUDIO_H__
#define __G2_FACTORY_MFAUDIO_H__

#include <string>
#include <tuple>
#include <vector>
#include <Windows.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include "G2.Factory.h"
#include "G2.Constants.h"

using namespace DirectX;

namespace G2 {

struct TD3D_MFAUDIO
{
	std::string				name;	// mf name same to file name
	struct MF_BUFFER*		r{};	// resource
	~TD3D_MFAUDIO();
};

class FactoryMfAudio : public IG2Factory<FactoryMfAudio, TD3D_MFAUDIO>
{
public:
	static FactoryMfAudio* instance();
	TD3D_MFAUDIO* ResourceLoad(const std::string& file);
public:
	TD3D_MFAUDIO* ResourceFind(const std::string& name) const override;
	int ResourceUnLoad(const std::string& name) override;
	struct MF_BUFFER* FindRes(const std::string& name) const;

	void UnLoadAudioLibrary();
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------

class MfAudioPlayer : public IG2AudioPlayer
{
public:
	static PG2AUDIOPLAYER Create(const std::string& file);
public:
	int		Init(const std::string& file)				override;
	int		Destroy()									override;
	void	Play(bool bLoop=true, float fVolume=1.0F)	override;
	void	Stop()										override;
	bool	IsPlaying()									override;
	void	Pause()										override;
	void	Resume()									override;
protected:
	struct MF_PLAYER* m_player{};
	bool	m_paused	{};
};

} // namespace G2

#endif // __G2_FACTORY_MFAUDIO_H__
