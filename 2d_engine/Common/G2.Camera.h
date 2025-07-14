#pragma once
#ifndef _G2_CAMERA_H_
#define _G2_CAMERA_H_

#include <string>
#include <DirectXMath.h>

using namespace std;
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

	static IG2Camera*	create(const std::string& name, EG2CAMERA type);
};

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class G2Camera : public IG2Camera
{
protected:
	string		m_name		;
	bool		m_beUpdate {};
	XMMATRIX	m_tmViw		= XMMatrixIdentity();
	XMMATRIX	m_tmPrj		= XMMatrixIdentity();
	XMMATRIX	m_tmVP		= XMMatrixIdentity();
	XMFLOAT3	m_vcEye		= {0.0F, 0.0F, -1.0F};						// position
	XMFLOAT3	m_vcLook	= {0.0F, 0.0F,  0.0F};						// look at position
	XMFLOAT3	m_vcUp		= {0.0F, 1.0F,  0.0F};						// up vector
	XMFLOAT3	m_vcDir		= {0.0F, 0.0F,  1.0F};						// direction
	XMFLOAT4	m_vcFNF		= {XM_PIDIV4, 800/480.0F, 1.0F, 5000.0F};	// x: fov PI / 4.0F, y: aspect ratio, z:near, w:far or

public:
	int			Type	() const  override { return EG2CAM_ZERO; }
	int			Init	(const std::any& initialValue = {}) override;
	int			Update	(const std::any& t = {}) override;

	XMMATRIX	ViewMatrix          () const override { return m_tmViw;	}
	XMMATRIX	ProjectionMatrix    () const override { return m_tmPrj; }
	XMMATRIX	ViewProjectMatrix   () const override { return m_tmVP;  }

	void		Position    (const XMFLOAT3& v) override;
	XMFLOAT3	Position    () const override			{	return 	m_vcEye; }
	void		LookAt      (const XMFLOAT3& v) override;
	XMFLOAT3	LookAt      () const override			{	return m_vcLook; }
	XMFLOAT3	Up          () const override			{	return m_vcUp;   }
	XMFLOAT3	Direction   () const override			{	return m_vcDir;  }
	void		FNF         (const XMFLOAT4& v) override;
	XMFLOAT4	FNF         () const override			{	return m_vcFNF;  }

	void		MoveLeft	(float speed) override;
	void		MoveRight	(float speed) override;
	void		MoveUp		(float speed) override;
	void		MoveDown	(float speed) override;
};

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class G2CameraFPC : public G2Camera
{
public:
	int			Type	() const  override { return EG2CAM_FPC; }
};

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class G2Camera3RD : public G2CameraFPC
{
	int			Type	() const  override { return EG2CAM_3RD; }
};

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class G2CameraUI : public G2Camera
{
public:
	G2CameraUI();
	int			Type	() const  override { return EG2CAM_UI; }
	int			Init	(const std::any& initialValue = {}) override;
	int			Update	(const std::any& t = {}) override;
};

class G2Camera2D : public G2Camera
{
public:
	G2Camera2D();
	int			Type	() const  override { return EG2CAM_2D; }
	int			Init	(const std::any& initialValue = {}) override;
	int			Update	(const std::any& t = {}) override;
};

#endif // _G2_CAMERA_H_
