#include "Camera.h"

Camera::Camera(int nWidth, int nHeight)	:
	m_nWidth(nWidth), m_nHeight(nHeight),
	m_fNear(0.01f), m_fFar(1000.0f), m_fFOV(XM_PIDIV2)
{
	m_position = XMFLOAT4(0.0f, 0.0f, -200.0f, 1.0f);

	m_eye = XMLoadFloat4(&m_position);
	m_at = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	m_up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	m_fRatio = (float)nWidth / (float)nHeight;
	XMMATRIX viewMatrix = XMMatrixLookAtLH(m_eye, m_at, m_up);
	XMMATRIX projectionMatrix = XMMatrixPerspectiveFovLH
		(m_fFOV, m_fRatio, m_fNear, m_fFar);

	m_pView = std::unique_ptr<XMFLOAT4X4>(new XMFLOAT4X4());
	XMStoreFloat4x4(m_pView.get(), viewMatrix);

	m_pProjection = std::unique_ptr<XMFLOAT4X4>(new XMFLOAT4X4());
	XMStoreFloat4x4(m_pProjection.get(), projectionMatrix);
}

Camera::~Camera()
{
}

void Camera::SetPosition(float x, float y, float z)
{
	m_position = XMFLOAT4(x, y, z, 1.0f);
	m_eye = XMLoadFloat4(&m_position);
	XMMATRIX viewMatrix = XMMatrixLookAtLH(m_eye, m_at, m_up);
	XMStoreFloat4x4(m_pView.get(), viewMatrix);
}

void Camera::SetRotation(float x, float y, float z)
{
	m_rotation = XMFLOAT4(x, y, z, 1.0f);
}

XMFLOAT4 Camera::GetPosition() const
{
	return m_position;
}

XMFLOAT4 Camera::GetRotation() const
{
	return m_rotation;
}

float Camera::GetWidth() const
{
	return (float)m_nWidth;
}

float Camera::GetHeight() const
{
	return (float)m_nHeight;
}

XMFLOAT2 Camera::GetPlaneAt(float zDepth) const
{
	XMFLOAT2 dimensions;

	float halfFOV = m_fFOV * 0.5f;
	float tanHalfFOV = tanf(halfFOV);

	// Calculate the distance from camera to zDepth
	float fDistance = zDepth - m_position.z;
	float fHalfTargetPlane = tanHalfFOV * fDistance;

	// Calculate the dimensions of the plane at zDepth
	dimensions.y = 2.0f * fHalfTargetPlane;
	dimensions.x = dimensions.y * (m_fRatio);

	return dimensions;
}

XMFLOAT4X4* Camera::GetView() const
{
	return m_pView.get();
}

XMMATRIX Camera::GetViewMatrix() const
{
	return XMLoadFloat4x4(m_pView.get());
}

XMFLOAT4X4* Camera::GetProjection() const
{
	return m_pProjection.get();
}

XMMATRIX Camera::GetProjectionMatrix() const
{
	return XMLoadFloat4x4(m_pProjection.get());
}