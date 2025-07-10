#pragma once
#ifndef _G2_CAMERA_H_
#define _G2_CAMERA_H_

#include <DirectXMath.h>

using namespace DirectX;

enum EG2CAMERA
{
	EG2CAM_NONE		= 0x0000,
	EG2CAM_ZERO		,				// base
	EG2CAM_UI		,				// for ui
	EG2CAM_2D		,				// for 2d object
	EG2CAM_FPC		,				// for first person camera
	EG2CAM_3RD		,				// for 3rd person camera
	EG2CAM_COUNT	,				// total count
};

struct IG2Camera
{
	virtual ~IG2Camera() {};
	virtual int			Type() const = 0;
	virtual int			Init(const std::any& initialValue = {}) = 0;
	virtual int			Update(const std::any& t = {}) = 0;

	virtual XMMATRIX	ViewMatrix       () const = 0;
	virtual XMMATRIX	ProjectionMatrix () const = 0;
	virtual XMMATRIX	ViewProjectMatrix() const = 0;
	virtual void		Position    (const XMFLOAT3&) = 0;		// position == eye
	virtual XMFLOAT3	Position    ()const = 0;				// position == eye
	virtual void		LookAt      (const XMFLOAT3&) = 0;		// look at
	virtual XMFLOAT3	LookAt      ()const = 0;				// look at
	virtual XMFLOAT3	Up          ()const = 0;				// up vector
	virtual XMFLOAT3	Direction   ()const = 0;				// direction
	virtual void		FNF         (const XMFLOAT4&) = 0;		// x: fov PI / 4.0F, y: aspect ratio, z:near, w:far or
	virtual XMFLOAT4	FNF         ()const = 0;				// x: screen w, y: screen h, z: near, w: far for orthographic

	virtual void		MoveLeft    (float speed) = 0;
	virtual void		MoveRight   (float speed) = 0;
	virtual void		MoveUp      (float speed) = 0;
	virtual void		MoveDown    (float speed) = 0;

	static IG2Camera*	create(EG2CAMERA type);
};

#endif // _G2_CAMERA_H_
