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
//--------------------------------------------------------------------------------------
#define _XM_NO_INTRINSICS_

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

//#include "main.h"

#include "AppEngine.h"

#include <string>

//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
LRESULT CALLBACK        WndProc(HWND, UINT, WPARAM, LPARAM);
void		            OutputValue(float f, string name);

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------

wchar_t                     g_sAppTitle[100] = L"Refactored Boids";

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow )
{
    UNREFERENCED_PARAMETER( hPrevInstance );
    UNREFERENCED_PARAMETER( lpCmdLine );

    AppEngine::AppEngineParams params;
    params.pTitle = g_sAppTitle;
    params.nWidth = WINDOW_WIDTH;
    params.nHeight = WINDOW_HEIGHT;
    params.nCmdShow = nCmdShow;
    params.hInstance = hInstance;

    std::unique_ptr<AppEngine> pAppEngine = std::make_unique<AppEngine>(params);
    pAppEngine->Start();
    pAppEngine->Run();
    pAppEngine->Stop();

    return 0;
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

void OutputValue(float f, string name)
{
	char sz[1024] = { 0 };
	sprintf_s(sz, "%s: %f\n", name.c_str(), f);
	OutputDebugStringA(sz);
}
