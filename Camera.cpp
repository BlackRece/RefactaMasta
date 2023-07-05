#include "Camera.h"

Camera::Camera(int nWidth, int nHeight)
{
	m_position = XMFLOAT4(0.0f, 0.0f, -200.0f, 1.0f);

	m_eye = XMLoadFloat4(&m_position);
	m_at = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	m_up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	m_viewMatrix = XMMatrixLookAtLH(m_eye, m_at, m_up);
	m_projectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV2, (float)nWidth / (float)nHeight, 0.01f, 1000.0f);

	m_pView = std::unique_ptr<XMFLOAT4X4>(new XMFLOAT4X4());
	XMStoreFloat4x4(m_pView.get(), m_viewMatrix);

	m_pProjection = std::unique_ptr<XMFLOAT4X4>(new XMFLOAT4X4());
	XMStoreFloat4x4(m_pProjection.get(), m_projectionMatrix);
}

Camera::~Camera()
{
}


XMFLOAT4 Camera::GetPosition() const
{
	return m_position;
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