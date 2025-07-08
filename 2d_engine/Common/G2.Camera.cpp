#include <Windows.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include "G2.ConstantsWin.h"
#include "G2.Camera.h"

using std::any_cast;
using namespace G2;

class G2Camera : public IG2Camera
{
protected:
	bool		m_beUpdate {};
	XMMATRIX	m_tmViw = XMMatrixIdentity();
	XMMATRIX	m_tmPrj = XMMatrixIdentity();
	XMMATRIX	m_tmVP  = XMMatrixIdentity();
	XMFLOAT3	m_vcEye  = {0.0F, 0.0F, -1.0F};			// position
	XMFLOAT3	m_vcLook = {0.0F, 0.0F,  0.0F};			// look at position
	XMFLOAT3	m_vcUp   = {0.0F, 1.0F,  0.0F};			// up vector
	XMFLOAT3	m_vcDir  = {0.0F, 0.0F,  1.0F};			// direction
	XMFLOAT4	m_vcFNF  = {XM_PIDIV4, 800/480.0F, 1.0F, 5000.0F};	// x: fov PI / 4.0F, y: aspect ratio, z:near, w:far or

public:
	int			Type() const  override { return EG2CAM_ZERO; }
	int			Init(const std::any& initialValue = {}) override
	{
		return E_FAIL;
	}
	int			Update(const std::any& t = {}) override
	{
		if (m_beUpdate)
		{
			m_vcFNF.y = *any_cast<float*>(IG2GraphicsD3D::instance()->getAttrib(ATT_ASPECTRATIO));
			m_tmPrj = XMMatrixPerspectiveFovLH(m_vcFNF.x, m_vcFNF.y, m_vcFNF.z, m_vcFNF.w);
			m_tmViw = XMMatrixLookAtLH(XMLoadFloat3(&m_vcEye), XMLoadFloat3(&m_vcLook), XMLoadFloat3(&m_vcUp));
			m_tmVP  = m_tmViw * m_tmPrj;
		}
		return S_OK;
	}

	XMMATRIX	ViewMatrix          () const override { return m_tmViw;	}
	XMMATRIX	ProjectionMatrix    () const override { return m_tmPrj; }
	XMMATRIX	ViewProjectMatrix   () const override { return m_tmVP;  }
	void		Position    (const XMFLOAT3& v) override
	{
		if (m_vcEye.x != v.x || m_vcEye.y != v.y || m_vcEye.z != v.z)
		{
			m_beUpdate = true;
			m_vcEye = v;
		}
	}
	XMFLOAT3	Position    () const override			{	return 	m_vcEye; }
	void		LookAt      (const XMFLOAT3& v) override
	{
		if (m_vcLook.x != v.x || m_vcLook.y != v.y || m_vcLook.z != v.z)
		{
			m_beUpdate = true;
			m_vcLook = v;
		}
	}
	XMFLOAT3	LookAt      () const override			{	return m_vcLook; }
	XMFLOAT3	Up          () const override			{	return m_vcUp;   }
	XMFLOAT3	Direction   () const override			{	return m_vcDir;  }
	void		FNF         (const XMFLOAT4& v) override
	{
		if (m_vcFNF.x != v.x || m_vcFNF.y != v.y || m_vcFNF.z != v.z || m_vcFNF.w != v.w)
		{
			m_beUpdate = true;
			m_vcFNF = v;
		}
	}
	XMFLOAT4	FNF         () const override			{	return m_vcFNF;  }

	void		MoveLeft(float speed) override
	{
		m_beUpdate = true;
	}
	void		MoveRight(float speed) override
	{
		m_beUpdate = true;
	}
	void		MoveUp(float speed) override
	{
		m_beUpdate = true;
	}
	void		MoveDown(float speed) override
	{
		m_beUpdate = true;
	}
};

class G2CameraFPC : public G2Camera
{
public:
	int			Type() const  override { return EG2CAM_FPC; }
};

class G2Camera3RD : public G2CameraFPC
{
	int			Type() const  override { return EG2CAM_3RD; }
};

class G2CameraUI : public G2Camera
{
public:
	G2CameraUI()
	{
		::SIZE  screenSize = *any_cast<::SIZE*>(IG2GraphicsD3D::instance()->getAttrib(ATT_SCREEN_SIZE));
		m_vcFNF = { (float)screenSize.cx, (float)screenSize.cy, 1.0f, 5000.0f };
		m_tmPrj = XMMatrixOrthographicLH(m_vcFNF.x, m_vcFNF.y, m_vcFNF.z, m_vcFNF.w);
		m_vcEye = { 0.0f, 0.0f, -100.0f };
		m_vcLook = { 0.0f, 0.0f,   0.0f };
		m_tmViw = XMMatrixLookAtLH(XMLoadFloat3(&m_vcEye), XMLoadFloat3(&m_vcLook), XMLoadFloat3(&m_vcUp));
		m_tmVP = m_tmViw * m_tmPrj;
	}
	int			Type() const  override { return EG2CAM_UI; }
	int			Update(const std::any& t = {}) override
	{
		if (m_beUpdate)
		{
			m_vcFNF.y = *any_cast<float*>(IG2GraphicsD3D::instance()->getAttrib(ATT_ASPECTRATIO));
			m_tmPrj = XMMatrixOrthographicLH(m_vcFNF.x, m_vcFNF.y, m_vcFNF.z, m_vcFNF.w);
			m_tmViw = XMMatrixLookAtLH(XMLoadFloat3(&m_vcEye), XMLoadFloat3(&m_vcLook), XMLoadFloat3(&m_vcUp));
			m_tmVP = m_tmViw * m_tmPrj;
		}
		return S_OK;
	}
};

class G2Camera2D : public G2Camera
{
public:
	G2Camera2D()
	{
		float aspectRatio = *any_cast<float*>(IG2GraphicsD3D::instance()->getAttrib(ATT_ASPECTRATIO));
		m_vcFNF = { XM_PI / 3.0F, aspectRatio, 1.0f, 5000.0f };
		m_tmPrj = XMMatrixPerspectiveFovLH(m_vcFNF.x, m_vcFNF.y, m_vcFNF.z, m_vcFNF.w);
		m_vcEye  = { 0.0f, 0.0f, -700.0f};
		m_vcLook = { 0.0f, 0.0f,    0.0f};
		m_tmViw  = XMMatrixLookAtLH(XMLoadFloat3(&m_vcEye), XMLoadFloat3(&m_vcLook), XMLoadFloat3(&m_vcUp));		m_tmVP = m_tmViw * m_tmPrj;	}
	int			Type() const  override { return EG2CAM_2D; }
	int			Update(const std::any& t = {}) override
	{
		if (m_beUpdate)
		{
			m_vcFNF.y = *any_cast<float*>(IG2GraphicsD3D::instance()->getAttrib(ATT_ASPECTRATIO));
			m_tmPrj = XMMatrixPerspectiveFovLH(m_vcFNF.x, m_vcFNF.y, m_vcFNF.z, m_vcFNF.w);
			m_tmViw = XMMatrixLookAtLH(XMLoadFloat3(&m_vcEye), XMLoadFloat3(&m_vcLook), XMLoadFloat3(&m_vcUp));
			m_tmVP = m_tmViw * m_tmPrj;
		}
		return S_OK;
	}
};


IG2Camera* IG2Camera::create(EG2CAMERA type)
{
	IG2Camera* ret = {};
	switch (type)
	{
	case EG2CAM_NONE:
		return ret;
	case EG2CAM_ZERO:
		return ret;
	case EG2CAM_UI:
		ret = new G2CameraUI;
		break;
	case EG2CAM_2D:
		ret = new G2Camera2D;
		break;
	case EG2CAM_FPC:
		ret = new G2CameraFPC;
		break;
	case EG2CAM_3RD:
		ret = new G2Camera3RD;
		break;
	}
	if (ret)
		ret->Update();

	return ret;
}
