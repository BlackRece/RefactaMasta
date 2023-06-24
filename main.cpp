//--------------------------------------------------------------------------------------
// File: main.cpp
//
// This application demonstrates animation using matrix transformations
//
// http://msdn.microsoft.com/en-us/library/windows/apps/ff729722.aspx
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#define _XM_NO_INTRINSICS_

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

#define SEPERATION_MULTIPLIER	5.0f    //1.0f//12.0f
#define ALIGNMENT_MULTPLIER		10.0f   //1.0f//8.0f
#define COHESION_MULTIPLIER		5.0f    //1.0f//8.0f

#define VELOCITY_MULTIPLIER		5.0f

#include "main.h"

#include <time.h>
#include "Boid.h"

#include "ImGui.h"
#include <string>


//--------------------------------------------------------------------------------------
// eye position
//--------------------------------------------------------------------------------------
DirectX::XMFLOAT4 g_EyePosition(0.0f, 0, -200, 1.0f);

//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT 		        InitWindow(HINSTANCE hInstance, int nCmdShow);
HRESULT	    	        InitDevice();
HRESULT		            InitMesh();
HRESULT		            InitWorld(int width, int height);
void		            CleanupDevice();
LRESULT CALLBACK        WndProc(HWND, UINT, WPARAM, LPARAM);
void		            Render();
void		            OutputValue(float f, string name);

void                    InitImGui();
void                    RenderImGui();
void                    CleanupImGui();

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------

wchar_t                 g_sAppTitle[100] = L"Refactored Boids";
std::unique_ptr<AppWindow> g_pAppWindow;

D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL       g_featureLevel = D3D_FEATURE_LEVEL_11_0;
ID3D11Device*           g_pd3dDevice = nullptr;
ID3D11Device1*          g_pd3dDevice1 = nullptr;
ID3D11DeviceContext*    g_pImmediateContext = nullptr;
ID3D11DeviceContext1*   g_pImmediateContext1 = nullptr;
IDXGISwapChain*         g_pSwapChain = nullptr;
IDXGISwapChain1*        g_pSwapChain1 = nullptr;
ID3D11RenderTargetView* g_pRenderTargetView = nullptr;
ID3D11Texture2D*        g_pDepthStencil = nullptr;
ID3D11DepthStencilView* g_pDepthStencilView = nullptr;

ID3D11VertexShader*     g_pVertexShader = nullptr;
ID3D11PixelShader*      g_pPixelShader = nullptr;
ID3D11SamplerState *	g_pSamplerNormal = nullptr;
ID3D11InputLayout*		g_pInputLayout = nullptr;

ID3D11Buffer*           g_pConstantBuffer = nullptr;
ID3D11Buffer*           g_pMaterialConstantBuffer = nullptr;
ID3D11Buffer*           g_pLightConstantBuffer = nullptr;

XMMATRIX                g_View;
XMMATRIX                g_Projection;

int						g_viewWidth;
int						g_viewHeight;

vecBoid					g_Boids;

constexpr auto          g_PI = 3.14159265358979323846;

static float            g_seperation;
static float            g_alignment;
static float            g_cohesion;
static float            g_velocity;


Boid* createFish(XMFLOAT3 position, bool shark)
{
    HRESULT hr;

    Boid* fish = new Boid(position);

    fish->setScale(1.0f);
    if (shark)
    {
        fish->setScale(3.0f);
        fish->setAmbientColour(XMFLOAT4(255.0f, 0.0f, 0.0f, 1.0f));
    }

    hr = fish->initMesh(g_pd3dDevice, g_pImmediateContext);
    
    if (FAILED(hr))
        return fish;

    g_Boids.push_back(fish);
    return fish;
}

void squareFormation(int amount)
{
    XMFLOAT3 position;
    int perLine = round(sqrt(amount));

    for (int x = 0; x < perLine; x++)
    {
        for (int y = 0; y < perLine; y++)
        {
            position = XMFLOAT3(x * perLine, y * perLine, 0);
			if (x == 0 && y == 0)
                Boid* fish = createFish(position, true);
            else
				Boid* fish = createFish(position, false);
        }
    }
}

void circleFormation(int radius, int amount)
{
    int tmpX, tmpY;
    float degrees = amount / (2 * g_PI);

    for (int point = 0; point < amount; point++) {
        float angle = point * degrees;

        tmpX = radius * cos(angle * g_PI);
        tmpY = radius * sin(angle * g_PI);
    
		if(point == 0)
            Boid* fish = createFish(XMFLOAT3(tmpX, tmpY, 0), true);
        else
            Boid* fish = createFish(XMFLOAT3(tmpX, tmpY, 0), false);
    }
}

void spiralFormation(int coils, int radius, int rotation)
{
    // value of theta corresponding to end of last coil
    double thetaMax = coils * 2 * g_PI;

    // How far to step away from center for each side.
    double awayStep = radius / thetaMax;

    // distance between points to plot
    double chord = 10;

    int centerX, centerY;
    centerX = centerY = 0;
    
    Boid* midFish = createFish(XMFLOAT3(centerX, centerY, 0), true);

    // For every side, step around and away from center.
    // start at the angle corresponding to a distance of chord
    // away from centre.
    for (double theta = chord / awayStep; theta <= thetaMax; ) {
        //
        // How far away from center
        double away = awayStep * theta;
        //
        // How far around the center.
        double around = theta + rotation;
        //
        // Convert 'around' and 'away' to X and Y.
        double x = centerX + cos(around) * away;
        double y = centerY + sin(around) * away;
        //
        // Now that you know it, do it.
        if(centerX == x && centerY == y)
            Boid* fish = createFish(XMFLOAT3(x, y, 0), true);
        else
            Boid* fish = createFish(XMFLOAT3(x, y, 0), false);

        // to a first approximation, the points are on a circle
        // so the angle between them is chord/radius
        theta += chord / away;
    }
}

void placeFish()
{
    g_seperation = SEPERATION_MULTIPLIER;
    g_alignment = ALIGNMENT_MULTPLIER;
    g_cohesion = COHESION_MULTIPLIER;

    g_velocity = VELOCITY_MULTIPLIER;

    int amount = 100;
    int radius = 100;

    squareFormation(amount);
    //circleFormation(radius, amount);
    //spiralFormation(10, radius, 10);
}

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow )
{
    UNREFERENCED_PARAMETER( hPrevInstance );
    UNREFERENCED_PARAMETER( lpCmdLine );

    AppWindow::AppWindowParams params;
    params.pTitle = g_sAppTitle;
    params.nWidth = WINDOW_WIDTH;
    params.nHeight = WINDOW_HEIGHT;
    params.nCmdShow = nCmdShow;

    g_pAppWindow = std::make_unique<AppWindow>(hInstance, params);

    /*if( FAILED( InitWindow( hInstance, nCmdShow ) ) )
        return 0;*/

    if( FAILED( InitDevice() ) )
    {
        CleanupDevice();
        return 0;
    }

    InitImGui();

    // Main message loop
    MSG msg = {0};
    while( WM_QUIT != msg.message )
    {
        if( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
            Render();
        }
    }
	
    CleanupImGui();
    CleanupDevice();

    return ( int )msg.wParam;
}

//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DCompile
//
// With VS 11, we could load up prebuilt .cso files instead...
//--------------------------------------------------------------------------------------
HRESULT     CompileShaderFromFile( const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut )
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

    ID3DBlob* pErrorBlob = nullptr;
    hr = D3DCompileFromFile( szFileName, nullptr, nullptr, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob );
    if( FAILED(hr) )
    {
        if( pErrorBlob )
        {
            OutputDebugStringA( reinterpret_cast<const char*>( pErrorBlob->GetBufferPointer() ) );
            pErrorBlob->Release();
        }
        return hr;
    }
    if( pErrorBlob ) pErrorBlob->Release();

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
HRESULT     InitDevice()
{
    HRESULT hr = S_OK;

    RECT rc;
    //GetClientRect( g_hWnd, &rc );
    GetClientRect(g_pAppWindow->GetHandle(), &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

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
    UINT numDriverTypes = ARRAYSIZE( driverTypes );

	D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE( featureLevels );

    for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
    {
        g_driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDevice( nullptr, g_driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                                D3D11_SDK_VERSION, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext );

        if ( hr == E_INVALIDARG )
        {
            // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
            hr = D3D11CreateDevice( nullptr, g_driverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
                                    D3D11_SDK_VERSION, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext );
        }

        if( SUCCEEDED( hr ) )
            break;
    }
    if( FAILED( hr ) )
        return hr;

    // Obtain DXGI factory from device (since we used nullptr for pAdapter above)
    IDXGIFactory1* dxgiFactory = nullptr;
    {
        IDXGIDevice* dxgiDevice = nullptr;
        hr = g_pd3dDevice->QueryInterface( __uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice) );
        if (SUCCEEDED(hr))
        {
            IDXGIAdapter* adapter = nullptr;
            hr = dxgiDevice->GetAdapter(&adapter);
            if (SUCCEEDED(hr))
            {
                hr = adapter->GetParent( __uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory) );
                adapter->Release();
            }
            dxgiDevice->Release();
        }
    }
    if (FAILED(hr))
        return hr;

    // Create swap chain
    IDXGIFactory2* dxgiFactory2 = nullptr;
    hr = dxgiFactory->QueryInterface( __uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory2) );
    if ( dxgiFactory2 )
    {
        // DirectX 11.1 or later
        hr = g_pd3dDevice->QueryInterface( __uuidof(ID3D11Device1), reinterpret_cast<void**>(&g_pd3dDevice1) );
        if (SUCCEEDED(hr))
        {
            (void) g_pImmediateContext->QueryInterface( __uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&g_pImmediateContext1) );
        }

        DXGI_SWAP_CHAIN_DESC1 sd = {};
        //sd.Width = width;
        //sd.Height = height;
        sd.Width = g_pAppWindow->GetWidth();
        sd.Height = g_pAppWindow->GetHeight();
        sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = 1;

        //hr = dxgiFactory2->CreateSwapChainForHwnd( 
        //    g_pd3dDevice, 
        //    g_hWnd, 
        //    &sd, 
        //    0, 
        //    0,
        //    &g_pSwapChain1 );

        hr = dxgiFactory2->CreateSwapChainForHwnd(
            g_pd3dDevice,
            g_pAppWindow->GetHandle(),
            &sd,
            0,
            0,
            &g_pSwapChain1);
        if (SUCCEEDED(hr))
        {
            hr = g_pSwapChain1->QueryInterface( __uuidof(IDXGISwapChain), reinterpret_cast<void**>(&g_pSwapChain) );
        }

        dxgiFactory2->Release();
    }
    else
    {
        // DirectX 11.0 systems
        DXGI_SWAP_CHAIN_DESC sd = {};
        sd.BufferCount = 1;
        //sd.BufferDesc.Width = width;
        //sd.BufferDesc.Height = height;
        sd.BufferDesc.Width = g_pAppWindow->GetWidth();
        sd.BufferDesc.Height = g_pAppWindow->GetHeight();
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        //sd.OutputWindow = g_hWnd;
        sd.OutputWindow = g_pAppWindow->GetHandle();
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;

        hr = dxgiFactory->CreateSwapChain( g_pd3dDevice, &sd, &g_pSwapChain );
    }

    // Note this tutorial doesn't handle full-screen swapchains so we block the ALT+ENTER shortcut
    //dxgiFactory->MakeWindowAssociation( g_hWnd, DXGI_MWA_NO_ALT_ENTER );
    dxgiFactory->MakeWindowAssociation(
        g_pAppWindow->GetHandle(), 
        DXGI_MWA_NO_ALT_ENTER);

    dxgiFactory->Release();

    if (FAILED(hr))
        return hr;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = g_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast<void**>( &pBackBuffer ) );
    if( FAILED( hr ) )
        return hr;

    hr = g_pd3dDevice->CreateRenderTargetView( pBackBuffer, nullptr, &g_pRenderTargetView );
    pBackBuffer->Release();
    if( FAILED( hr ) )
        return hr;

    // Create depth stencil texture
    D3D11_TEXTURE2D_DESC descDepth = {};
    //descDepth.Width = width;
    //descDepth.Height = height;
    descDepth.Width = g_pAppWindow->GetWidth();
    descDepth.Height = g_pAppWindow->GetHeight();
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    hr = g_pd3dDevice->CreateTexture2D( &descDepth, 0, &g_pDepthStencil );
    if( FAILED( hr ) )
        return hr;

    // Create the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = g_pd3dDevice->CreateDepthStencilView( g_pDepthStencil, &descDSV, &g_pDepthStencilView );
    if( FAILED( hr ) )
        return hr;

    g_pImmediateContext->OMSetRenderTargets( 1, &g_pRenderTargetView, g_pDepthStencilView );

    // Setup the viewport
    D3D11_VIEWPORT vp;
    //vp.Width = (FLOAT)width;
    //vp.Height = (FLOAT)height;
    vp.Width = (FLOAT)g_pAppWindow->GetWidth();
    vp.Height = (FLOAT)g_pAppWindow->GetHeight();
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    g_pImmediateContext->RSSetViewports( 1, &vp );

	hr = InitMesh();
	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"Failed to initialise mesh.", L"Error", MB_OK);
		return hr;
	}

	hr = InitWorld(width, height);
	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"Failed to initialise world.", L"Error", MB_OK);
		return hr;
	}


    return S_OK;
}

// ***************************************************************************************
// InitMesh
// ***************************************************************************************
HRESULT		InitMesh()
{
	// Compile the vertex shader
	ID3DBlob* pVSBlob = nullptr;
	HRESULT hr = CompileShaderFromFile(L"shader.fx", "VS", "vs_4_0", &pVSBlob);
	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the vertex shader
	hr = g_pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &g_pVertexShader);
	if (FAILED(hr))
	{
		pVSBlob->Release();
		return hr;
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
	hr = g_pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &g_pInputLayout);
	pVSBlob->Release();
	if (FAILED(hr))
		return hr;

	// Set the input layout ****
	g_pImmediateContext->IASetInputLayout(g_pInputLayout);

	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
	hr = CompileShaderFromFile(L"shader.fx", "PS", "ps_4_0", &pPSBlob);
	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the pixel shader
	hr = g_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &g_pPixelShader);
	pPSBlob->Release();
	if (FAILED(hr))
		return hr;

	D3D11_BUFFER_DESC bd = {};

	// Create the light constant buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(LightPropertiesConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = g_pd3dDevice->CreateBuffer(&bd, nullptr, &g_pLightConstantBuffer);
	if (FAILED(hr))
		return hr;

	// Create the constant buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = g_pd3dDevice->CreateBuffer(&bd, nullptr, &g_pConstantBuffer);
	if (FAILED(hr))
		return hr;

	// Create the material constant buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(MaterialPropertiesConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = g_pd3dDevice->CreateBuffer(&bd, nullptr, &g_pMaterialConstantBuffer);
	if (FAILED(hr))
		return hr;


	placeFish();

	return hr;
}

// ***************************************************************************************
// InitWorld
// ***************************************************************************************
HRESULT		InitWorld(int width, int height)
{
	// Initialize the view matrix
	XMVECTOR Eye = XMLoadFloat4(&g_EyePosition);
	XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	g_View = XMMatrixLookAtLH(Eye, At, Up);

	// Initialize the projection matrix
	g_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, width / (FLOAT)height, 0.01f, 1000.0f);


	return S_OK;
}


//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void        CleanupDevice()
{
	for (unsigned int i = 0; i < g_Boids.size(); i++)
	{
		delete g_Boids[i];
	}

	g_Boids.clear();

    // Remove any bound render target or depth/stencil buffer
    ID3D11RenderTargetView* nullViews[] = { nullptr };
    g_pImmediateContext->OMSetRenderTargets(_countof(nullViews), nullViews, nullptr);


    if( g_pImmediateContext ) g_pImmediateContext->ClearState();

    // Flush the immediate context to force cleanup
    if (g_pImmediateContext1) g_pImmediateContext1->Flush();
    g_pImmediateContext->Flush();

	if (g_pInputLayout) g_pInputLayout->Release();
    if (g_pLightConstantBuffer) g_pLightConstantBuffer->Release();
    if (g_pMaterialConstantBuffer) g_pMaterialConstantBuffer->Release();

    if( g_pConstantBuffer ) g_pConstantBuffer->Release();
    if( g_pVertexShader ) g_pVertexShader->Release();
    if( g_pPixelShader ) g_pPixelShader->Release();
    if( g_pDepthStencil ) g_pDepthStencil->Release();
    if( g_pDepthStencilView ) g_pDepthStencilView->Release();
    if( g_pRenderTargetView ) g_pRenderTargetView->Release();
    if( g_pSwapChain1 ) g_pSwapChain1->Release();
    if( g_pSwapChain ) g_pSwapChain->Release();
    if( g_pImmediateContext1 ) g_pImmediateContext1->Release();
    if( g_pImmediateContext ) g_pImmediateContext->Release();


    ID3D11Debug* debugDevice = nullptr;
    g_pd3dDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&debugDevice));



    // handy for finding dx memory leaks
   // debugDevice->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);

    if (debugDevice)
        debugDevice->Release();

    if (g_pd3dDevice1) g_pd3dDevice1->Release();
    if (g_pd3dDevice) g_pd3dDevice->Release();
}

//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT ps;
    HDC hdc;

    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
        return true;

    switch( message )
    {
	case WM_LBUTTONDOWN:
	{
		//int xPos = GET_X_LPARAM(lParam);
		//int yPos = GET_Y_LPARAM(lParam);
		break;
	}
    case WM_PAINT:
        hdc = BeginPaint( hWnd, &ps );
        EndPaint( hWnd, &ps );
        break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        break;

        // Note that this tutorial does not handle resizing (WM_SIZE) requests,
        // so we created the window without the resize border.

    default:
        return DefWindowProc( hWnd, message, wParam, lParam );
    }

    return 0;
}


void setupLightingConstantBuffer()
{
	XMFLOAT4 LightPosition(g_EyePosition);

	Light light;
	light.Enabled = static_cast<int>(true);
	light.LightType = PointLight;
	light.Color = XMFLOAT4(Colors::White);
	light.SpotAngle = XMConvertToRadians(45.0f); // never used
	light.ConstantAttenuation = 0.005f;
	light.LinearAttenuation =  0.0014f;
	light.QuadraticAttenuation = 0.000007f;

	light.Position = LightPosition;
	//XMVECTOR LightDirection = XMVectorSet(-LightPosition.x, -LightPosition.y, -LightPosition.z, 0.0f);
	//LightDirection = XMVector3Normalize(LightDirection);
	//XMStoreFloat4(&light.Direction, LightDirection);

	LightPropertiesConstantBuffer lightProperties;
	lightProperties.EyePosition = LightPosition;
	lightProperties.Lights[0] = light;
	g_pImmediateContext->UpdateSubresource(g_pLightConstantBuffer, 0, nullptr, &lightProperties, 0, 0);
}

void setupMaterialConstantBuffer(const unsigned int index)
{
	MaterialPropertiesConstantBuffer mcb = g_Boids[index]->getMaterial();
	g_pImmediateContext->UpdateSubresource(g_pMaterialConstantBuffer, 0, nullptr, &mcb, 0, 0);
}

void setupTransformConstantBuffer(const unsigned int index)
{
	ConstantBuffer cb1;
	cb1.mWorld = XMMatrixTranspose(XMLoadFloat4x4(g_Boids[index]->getTransform()));
	cb1.mView = XMMatrixTranspose(g_View);
	cb1.mProjection = XMMatrixTranspose(g_Projection);
	cb1.vOutputColor = XMFLOAT4(0, 0, 0, 0);
	g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, nullptr, &cb1, 0, 0);
}

//--------------------------------------------------------------------------------------
// Render a frame
//--------------------------------------------------------------------------------------
void Render()
{
    // Update our time
    static float t = 0.0f;
    static ULONGLONG timeStart = 0;
    ULONGLONG timeCur = GetTickCount64();
    if( timeStart == 0 )
        timeStart = timeCur;
    t = ( timeCur - timeStart ) / 1000.0f;
	timeStart = timeCur;

	float FPS60 = 1.0f / 60.0f;
	static float cummulativeTime = 0;

	// cap the framerate at 60 fps 
	cummulativeTime += t;
	if (cummulativeTime >= FPS60) {
		cummulativeTime = cummulativeTime - FPS60;
	}
	else {
		return;
	}
    
    // Clear the back buffer
    g_pImmediateContext->ClearRenderTargetView( g_pRenderTargetView, Colors::AntiqueWhite );

    // Clear the depth buffer to 1.0 (max depth)
    g_pImmediateContext->ClearDepthStencilView( g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );

    Boid::updateStats(t);

	for(unsigned int i=0; i< g_Boids.size(); i++)
	{ 
        g_Boids[i]->setSeperationMultiplier(g_seperation);
        g_Boids[i]->setAlignmentMultiplier(g_alignment);
        g_Boids[i]->setCohesionMultiplier(g_cohesion);
        g_Boids[i]->setVelocityMultiplier(g_velocity);
		
		g_Boids[i]->update(t, &g_Boids);
		XMMATRIX vp = g_View * g_Projection;
		//Boid* dob = (Boid*)g_Boids[i];

		//dob->checkIsOnScreenAndFix(g_View, g_Projection);
        g_Boids[i]->checkIsOnScreenAndFix(g_View, g_Projection);

		setupTransformConstantBuffer(i);
		setupLightingConstantBuffer();
		setupMaterialConstantBuffer(i);

		// Render a cube
		g_pImmediateContext->VSSetShader( g_pVertexShader, nullptr, 0 );
		g_pImmediateContext->VSSetConstantBuffers( 0, 1, &g_pConstantBuffer );
	
		g_pImmediateContext->PSSetShader( g_pPixelShader, nullptr, 0 );
		g_pImmediateContext->PSSetConstantBuffers(1, 1, &g_pMaterialConstantBuffer);
		g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pLightConstantBuffer);

		g_pImmediateContext->PSSetShaderResources(0, 1, g_Boids[i]->getTextureResourceView() );
		g_pImmediateContext->PSSetSamplers(0, 1, g_Boids[i]->getTextureSamplerState());

		// draw 
		g_Boids[i]->draw(g_pImmediateContext);
	}
	
    RenderImGui();

    // Present our back buffer to our front buffer
    g_pSwapChain->Present( 0, 0 );
}

void OutputValue(float f, string name)
{
	char sz[1024] = { 0 };
	sprintf_s(sz, "%s: %f\n", name.c_str(), f);
	OutputDebugStringA(sz);
}

//--------------------------------------------------------------------------------------
// ImGui Implementation Methods
//--------------------------------------------------------------------------------------

void InitImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();
    //ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplWin32_Init(g_pAppWindow->GetHandle());
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pImmediateContext);
}

void RenderImGui()
{
    const float vMin = -1.0f;
    const float vMax = 2.0f;

    static int i_boidAmount = 100;
    static int i_radius = 100;
    static int i_coils = 10;
    static int i_rotation = 10;

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	
    ImGui::Begin("_.- Boid Options -._");
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::Text("Boids: %d", g_Boids.size());
    ImGui::NewLine();
    ImGui::InputInt("Boid Amount", &i_boidAmount);
    ImGui::InputInt("Shape Radius", &i_radius);
    ImGui::InputInt("Rotation", &i_rotation);
    ImGui::InputInt("Coils", &i_coils);
    if (ImGui::Button("Square")) 
    {
        g_Boids.clear();
        squareFormation(i_boidAmount);
    }
    if (ImGui::Button("Circle"))
    {
        g_Boids.clear();
        circleFormation(i_radius, i_boidAmount);
    }
    if (ImGui::Button("Spiral"))
    {
        g_Boids.clear();
        spiralFormation(i_coils, i_radius, i_rotation);
    }
    ImGui::NewLine();
    ImGui::SliderFloat("Seperation Multiplier", &g_seperation, vMin, vMax);
    ImGui::InputFloat("Seperation:", &g_seperation, vMin, vMax, "%.4f");
    ImGui::SliderFloat("Alignment Multiplier", &g_alignment, vMin, vMax);
    ImGui::InputFloat("Alignment :", &g_alignment, vMin, vMax, "%.4f");
    ImGui::SliderFloat("Cohesion Multiplier", &g_cohesion, vMin, vMax);
    ImGui::InputFloat("Cohesion:", &g_cohesion, vMin, vMax, "%.4f");
    ImGui::NewLine();
    ImGui::SliderFloat("Velocity Multiplier", &g_velocity, vMin, vMax);
    ImGui::InputFloat("Velocity:", &g_velocity, vMin, vMax, "%.4f");
    ImGui::NewLine();

    BoidData stats = Boid::getStats();
    std::string msg =
        "Time Elapsed: " +
        std::to_string(stats.fTotalTime);
    ImGui::Text(msg.c_str());
    
    msg =
        "Captured: " +
        std::to_string(stats.stats.size());
    ImGui::Text(msg.c_str());

    msg = 
        "Average Bite Time: " +
        std::to_string(stats.GetAverage());
    ImGui::Text(msg.c_str());

    for(int i = 0; i < stats.stats.size(); i++)
    {
        msg = 
            std::to_string(i) +
            ": " +
            std::to_string(stats.stats[i]);
        ImGui::Text(msg.c_str());
    }

    ImGui::NewLine();
    ImGui::End();

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CleanupImGui()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

