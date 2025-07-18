﻿
#pragma once
#ifndef _G2_CONSTANTS_H_
#define _G2_CONSTANTS_H_

#include <any>
#include <string>

namespace G2 {

enum
{
	FRAME_BUFFER_COUNT = 2,
};

struct IG2AppFrame
{
	virtual ~IG2AppFrame() = default;

	static IG2AppFrame* instance();
	virtual int init(const std::any& initialValue = {}) = 0;
	virtual int destroy() = 0;

	virtual std::any getAttrib(int nAttrib) = 0;
	virtual int      setAttrib(int nAttrib, const std::any& v = {}) = 0;
	virtual int      command(int nCmd, const std::any& v = {}) = 0;
};

enum EG2GRAPHICS
{
	EG2_GRAPHICS_NONE = 0x0000,
	EG2_D3D9    = 0x0900,		EG2_D3D10     = 0x1100,	EG2_D3D11     = 0x1100,	EG2_D3D12  = 0x1200,
	EG2_OGL_2   = 0x2000,		EG2_OGL_30    = 0x3000,	EG2_OGL_31    = 0x3100,	EG2_OGL_32 = 0x3200,
	EG2_VULKAN1 = 0x4100,		EG2_VULKAN1_2 = 0x4200,	EG2_VULKAN1_3 = 0x4300,
	EG2_METAL   = 0x8100,
};

struct IG2Graphics
{
	virtual ~IG2Graphics() = default;

	static IG2Graphics* instance();
	virtual EG2GRAPHICS type() const = 0;
	virtual int init(const std::any& initialValue = {}) = 0;
	virtual int destroy() = 0;

	virtual std::any getAttrib(int nAttrib) = 0;
	virtual int      setAttrib(int nAttrib, const std::any& v = {}) = 0;
	virtual int      command(int nCmd, const std::any& v = {}) = 0;
};

struct IG2Scene
{
	virtual ~IG2Scene() = default;
	virtual int		Type()									= 0;
	virtual int		Init(const std::any& initialValue = {}) = 0;
	virtual int		Destroy()								= 0;
	virtual int		Update(const std::any& t)				= 0;
	virtual int		Render()								= 0;
	virtual int		Notify(const std::string&, const std::any&) = 0;
};
typedef IG2Scene*			PG2SCENE;

struct IG2Object
{
	virtual ~IG2Object() = default;
	virtual int		Type()									= 0;
	virtual int		Init(const std::any& initialValue = {}) = 0;
	virtual int		Destroy()								= 0;
	virtual int		Update(const std::any& t)				= 0;
	virtual int		Render()								= 0;
};
typedef IG2Object*	PG2OBJECT;

struct IG2Listener
{
	virtual ~IG2Listener() = default;
	virtual int		Notify(const std::string&, const std::any&)= 0;
};
typedef IG2Listener*	PG2LISTENER;

struct IG2AudioPlayer
{
	virtual int Init(const std::string& file)				= 0;
	virtual int Destroy()									= 0;
	virtual void Play(bool bLoop=true, float fVolume=1.0F, float fSpeed=1.0F)	= 0;
	virtual void Stop()										= 0;
	virtual bool IsPlaying()								= 0;
	virtual void Pause()									= 0;
	virtual void Resume()									= 0;
};
typedef IG2AudioPlayer*	PG2AUDIOPLAYER;

} // namespace G2 {

#endif // _G2Constants_H_


