#ifndef APPENGINE_H
#define APPENGINE_H

#include <memory>
#include "AppWindow.h"
#include "Graphics.h"
#include "Camera.h"
//#include "Light.h"
#include "FrameTimer.h"

#define DEFAULT_FPS 60.0f

class Game;

class AppEngine
{
public:
	struct AppEngineParams
	{
		int			nCmdShow;
		int			nWidth;
		int			nHeight;
		wchar_t*	pTitle;
		HINSTANCE	hInstance;
	};

	AppEngine(AppEngineParams params);
	~AppEngine();
	void Start();
	void Run();
	void Stop();

private:
	void ProcessMessages();
	void Update();
	void Render();

	bool m_initialized;
	bool m_bRunning;

	// TODO: Instantiate a Graphics object
	std::unique_ptr<Graphics> m_pGraphics;

	// TODO: Instantiate a Window object
	std::unique_ptr<AppWindow> m_pAppWindow;

	// TODO: Instantiate a Light object
	//std::unique_ptr<Light> m_pLight;

	std::unique_ptr<FrameTimer> m_pTimer;

	std::unique_ptr<Game> m_pGame;
};

#endif // !APPENGINE_H
