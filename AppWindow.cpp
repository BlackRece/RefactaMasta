#include "AppWindow.h"

const wchar_t* AppWindow::m_sClassName = L"AppWindowClass";

AppWindow::AppWindow(HINSTANCE hInstance, AppWindowParams& params) :
    m_hInstance(hInstance), m_sAppTitle(params.pTitle),
    m_nWidith(params.nWidth), m_nHeight(params.nHeight),
    m_hClass(0), m_hWnd(0)
{
    CreateClass();
	CreateGameWindow(params.nWidth, params.nHeight, params.nCmdShow);
}

AppWindow::~AppWindow()
{
    DestroyGameWindow();
	DestroyClass();
}

HWND AppWindow::GetHandle() const
{
	return m_hWnd;
}

int AppWindow::GetWidth() const
{
	return m_nWidith;
}

int AppWindow::GetHeight() const
{
	return m_nHeight;
}

void AppWindow::CreateClass()
{
    WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(WNDCLASSEX));

	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= AppWindow::MsgCallBack;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= m_hInstance;
	wcex.hIcon			= LoadIcon(m_hInstance, (LPCTSTR)IDI_TUTORIAL1);
	wcex.hIconSm		= LoadIcon(m_hInstance, (LPCTSTR)IDI_TUTORIAL1);
	wcex.hCursor		= LoadCursor(m_hInstance, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= m_sClassName;

	m_hClass = RegisterClassEx(&wcex);
}

void AppWindow::DestroyClass()
{
}

void AppWindow::CreateGameWindow(int nWidth, int nHeight, int nCmdShow)
{
	DWORD dwStyle =
		WS_OVERLAPPED |
		WS_CAPTION |
		WS_SYSMENU |
		WS_MINIMIZEBOX;
	DWORD dwExStyle =
		WS_EX_WINDOWEDGE |
		WS_EX_APPWINDOW;

	RECT windowRect;
	ZeroMemory(&windowRect, sizeof(windowRect));

	windowRect.right = nWidth;
	windowRect.bottom = nHeight;

	AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

	m_hWnd = CreateWindowEx(
		dwExStyle,
		m_sClassName,
		m_sAppTitle,
		dwStyle,
		CW_USEDEFAULT, CW_USEDEFAULT,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		0,
		0,
		m_hInstance,
		0
	);
	ShowWindow(m_hWnd, nCmdShow);
}

void AppWindow::DestroyGameWindow()
{
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT AppWindow::MsgCallBack(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
	{
		//int xPos = GET_X_LPARAM(lParam);
		//int yPos = GET_Y_LPARAM(lParam);
		break;
	}
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

		// Note that this tutorial does not handle resizing (WM_SIZE) requests,
		// so we created the window without the resize border.

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}
