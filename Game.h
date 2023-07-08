#ifndef GAME_H
#define GAME_H

#include <DirectXMath.h>
#include <memory>
#include <vector>
#include "Zoid.h"

#define SEPERATION_MULTIPLIER	5.0f    //1.0f//12.0f
#define ALIGNMENT_MULTPLIER		10.0f   //1.0f//8.0f
#define COHESION_MULTIPLIER		5.0f    //1.0f//8.0f

#define VELOCITY_MULTIPLIER		5.0f

class Graphics;
class Camera;

class Game
{
public:

	enum Formation
	{
		SQUARE = 0,
		CIRCLE = 1,
		SPIRAL = 2
	};

	Game();
	~Game();

	void Initialise(Graphics& pGraphics);
	void Update(float dt);
	void Render(Graphics& pGraphics);

	std::shared_ptr<Zoid> createFish(XMFLOAT3 position, bool shark);
	void squareFormation(int amount);
	void circleFormation(int radius, int amount);
	void spiralFormation(int coils, int radius, int rotation);
	void placeFish(Formation eFormation);

private:
	void DeleteZoids();

	Zoid::vecZoid				m_vecZoids;

	// TODO: Instantiate a Camera object
	std::unique_ptr<Camera>		m_pCamera;

	float						m_fSeperation;
	float						m_fAlignment;
	float						m_fCohesion;
	float						m_fVelocity;
};

#endif // !GAME_H
