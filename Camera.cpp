#include "Camera.h"

Camera::Camera(int nWidth, int nHeight)
{
	m_position = XMFLOAT4(0.0f, 0.0f, -200.0f, 1.0f);

	m_eye = XMLoadFloat4(&m_position);
	m_at = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	m_up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	m_viewMatrix = XMMatrixLookAtLH(m_eye, m_at, m_up);
	m_projectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV2, (float)nWidth / (float)nHeight, 0.01f, 1000.0f);

	XMStoreFloat4x4(&m_view, m_viewMatrix);
	XMStoreFloat4x4(&m_projection, m_projectionMatrix);
}

Camera::~Camera()
{
}

XMFLOAT4 Camera::GetPosition() const
{
	return m_position;
}

XMFLOAT4X4 Camera::GetView() const
{
	return m_view;
}

XMFLOAT4X4 Camera::GetProjection() const
{
	return m_projection;
}
