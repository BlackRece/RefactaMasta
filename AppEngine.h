#ifndef APPENGINE_H
#define APPENGINE_H

#include <memory>
#include "AppWindow.h"
#include "Graphics.h"
#include "Camera.h"
#include "Light.h"

class AppEngine
{
public:
	struct AppEngineParams
	{
		int			nCmdShow;
		int			nWidth;
		int			nHeight;
		wchar_t*	pTitle;
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
	std::unique_ptr<AppWindow> m_pWindow;

	// TODO: Instantiate a Camera object
	std::unique_ptr<Camera> m_pCamera;

	// TODO: Instantiate a Light object
	//std::unique_ptr<Light> m_pLight;


};

#endif // !APPENGINE_H
