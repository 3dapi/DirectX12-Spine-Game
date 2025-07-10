#include <Windows.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include "G2.ConstantsWin.h"
#include "G2.Camera.h"

using std::any_cast;
using namespace G2;

int G2Camera::Init(const std::any& initialValue)
{
	return S_OK;
}
int G2Camera::Update(const std::any& t)
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
void G2Camera::Position(const XMFLOAT3& v)
{
	if (m_vcEye.x != v.x || m_vcEye.y != v.y || m_vcEye.z != v.z)
	{
		m_beUpdate = true;
		m_vcEye = v;
	}
}
void G2Camera::LookAt(const XMFLOAT3& v)
{
	if (m_vcLook.x != v.x || m_vcLook.y != v.y || m_vcLook.z != v.z)
	{
		m_beUpdate = true;
		m_vcLook = v;
	}
}
void G2Camera::FNF(const XMFLOAT4& v)
{
	if (m_vcFNF.x != v.x || m_vcFNF.y != v.y || m_vcFNF.z != v.z || m_vcFNF.w != v.w)
	{
		m_beUpdate = true;
		m_vcFNF = v;
	}
}
void G2Camera::MoveLeft(float speed)
{
	m_beUpdate = true;
}
void G2Camera::MoveRight(float speed)
{
	m_beUpdate = true;
}
void G2Camera::MoveUp(float speed)
{
	m_beUpdate = true;
}
void G2Camera::MoveDown(float speed)
{
	m_beUpdate = true;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

G2CameraUI::G2CameraUI()
{
}

int G2CameraUI::Init(const std::any& initialValue)
{
	::SIZE  screenSize = *any_cast<::SIZE*>(IG2GraphicsD3D::instance()->getAttrib(ATT_SCREEN_SIZE));
	m_vcFNF = { (float)screenSize.cx, (float)screenSize.cy, 1.0f, 5000.0f };
	m_tmPrj = XMMatrixOrthographicLH(m_vcFNF.x, m_vcFNF.y, m_vcFNF.z, m_vcFNF.w);
	m_vcEye = { 0.0f, 0.0f, -100.0f };
	m_vcLook = { 0.0f, 0.0f,   0.0f };
	m_tmViw = XMMatrixLookAtLH(XMLoadFloat3(&m_vcEye), XMLoadFloat3(&m_vcLook), XMLoadFloat3(&m_vcUp));
	m_tmVP = m_tmViw * m_tmPrj;
	return S_OK;
}

int G2CameraUI::Update(const std::any& t)
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

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

G2Camera2D::G2Camera2D()
{
}

int G2Camera2D::Init(const std::any& initialValue /* = */ )
{
	float aspectRatio = *any_cast<float*>(IG2GraphicsD3D::instance()->getAttrib(ATT_ASPECTRATIO));
	m_vcFNF = { XM_PI / 3.0F, aspectRatio, 1.0f, 5000.0f };
	m_tmPrj = XMMatrixPerspectiveFovLH(m_vcFNF.x, m_vcFNF.y, m_vcFNF.z, m_vcFNF.w);

	m_vcEye  = { 0.0f, 0.0f, -700.0f};
	m_vcLook = { 0.0f, 0.0f,    0.0f};

	XMVECTORF32 eye  = { m_vcEye.x , m_vcEye.y , m_vcEye.z , 0.0f };
	XMVECTORF32 look = { m_vcLook.x, m_vcLook.y, m_vcLook.z, 0.0f };
	XMVECTORF32  up  = { m_vcUp.x  , m_vcUp.y , m_vcUp.z   , 0.0f };

	m_tmViw  = XMMatrixLookAtLH(eye, look, up);	m_tmVP = m_tmViw * m_tmPrj;	return S_OK;}

int G2Camera2D::Update(const std::any& t)
{
	if (m_beUpdate)
	{
		float aspectRatio = *any_cast<float*>(IG2GraphicsD3D::instance()->getAttrib(ATT_ASPECTRATIO));
		m_vcFNF.y         = aspectRatio;
		m_tmPrj = XMMatrixPerspectiveFovLH(m_vcFNF.x, m_vcFNF.y, m_vcFNF.z, m_vcFNF.w);

		XMVECTORF32 eye  = { m_vcEye.x , m_vcEye.y , m_vcEye.z , 0.0f };
		XMVECTORF32 look = { m_vcLook.x, m_vcLook.y, m_vcLook.z, 0.0f };
		XMVECTORF32  up  = { m_vcUp.x  , m_vcUp.y , m_vcUp.z   , 0.0f };
		//m_tmViw = XMMatrixLookAtLH(eye, look, up);
		m_tmVP = m_tmViw * m_tmPrj;
	}
	return S_OK;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

IG2Camera* IG2Camera::create(EG2CAMERA type)
{
	IG2Camera* ret = nullptr;
	if(EG2CAM_UI == type)
	{
		ret = new G2CameraUI;
	}
	else if (EG2CAM_2D == type)
	{
		ret = new G2Camera2D;
	}
	else if(EG2CAM_FPC == type)
	{
		ret = new G2CameraFPC;
	}
	else if(EG2CAM_3RD == type)
	{
		ret = new G2Camera3RD;
	}

	if (ret)
		ret->Init();

	return ret;
}
