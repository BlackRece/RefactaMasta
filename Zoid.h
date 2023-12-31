#ifndef ZOID_H
#define ZOID_H

#include "DrawableGameObject.h"
#include <vector>

class Camera;

class Zoid :
	public DrawableGameObject
{
public:
	typedef vector<std::shared_ptr<Zoid>> vecZoid;

	struct ZoidData
	{
		float fTimer;
		float fTotalTime;
		int iEaten;

		std::vector<float> stats;

		void AddZoid()
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

	Zoid(XMFLOAT3 position);
	~Zoid();

	static inline ZoidData			getStats() { return m_stats; }
	static inline void				updateStats(float delta) { m_stats.Update(delta); }
	
	static void						setSeperationMultiplier(const float value);
	static void						setAlignmentMultiplier(const float value);
	static void						setCohesionMultiplier(const float value);
	static void						setVelocityMultiplier(const float value);

	XMFLOAT3*						getDirection() { return &m_direction; }

	void							WrapBoundary(const XMFLOAT2 &dimensions);

	void							update(float t, vecZoid drawList);

	void							setRange(const float value);

protected:
	void							setDirection(XMFLOAT3 direction);


	vecZoid							nearbyZoids(vecZoid ZoidList);
	XMFLOAT3						calculateSeparationVector_Group(vecZoid drawList);
	XMFLOAT3						calculateSeparationVector_Nearest(vecZoid drawList);
	XMFLOAT3						calculateAlignmentVector(vecZoid drawList);
	XMFLOAT3						calculateCohesionVector(vecZoid drawList);
	XMFLOAT3						calculateFlockingVector(vecZoid ZoidList);
	XMFLOAT3						calculatePredatorVector(vecZoid ZoidList);
	void							createRandomDirection();

	XMFLOAT3						addFloat3(XMFLOAT3& f1, XMFLOAT3& f2);
	XMFLOAT3						subtractFloat3(XMFLOAT3& f1, XMFLOAT3& f2);
	XMFLOAT3						normaliseFloat3(XMFLOAT3& f1);
	float							magnitudeFloat3(XMFLOAT3& f1);
	float							dotProduct(XMFLOAT3& f1, XMFLOAT3& f2);
	float							distanceFloat3(XMFLOAT3& f1, XMFLOAT3& f2);
	XMFLOAT3						multiplyFloat3(XMFLOAT3& f1, const float scalar);
	XMFLOAT3						divideFloat3(XMFLOAT3& f1, const float scalar);
	XMFLOAT3						lerpFloat3(const XMFLOAT3& f1, const XMFLOAT3& f2, const float scalar);

	XMFLOAT3						addWeightedFloat3(XMFLOAT3& dest, XMFLOAT3& source, const float multiplier);

	float							RandomFloat(const float fMin, const float fMax);

	XMFLOAT3						m_direction;
	XMFLOAT3						m_velocity;
	float							m_speed;

private:
	float							m_range;

	static float					m_seperationMultiplier;
	static float					m_alignmentMultiplier;
	static float					m_cohesionMultiplier;
	static float					m_velocityMultiplier;

	static ZoidData					m_stats;
};

#endif // !ZOID_H