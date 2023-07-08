#ifndef IMGUIWRAPPER_H
#define IMGUIWRAPPER_H

#include <Windows.h>

class Graphics;

class ImGuiWrapper
{
public:
	struct MultiplierParams
	{
		int		iZoidAmount;

		float	fSeperation;
		float	fAlignment;
		float	fCohesion;
		float	fVelocity;
	};

	ImGuiWrapper();
	~ImGuiWrapper();

	void Initialise(HWND& hWnd, Graphics& pGraphics);
	void Render();
	void Shutdown();

	void SetMultipliers(MultiplierParams params);
	MultiplierParams GetMultipliers();

private:
	ImGuiWrapper(const ImGuiWrapper&);
	void operator=(const ImGuiWrapper&);

	const float vMin = -1.0f;
	const float vMax = 2.0f;

	int m_iZoidAmount = 100;
	int m_iRadius = 100;
	int m_iCoils = 10;
	int m_iRotation = 10;

	float m_fSeperation = 5.0f;
	float m_fAlignment = 10.0f;
	float m_fCohesion = 5.0f;
	float m_fVelocity = 5.0f;

	//Zoid::ZoidStats m_zoidStats;
};

#endif // !IMGUIWRAPPER_H