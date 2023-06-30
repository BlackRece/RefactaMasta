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
// Forward declarations
//--------------------------------------------------------------------------------------
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
std::unique_ptr<Graphics> g_pGraphics;

ID3D11VertexShader*     g_pVertexShader = nullptr;
ID3D11PixelShader*      g_pPixelShader = nullptr;
ID3D11InputLayout*		g_pInputLayout = nullptr;

ID3D11Buffer*           g_pConstantBuffer = nullptr;
ID3D11Buffer*           g_pMaterialConstantBuffer = nullptr;
ID3D11Buffer*           g_pLightConstantBuffer = nullptr;

XMMATRIX                g_View;
XMMATRIX                g_Projection;

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

    hr = fish->initMesh(g_pGraphics->GetDevice(), g_pGraphics->GetContext());
    
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

    int amount = 10;// 100;
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

    g_pGraphics = std::make_unique<Graphics>(g_pAppWindow->GetHandle());
    g_pGraphics->Initialise(g_pAppWindow->GetWidth(), g_pAppWindow->GetHeight());

    placeFish();

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
    //g_pImmediateContext->OMSetRenderTargets(_countof(nullViews), nullViews, nullptr);
    g_pGraphics->GetContext()->OMSetRenderTargets(_countof(nullViews), nullViews, nullptr);


    //if( g_pImmediateContext ) g_pImmediateContext->ClearState();
    if (g_pGraphics->GetContext()) g_pGraphics->GetContext()->ClearState();

    // Flush the immediate context to force cleanup
    //if (g_pImmediateContext1) g_pImmediateContext1->Flush();
    //g_pImmediateContext->Flush();

	if (g_pInputLayout) g_pInputLayout->Release();
    if (g_pLightConstantBuffer) g_pLightConstantBuffer->Release();
    if (g_pMaterialConstantBuffer) g_pMaterialConstantBuffer->Release();

    if( g_pConstantBuffer ) g_pConstantBuffer->Release();
    if( g_pVertexShader ) g_pVertexShader->Release();
    if( g_pPixelShader ) g_pPixelShader->Release();
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
    
    g_pGraphics->BeginDraw();

    Boid::updateStats(t);

	for(unsigned int i=0; i< g_Boids.size(); i++)
	{ 
        g_Boids[i]->setSeperationMultiplier(g_seperation);
        g_Boids[i]->setAlignmentMultiplier(g_alignment);
        g_Boids[i]->setCohesionMultiplier(g_cohesion);
        g_Boids[i]->setVelocityMultiplier(g_velocity);
		
		g_Boids[i]->update(t, &g_Boids);
        
        g_View = g_pGraphics->GetViewMatrix();
        g_Projection = g_pGraphics->GetProjectionMatrix();
		XMMATRIX vp = g_View * g_Projection;
		//Boid* dob = (Boid*)g_Boids[i];

        g_Boids[i]->checkIsOnScreenAndFix(g_View, g_Projection);

        g_pGraphics->SetupTransformConstantBuffer(g_Boids[i]->getTransform());
        g_pGraphics->SetupLightingConstantBuffer();
        g_pGraphics->SetupMaterialConstantBuffer(g_Boids[i]->getMaterial());

		// Render a cube
        g_pGraphics->SetupConstantBuffers(
            *g_Boids[i]->getTextureResourceView(), 
            *g_Boids[i]->getTextureSamplerState());

		// draw 
        g_Boids[i]->draw(g_pGraphics->GetContext().Get());
	}
	
    RenderImGui();

    // Present our back buffer to our front buffer
    g_pGraphics->EndDraw();
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
    ImGui_ImplWin32_Init(g_pAppWindow->GetHandle());
    ImGui_ImplDX11_Init(
        g_pGraphics->GetDevice().Get(),
        g_pGraphics->GetContext().Get());
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

    /*for(int i = 0; i < stats.stats.size(); i++)
    {
        msg = 
            std::to_string(i) +
            ": " +
            std::to_string(stats.stats[i]);
        ImGui::Text(msg.c_str());
    }*/

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

