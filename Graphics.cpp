#include "Graphics.h"

Graphics::Graphics(HWND hWnd) :
	m_hWnd(hWnd), m_bIsDrawing(false)
{
}

Graphics::~Graphics()
{
    m_pVertexBuffer.Reset(); // Release vertex buffer
    m_pRenderTargetView.Reset(); // Release render target view
    m_pSwapChain.Reset(); // Release swap chain
    m_pDeviceContext.Reset(); // Release device context
    m_pDevice.Reset(); // Release device
}

Result Graphics::Initialise(int width, int height)
{
    m_nWidth = width;
    m_nHeight = height;

    Result result = CreateDevice();
    if (!result.IsSuccess())
        return result;

    result = CreateSwapChain();
    if (!result.IsSuccess())
        return result;

    result = CreateDepthStencilAndView();
    if (!result.IsSuccess())
		return result;

    result = CreateRenderTargetView();
    if (!result.IsSuccess())
        return result;

    SetViewport();

    result = CompileShaders();
    if (!result.IsSuccess())
        return result;

    m_pCamera = std::make_unique<Camera>(m_nWidth, m_nHeight);

    //TODO: Initialise meshes of drawable objects

    /*result = CreateVertexBuffer();
    if (!result.IsSuccess())
        return result;*/

    return Result(true, this);
}

void Graphics::BeginDraw()
{
    if (m_bIsDrawing)
        return;

    ClearScreen(1.0f, 1.0f, 1.0f);
    m_pDeviceContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), nullptr);
    m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    m_bIsDrawing = true;
}

void Graphics::EndDraw()
{
    if (!m_bIsDrawing)
        return;

    m_pSwapChain->Present(1u, 0u);
    m_bIsDrawing = false;
}

void Graphics::ClearScreen(float r, float g, float b)
{
    if (m_bIsDrawing)
        return;

    const float color[4] = { r, g, b, 1.0f };
    m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView.Get(), color);
    m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

Result Graphics::CreateDevice()
{
    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    HRESULT hr = S_OK;

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {

        hr = D3D11CreateDevice(
            0,
            driverTypes[driverTypeIndex],
            0,
            createDeviceFlags,
            featureLevels,
            numFeatureLevels,
            D3D11_SDK_VERSION,
            m_pDevice.GetAddressOf(),
            &m_featureLevel,
            m_pDeviceContext.GetAddressOf()
        );

        if (hr == E_INVALIDARG)
        {
            // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
            hr = D3D11CreateDevice(
                0,
                driverTypes[driverTypeIndex],
                0,
                createDeviceFlags,
                &featureLevels[1],
                numFeatureLevels - 1,
                D3D11_SDK_VERSION,
                m_pDevice.GetAddressOf(),
                &m_featureLevel,
                m_pDeviceContext.GetAddressOf()
            );
        }

        if (SUCCEEDED(hr))
            break;
    }

    return Result(
        SUCCEEDED(hr), this,
        std::string(__func__) +
        ": Failed to create device and context.\n" + GetErrorMessage(hr));

}

Result Graphics::CreateSwapChain()
{
    HRESULT hr = S_OK;

    // Obtain the DXGI factory from the device
    wrl::ComPtr<IDXGIDevice> dxgiDevice;
    hr = m_pDevice.As(&dxgiDevice);
    if (FAILED(hr))
        return Result(
            false, this,
            std::string(__func__) +
            ": Could not get IDXGIDevice - " + GetErrorMessage(hr));

    wrl::ComPtr<IDXGIAdapter> dxgiAdapter;
    hr = dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf());
    if (FAILED(hr))
        return Result(
            false, this,
            std::string(__func__) +
            ": Could not get IDXGIAdapter - " + GetErrorMessage(hr));

    wrl::ComPtr<IDXGIFactory> dxgiFactory;
    hr = dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));
    if (FAILED(hr))
        return Result(
            false, this,
            std::string(__func__) +
            "Could not get IDXGIFactory" + GetErrorMessage(hr));

    // Create the swap chain
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferDesc.Width =    m_nWidth;
    swapChainDesc.BufferDesc.Height =   m_nHeight;
    swapChainDesc.BufferDesc.Format =   DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 1;
    swapChainDesc.OutputWindow = m_hWnd;
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    hr = dxgiFactory->CreateSwapChain(m_pDevice.Get(), &swapChainDesc, m_pSwapChain.GetAddressOf());

    return Result(
        SUCCEEDED(hr), this,
        std::string(__func__) +
        ": Could not create swap chain. - " + GetErrorMessage(hr));
}

Result Graphics::CreateDepthStencilAndView()
{
    HRESULT hr = S_OK;

    // Create depth stencil texture
    D3D11_TEXTURE2D_DESC descDepth = {};
    descDepth.Width = m_nWidth;
    descDepth.Height = m_nHeight;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;

    hr = m_pDevice->CreateTexture2D(
        &descDepth,
        0,
        m_pDepthStencil.GetAddressOf());
    if (FAILED(hr))
        return Result(
            false, this,
            std::string(__func__) +
            ": Could not create depth stencil texture.\n" +
            GetErrorMessage(hr));

    // Create the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = m_pDevice->CreateDepthStencilView(
        m_pDepthStencil.Get(),
        &descDSV,
        m_pDepthStencilView.GetAddressOf());
    if (FAILED(hr))
        return Result(
            false, this,
            std::string(__func__) +
            ": Could not create depth stencil view.\n" +
            GetErrorMessage(hr));

    return Result(true, this);
}

Result Graphics::CreateRenderTargetView()
{
    HRESULT hr = S_OK;

    wrl::ComPtr<ID3D11Texture2D> pBackBuffer;
    hr = m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    if (FAILED(hr))
        return Result(
            false, this,
            std::string(__func__) +
            ": Could not get back buffer" + GetErrorMessage(hr));

    hr = m_pDevice->CreateRenderTargetView(pBackBuffer.Get(), 0, m_pRenderTargetView.GetAddressOf());
    if (FAILED(hr))
        return Result(
            false, this,
            std::string(__func__) +
            ": Could not create render target view" + GetErrorMessage(hr));

    // Set the render target
    m_pDeviceContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), 0);

    return Result(true, this);
}

void Graphics::SetViewport()
{
    // Create and set the viewport
    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<float>(m_nWidth);
    viewport.Height = static_cast<float>(m_nHeight);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;

    m_pDeviceContext->RSSetViewports(1, &viewport);
}

Result Graphics::CompileShaders()
{
    Result result = CreateVertexShader();
    if (!result.IsSuccess())
        return result;

    result = CreatePixelShader();
    if (!result.IsSuccess())
        return result;

    result = CreateConstantBuffers();
    if (!result.IsSuccess())
        return result;

    return Result(true, this);
}

Result Graphics::CreateVertexShader()
{
    HRESULT hr = S_OK;
    ID3DBlob* pVSBlob = nullptr;

    //Result result = CompileShaderFromFile(L"Shaders\\Shader.hlsl", "VS", "vs_4_0", &pVSBlob);
    Result result = CompileShaderFromFile(L"Shader.fx", "VS", "vs_4_0", &pVSBlob);
    if (!result.IsSuccess())
        return result;

    hr = m_pDevice->CreateVertexShader(
        pVSBlob->GetBufferPointer(),
        pVSBlob->GetBufferSize(),
        0,
        m_pVertexShader.GetAddressOf());
    if (FAILED(hr))
    {
        pVSBlob->Release();
        return Result(
            false, this,
            std::string(__func__) +
            ": Could not create vertex shader. - " + GetErrorMessage(hr));
    }

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
    hr = m_pDevice->CreateInputLayout(
        layout,
        numElements,
        pVSBlob->GetBufferPointer(),
        pVSBlob->GetBufferSize(),
        m_pInputLayout.GetAddressOf());
    pVSBlob->Release();
    if (FAILED(hr))
        return Result(
            false, this,
            std::string(__func__) +
            ": Could not create input layout. - " + GetErrorMessage(hr));

    // Set the input layout ****
    m_pDeviceContext->IASetInputLayout(m_pInputLayout.Get());

    return Result(true, this);
}

Result Graphics::CreatePixelShader()
{
    HRESULT hr = S_OK;
    ID3DBlob* pPSBlob = nullptr;

    //Result result = CompileShaderFromFile(L"Shaders\\Shader.hlsl", "PS", "ps_4_0", &pPSBlob);
    Result result = CompileShaderFromFile(L"Shader.fx", "PS", "ps_4_0", &pPSBlob);
    if (!result.IsSuccess())
        return result;

    hr = m_pDevice->CreatePixelShader(
        pPSBlob->GetBufferPointer(),
        pPSBlob->GetBufferSize(),
        0,
        m_pPixelShader.GetAddressOf());
    pPSBlob->Release();
    if (FAILED(hr))
        return Result(
            false, this,
            std::string(__func__) +
            ": Could not create pixel shader. - " + GetErrorMessage(hr));

    return Result(true, this);
}

Result Graphics::CreateConstantBuffers()
{
    HRESULT hr = S_OK;
    D3D11_BUFFER_DESC bd = {};

    // Create the light constant buffer
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(LightPropertiesConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    hr = m_pDevice->CreateBuffer(&bd, 0, m_pLightConstantBuffer.GetAddressOf());
    if (FAILED(hr))
        return Result(
            false, this,
            std::string(__func__) +
            ": Could not create light constant buffer. - " + GetErrorMessage(hr));

    // Create the constant buffer
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(ConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    hr = m_pDevice->CreateBuffer(&bd, 0, m_pConstantBuffer.GetAddressOf());
    if (FAILED(hr))
        return Result(
            false, this,
            std::string(__func__) +
            ": Could not create constant buffer. - " + GetErrorMessage(hr));

    // Create the material constant buffer
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(MaterialPropertiesConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    hr = m_pDevice->CreateBuffer(&bd, 0, m_pMaterialConstantBuffer.GetAddressOf());
    if (FAILED(hr))
        return Result(
            false, this,
            std::string(__func__) +
            ": Could not create material constant buffer. - " + GetErrorMessage(hr));

    return Result(true, this);
}

void Graphics::SetupLightingConstantBuffer()
{
    //XMFLOAT4 LightPosition(g_EyePosition);

    Light light;
    light.Enabled = static_cast<int>(true);
    light.LightType = PointLight;
    light.Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    light.SpotAngle = XMConvertToRadians(45.0f); // never used
    light.ConstantAttenuation = 0.005f;
    light.LinearAttenuation = 0.0014f;
    light.QuadraticAttenuation = 0.000007f;

    //light.Position = LightPosition;
    light.Position = m_pCamera->GetPosition();
    //XMVECTOR LightDirection = XMVectorSet(-LightPosition.x, -LightPosition.y, -LightPosition.z, 0.0f);
    //LightDirection = XMVector3Normalize(LightDirection);
    //XMStoreFloat4(&light.Direction, LightDirection);

    LightPropertiesConstantBuffer lightProperties;
    lightProperties.EyePosition = m_pCamera->GetPosition();
    lightProperties.Lights[0] = light;
    //g_pImmediateContext->UpdateSubresource(g_pLightConstantBuffer, 0, nullptr, &lightProperties, 0, 0);
    m_pDeviceContext->UpdateSubresource(m_pLightConstantBuffer.Get(), 0, 0, &lightProperties, 0, 0);
}

void Graphics::SetupMaterialConstantBuffer(MaterialPropertiesConstantBuffer material)
{
    m_pDeviceContext->UpdateSubresource(
        m_pMaterialConstantBuffer.Get(),
        0,
        0,
        &material,
        0,
        0);
}

void Graphics::SetupTransformConstantBuffer(XMFLOAT4X4* transform)
{
    ConstantBuffer cb1;
    cb1.mWorld = XMMatrixTranspose(XMLoadFloat4x4(transform));
    cb1.mView = XMMatrixTranspose(XMLoadFloat4x4(m_pCamera->GetView()));
    cb1.mProjection = XMMatrixTranspose(XMLoadFloat4x4(m_pCamera->GetProjection()));
    cb1.vOutputColor = XMFLOAT4(0, 0, 0, 0);
    m_pDeviceContext->UpdateSubresource(m_pConstantBuffer.Get(), 0, 0, &cb1, 0, 0);
}

void Graphics::SetupConstantBuffers(ID3D11ShaderResourceView* pShaderResource, ID3D11SamplerState* pSamplerState)
{
    m_pDeviceContext->VSSetShader(m_pVertexShader.Get(), 0, 0);
    m_pDeviceContext->VSSetConstantBuffers(0, 1, m_pConstantBuffer.GetAddressOf());

    m_pDeviceContext->PSSetShader(m_pPixelShader.Get(), 0, 0);
    m_pDeviceContext->PSSetConstantBuffers(1, 1, m_pMaterialConstantBuffer.GetAddressOf());
    m_pDeviceContext->PSSetConstantBuffers(2, 1, m_pLightConstantBuffer.GetAddressOf());

    m_pDeviceContext->PSSetShaderResources(0, 1, &pShaderResource);
    m_pDeviceContext->PSSetSamplers(0, 1, &pSamplerState);
}

Result Graphics::CreateVertexBuffer()
{
    // Define the vertices for the buffer
    // Example: Create a triangle
    struct Vertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT4 color;
    };

    Vertex vertices[] =
    {
        { DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
        { DirectX::XMFLOAT3(1.0f, -1.0f, 0.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
        { DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) }
    };

    // Create the vertex buffer
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(vertices);
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = vertices;

    HRESULT hr = m_pDevice->CreateBuffer(&bufferDesc, &initData, m_pVertexBuffer.GetAddressOf());
    if (FAILED(hr))
        return Result(
            false, this,
            std::string(__func__) +
            ": Could not create vertex buffer. - " + GetErrorMessage(hr));

    // Set the vertex buffer
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    m_pDeviceContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);

    return Result(true, this);
}

Result Graphics::CompileShaderFromFile(
    const WCHAR*    szFileName, 
    LPCSTR          szEntryPoint, 
    LPCSTR          szShaderModel, 
    ID3DBlob**      ppBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;

    // Disable optimizations to further improve shader debugging
    dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ID3DBlob* pErrorBlob = 0;
    hr = D3DCompileFromFile(
        szFileName,
        0,
        0,
        szEntryPoint,
        szShaderModel,
        dwShaderFlags,
        0,
        ppBlobOut,
        &pErrorBlob);

    if (FAILED(hr))
    {
        if (pErrorBlob)
        {
            OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
            pErrorBlob->Release();
        }
        return Result(
            false, this,
            std::string(__func__) +
            ": Could not load from file.\n" + GetErrorMessage(hr));
    }
    if (pErrorBlob) pErrorBlob->Release();

    return Result(true, this);
}

wrl::ComPtr<ID3D11DeviceContext> Graphics::GetContext() const
{
    return m_pDeviceContext;
}

wrl::ComPtr<ID3D11Device> Graphics::GetDevice() const
{
    return m_pDevice;
}

XMMATRIX Graphics::GetViewMatrix() const
{
    return m_pCamera->GetViewMatrix();
}

XMMATRIX Graphics::GetProjectionMatrix() const
{
    return m_pCamera->GetProjectionMatrix();
}

std::string Graphics::GetErrorMessage(const HRESULT hr)
{
    LPSTR messageBuffer = nullptr;
    const DWORD FormatMessageOptions =
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS;

    size_t size = FormatMessageA(
        FormatMessageOptions,
        NULL,
        hr,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPSTR>(&messageBuffer),
        0,
        NULL);

    std::string errorMessage = size > 0
        ? messageBuffer
        : "Unknown error";

    LocalFree(messageBuffer);

    return errorMessage;
}
