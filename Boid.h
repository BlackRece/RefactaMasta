#pragma once
#include "DrawableGameObject.h"
#include <vector>

struct BoidData
{
	float fTimer;
	float fTotalTime;
	int iEaten;

	std::vector<float> stats; 

	void AddBoid() 
	{
		stats.push_back(fTimer); 
		fTotalTime += fTimer;
		fTimer = 0;
	}

	float GetAverage() 
	{
		if (stats.size() < 1)
			return 0.0f;

		float fTotal = 0;
		for (float fInstance : stats)
			fTotal += fInstance;

		return fTotal / stats.size();
	}

	inline void Update(float delta) { fTimer += delta; }
};

class Boid :
	public DrawableGameObject
{
public:

	Boid(XMFLOAT3 position);
	~Boid();

	static inline BoidData				getStats() { return m_stats; }
	static inline void					updateStats(float delta) { m_stats.Update(delta); }

	XMFLOAT3*							getDirection() { return &m_direction; }
	void								checkIsOnScreenAndFix(const XMMATRIX&  view, const XMMATRIX&  proj);
	void								update(float t, vecBoid& drawList);

	void								setSeperationMultiplier(const float value);
	void								setAlignmentMultiplier(const float value);
	void								setCohesionMultiplier(const float value);

	void								setVelocityMultiplier(const float value);
	void								setRange(const float value);

protected:
	void								setDirection(XMFLOAT3 direction);


	vecBoid								nearbyBoids(vecBoid& boidList);
	XMFLOAT3							calculateSeparationVector_Group(vecBoid* drawList);
	XMFLOAT3							calculateSeparationVector_Nearest(vecBoid* drawList);
	XMFLOAT3							calculateAlignmentVector(vecBoid* drawList);
	XMFLOAT3							calculateCohesionVector(vecBoid* drawList);
	XMFLOAT3							calculateFlockingVector(vecBoid* boidList);
	XMFLOAT3							calculatePredatorVector(vecBoid* boidList);
	void								createRandomDirection();

	XMFLOAT3							addFloat3(XMFLOAT3& f1, XMFLOAT3& f2);
	XMFLOAT3							subtractFloat3(XMFLOAT3& f1, XMFLOAT3& f2);
	XMFLOAT3							normaliseFloat3(XMFLOAT3& f1);
	float								magnitudeFloat3(XMFLOAT3& f1);
	float								dotProduct(XMFLOAT3& f1, XMFLOAT3& f2);
	float								distanceFloat3(XMFLOAT3& f1, XMFLOAT3& f2);
	XMFLOAT3							multiplyFloat3(XMFLOAT3& f1, const float scalar);
	XMFLOAT3							divideFloat3(XMFLOAT3& f1, const float scalar);
	XMFLOAT3							lerpFloat3(const XMFLOAT3& f1, const XMFLOAT3& f2, const float scalar);

	XMFLOAT3							addWeightedFloat3(XMFLOAT3& dest, XMFLOAT3& source, const float multiplier);

	float								RandomFloat(const float fMin, const float fMax);

	XMFLOAT3							m_direction;
	XMFLOAT3							m_velocity;
	float								m_speed;

private:
	float								m_velocityMultiplier;
	float								m_range;

	float								m_seperationMultiplier;
	float								m_alignmentMultiplier;
	float								m_cohesionMultiplier;

	static BoidData						m_stats;
};

