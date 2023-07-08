#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <string>
#include <memory>
#include "Result.h"
#include "Camera.h"

//TODO: Switch to using the ConstantBuffers.h file
//#include "ConstantBuffers.h"
#include "structures.h"

namespace wrl = Microsoft::WRL;

class Graphics
{
public:
	Graphics(HWND hWnd);
	~Graphics();

	Result	Initialise(int width, int height);
	void	BeginDraw();
	void	EndDraw();

	void	ClearScreen(float r, float g, float b);

	Result	CreateVertexBuffer();
	Result	CompileShaderFromFile(
		const WCHAR* szFileName, 
		LPCSTR szEntryPoint, 
		LPCSTR szShaderModel, 
		ID3DBlob** ppBlobOut);

	wrl::ComPtr<ID3D11DeviceContext>	GetContext() const;
	wrl::ComPtr<ID3D11Device>			GetDevice() const;

	float GetWidth() const;
	float GetHeight() const;

	void SetupLightingConstantBuffer(const Camera& pCamera);
	void SetupMaterialConstantBuffer(MaterialPropertiesConstantBuffer material);
	void SetupTransformConstantBuffer(const XMFLOAT4X4& pTransform, const Camera& pCamera);
	void SetupConstantBuffers(ID3D11ShaderResourceView* pShaderResource, ID3D11SamplerState* pSamplerState);

private:
	Result	CreateDevice();
	Result	CreateSwapChain();
	Result	CreateDepthStencilAndView();
	Result	CreateRenderTargetView();
	void	SetViewport();

	Result	CompileShaders();
	Result	CreateVertexShader();
	Result	CreatePixelShader();
	Result	CreateConstantBuffers();


	std::string GetErrorMessage(const HRESULT hr);

	D3D_FEATURE_LEVEL m_featureLevel;

	HWND m_hWnd;
	wrl::ComPtr<ID3D11Device>			m_pDevice;
	wrl::ComPtr<ID3D11DeviceContext>	m_pDeviceContext;

	wrl::ComPtr<IDXGISwapChain>			m_pSwapChain;
	wrl::ComPtr<ID3D11RenderTargetView>	m_pRenderTargetView;
	wrl::ComPtr<ID3D11Buffer>			m_pVertexBuffer;
	wrl::ComPtr<ID3D11VertexShader>		m_pVertexShader;
	wrl::ComPtr<ID3D11PixelShader>		m_pPixelShader;
	wrl::ComPtr<ID3D11InputLayout>		m_pInputLayout;

	wrl::ComPtr<ID3D11Texture2D>		m_pDepthStencil;
	wrl::ComPtr<ID3D11DepthStencilView>	m_pDepthStencilView;

	wrl::ComPtr<ID3D11Buffer>			m_pConstantBuffer;
	wrl::ComPtr<ID3D11Buffer>			m_pMaterialConstantBuffer;
	wrl::ComPtr<ID3D11Buffer>			m_pLightConstantBuffer;

	bool	m_bIsDrawing;
	int		m_nWidth;
	int		m_nHeight;
};

#endif // !GRAPHICS_H
