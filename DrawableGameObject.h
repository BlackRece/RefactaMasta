#pragma once

#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxcolors.h>
#include <DirectXCollision.h>
#include "DDSTextureLoader.h"
#include "resource.h"
#include <iostream>
#include "structures.h"
#include <wrl.h>

/*
 movement
 a current direction vector
 speed is constant
 for now, new directions are instant (no rotation periods)
*/

using namespace DirectX;
namespace wrl = Microsoft::WRL;

struct SimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 TexCoord;
};

class DrawableGameObject
{
public:
	DrawableGameObject();
	virtual ~DrawableGameObject();

	virtual HRESULT						initMesh(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext);
	virtual HRESULT						initMesh(
		wrl::ComPtr<ID3D11Device> pDevice,
		wrl::ComPtr<ID3D11DeviceContext> pContext);
	void								update(float t);
	void								draw(ID3D11DeviceContext* pContext);
	ID3D11Buffer*						getVertexBuffer() { return m_pVertexBuffer; }
	ID3D11Buffer*						getIndexBuffer() { return m_pIndexBuffer; }
	ID3D11ShaderResourceView**			getTextureResourceView() { return &m_pTextureResourceView; 	}
	XMFLOAT4X4*							getTransform() { return &m_World; }
	ID3D11SamplerState**				getTextureSamplerState() { return &m_pSamplerLinear; }
	MaterialPropertiesConstantBuffer	getMaterial() { return m_material;}

	void								setPosition(XMFLOAT3 position);
	XMFLOAT3*							getPosition() { return &m_position; }
	
	void								setScale(const float scale);
	void								setAmbientColour(XMFLOAT4 colour);



	protected:
	
	XMFLOAT4X4							m_World;
	ID3D11Buffer*						m_pVertexBuffer;
	ID3D11Buffer*						m_pIndexBuffer;
	ID3D11ShaderResourceView*			m_pTextureResourceView;
	ID3D11SamplerState *				m_pSamplerLinear;
	MaterialPropertiesConstantBuffer	m_material;
	XMFLOAT3							m_position;
	float								m_scale;
	XMFLOAT4							m_ambientColour;
	
};

