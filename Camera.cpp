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

Camera::CameraFrustum Camera::GetFrustum() const
{
	// Calculate the corners of the frustum in the view space.
	/*
	float aspectRatio = (float)m_nWidth / (float)m_nHeight;
	float nearPlaneWidth = 2.0f;
	float nearPlaneHeight = 2.0f / aspectRatio;
	float farPlaneWidth = nearPlaneWidth;
	float farPlaneHeight = nearPlaneHeight;
	*/
	float aspectRatio = (float)m_nWidth / (float)m_nHeight;
	float nearPlaneWidth = 2.0f * tanf(XM_PIDIV4); // Horizontal FOV is PI/2
	float nearPlaneHeight = nearPlaneWidth / aspectRatio;
	float farPlaneWidth = nearPlaneWidth;
	float farPlaneHeight = nearPlaneHeight;

	XMVECTOR corners[8] =
	{
		XMVectorSet(-nearPlaneWidth / 2.0f, nearPlaneHeight / 2.0f, 0.0f, 1.0f),    // Top left near
		XMVectorSet(nearPlaneWidth / 2.0f, nearPlaneHeight / 2.0f, 0.0f, 1.0f),     // Top right near
		XMVectorSet(-nearPlaneWidth / 2.0f, -nearPlaneHeight / 2.0f, 0.0f, 1.0f),   // Bottom left near
		XMVectorSet(nearPlaneWidth / 2.0f, -nearPlaneHeight / 2.0f, 0.0f, 1.0f),    // Bottom right near
		XMVectorSet(-farPlaneWidth / 2.0f, farPlaneHeight / 2.0f, 1.0f, 1.0f),      // Top left far
		XMVectorSet(farPlaneWidth / 2.0f, farPlaneHeight / 2.0f, 1.0f, 1.0f),       // Top right far
		XMVectorSet(-farPlaneWidth / 2.0f, -farPlaneHeight / 2.0f, 1.0f, 1.0f),     // Bottom left far
		XMVectorSet(farPlaneWidth / 2.0f, -farPlaneHeight / 2.0f, 1.0f, 1.0f)       // Bottom right far

		/*
		XMVectorSet(-1.0f, 01.0f, 00.0f, 01.0f),	// Top left near
		XMVectorSet(01.0f, 01.0f, 00.0f, 01.0f),	// Top right near
		XMVectorSet(-1.0f, -1.0f, 00.0f, 01.0f),	// Bottom left near
		XMVectorSet(01.0f, -1.0f, 00.0f, 01.0f),	// Bottom right near
		XMVectorSet(-1.0f, 01.0f, 01.0f, 01.0f),	// Top left far
		XMVectorSet(01.0f, 01.0f, 01.0f, 01.0f),	// Top right far
		XMVectorSet(-1.0f, -1.0f, 01.0f, 01.0f),	// Bottom left far
		XMVectorSet(01.0f, -1.0f, 01.0f, 01.0f)		// Bottom right far
		*/
	};

	// Transform the frustum corners from view space to world space.
	/*
	XMMATRIX viewMatrix = GetViewMatrix();
	for (int i = 0; i < 8; i++)
		corners[i] = XMVector4Transform(corners[i], viewMatrix);
	*/
	XMMATRIX vpMatrix = GetViewMatrix() * GetProjectionMatrix();
	XMMATRIX invProjection = XMMatrixInverse(0, GetProjectionMatrix());

	for (int i = 0; i < 8; i++)
		corners[i] = XMVector4Transform(corners[i], invProjection);
		//corners[i] = XMVector4Transform(corners[i], vpMatrix);

	// Calculate the frustum bounds in world space.
	float minX = FLT_MAX;
	float maxX = -FLT_MAX;
	float minY = FLT_MAX;
	float maxY = -FLT_MAX;
	float minZ = FLT_MAX;
	float maxZ = -FLT_MAX;

	for (int i = 0; i < 8; i++)
	{
		XMFLOAT4 corner;
		XMStoreFloat4(&corner, corners[i]);

		if (corner.x < minX) minX = corner.x;
		if (corner.x > maxX) maxX = corner.x;
		if (corner.y < minY) minY = corner.y;
		if (corner.y > maxY) maxY = corner.y;
		if (corner.z < minZ) minZ = corner.z;
		if (corner.z > maxZ) maxZ = corner.z;
	}
	
	// Return the frustum.
	CameraFrustum frustum;
	frustum.minX = minX;
	frustum.maxX = maxX;
	frustum.minY = minY;
	frustum.maxY = maxY;
	frustum.minZ = minZ;
	frustum.maxZ = maxZ;

	return frustum;
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