#ifndef CAMERA_H
#define CAMERA_H

#include <DirectXMath.h>
#include <memory>

using namespace DirectX;

class Camera
{
public:
	Camera(int nWidth, int nHeight);
	~Camera();

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	XMFLOAT4 GetPosition() const;
	XMFLOAT3 GetRotation() const;
	
	float GetWidth() const;
	float GetHeight() const;
	
	void Render();
	XMFLOAT4X4* GetView() const;
	XMMATRIX GetViewMatrix() const;
	XMFLOAT4X4* GetProjection() const;
	XMMATRIX GetProjectionMatrix() const;


private:
	XMFLOAT4 m_position;
	XMVECTOR m_eye;
	XMVECTOR m_at;
	XMVECTOR m_up;

	XMMATRIX m_viewMatrix;
	XMMATRIX m_projectionMatrix;

	std::unique_ptr<XMFLOAT4X4> m_pView;
	std::unique_ptr<XMFLOAT4X4> m_pProjection;

	int m_nWidth;
	int m_nHeight;
};

#endif // !CAMERA_H