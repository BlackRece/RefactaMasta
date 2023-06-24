#ifndef APPWINDOW_H
#define APPWINDOW_H

#include <Windows.h>
#include <memory>
#include "Resource.h"
#include "ImGui.h"

class AppWindow
{
public:
	struct AppWindowParams
	{
		int			nCmdShow;
		int			nWidth;
		int			nHeight;
		wchar_t*	pTitle;
	};

	AppWindow(HINSTANCE hInstance, AppWindowParams& params);
	~AppWindow();

	HWND GetHandle() const;
	int GetWidth() const;
	int GetHeight() const;

private:
	AppWindow(const AppWindow&);
	void operator=(const AppWindow&);

	void CreateClass();
	void DestroyClass();

	void CreateGameWindow(int nWidth, int nHeight, int nCmdShow);
	void DestroyGameWindow();

	static LRESULT CALLBACK MsgCallBack(
		HWND hWnd,
		UINT message,
		WPARAM wParam,
		LPARAM lParam);

	HINSTANCE	m_hInstance;
	HWND		m_hWnd;
	ATOM		m_hClass;

	int			m_nWidith;
	int			m_nHeight;

	wchar_t*	m_sAppTitle;
	static const wchar_t* m_sClassName;

};

#endif // !APPWINDOW_H