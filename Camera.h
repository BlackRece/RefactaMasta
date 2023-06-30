#ifndef CAMERA_H
#define CAMERA_H

//#include <d3d11_1.h>
#include <DirectXMath.h>

using namespace DirectX;

class Camera
{
public:
	Camera(int nWidth, int nHeight);
	~Camera();

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	DirectX::XMFLOAT4 GetPosition() const;
	DirectX::XMFLOAT3 GetRotation() const;
	void Render();
	XMFLOAT4X4 GetView() const;
	XMFLOAT4X4 GetProjection() const;

private:
	XMFLOAT4 m_position;
	XMVECTOR m_eye;
	XMVECTOR m_at;
	XMVECTOR m_up;

	XMMATRIX m_viewMatrix;
	XMMATRIX m_projectionMatrix;

	XMFLOAT4X4 m_view;
	XMFLOAT4X4 m_projection;
};

#endif // !CAMERA_H