#ifndef APPENGINE_H
#define APPENGINE_H

class AppEngine
{
public:
	AppEngine();
	~AppEngine();
	void Initialize();
	void Run();
	void Shutdown();

private:
	void Update();
	void Render();

	bool m_initialized;

	// TODO: Instantiate a Camera object
	//std::unique_ptr<Camera> m_pCamera;

	// TODO: Instantiate a Graphics object
	//std::unique_ptr<Graphics> m_pGraphics;

	// TODO: Instantiate a Light object
	//std::unique_ptr<Light> m_pLight;


};

#endif // !APPENGINE_H
