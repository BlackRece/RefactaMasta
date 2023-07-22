#ifndef CAMERA_H
#define CAMERA_H

#include <DirectXMath.h>
#include <memory>
#include <cmath>

using namespace DirectX;

class Camera
{
public:
	struct CameraFrustum
	{
		float minX;
		float maxX;
		float minY;
		float maxY;
		float minZ;
		float maxZ;
	};

	Camera(int nWidth, int nHeight);
	~Camera();

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	XMFLOAT4 GetPosition() const;
	XMFLOAT4 GetRotation() const;
	
	float GetWidth() const;
	float GetHeight() const;
	CameraFrustum GetFrustum() const;
	XMFLOAT2 GetPlaneAt(float zDepth) const;
	
	void Render();
	XMFLOAT4X4* GetView() const;
	XMMATRIX GetViewMatrix() const;
	XMFLOAT4X4* GetProjection() const;
	XMMATRIX GetProjectionMatrix() const;

private:
	XMFLOAT4 m_rotation;
	XMFLOAT4 m_position;
	XMVECTOR m_eye;
	XMVECTOR m_at;
	XMVECTOR m_up;

	std::unique_ptr<XMFLOAT4X4> m_pView;
	std::unique_ptr<XMFLOAT4X4> m_pProjection;

	int m_nWidth;
	int m_nHeight;
	float m_fNear;
	float m_fFar;
	float m_fFOV;
	float m_fRatio;
};

#endif // !CAMERA_H