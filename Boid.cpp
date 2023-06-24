#include "Boid.h"

#define NEARBY_DISTANCE			20.0f	// how far boids can see

BoidData Boid::m_stats;

Boid::Boid(XMFLOAT3 position)
{
	m_position = position;
	//m_direction = XMFLOAT3(0, 1, 0);
	createRandomDirection();
	if (isnan(m_direction.x))
		m_direction = XMFLOAT3(0, 1, 0);
		//m_direction = m_direction;

	m_speed = 100;
}

Boid::~Boid()
{
}

void Boid::createRandomDirection()
{
	float x = (float)(rand() % 10);
	x -= 5;
	float y = (float)(rand() % 10);
	y -= 5;
	float z = 0;
	setDirection(XMFLOAT3(x, y, z));
}

void Boid::setDirection(XMFLOAT3 direction)
{
	XMVECTOR v = XMLoadFloat3(&direction);
	v = XMVector3Normalize(v);
	XMStoreFloat3(&m_direction, v);
}

void Boid::setSeperationMultiplier(const float value)
{
	m_seperationMultiplier = value;
}

void Boid::setAlignmentMultiplier(const float value)
{
	m_alignmentMultiplier = value;
}

void Boid::setCohesionMultiplier(const float value)
{
	m_cohesionMultiplier = value;
}

void Boid::setVelocityMultiplier(const float value)
{
	m_velocityMultiplier = value;
}

void Boid::update(float t, vecBoid* boidList)
{

	// create a list of nearby boids
	vecBoid nearBoids = nearbyBoids(boidList);

	XMFLOAT3 vDesiredDirection = m_scale == 1 
		? calculateFlockingVector(&nearBoids)
		: calculatePredatorVector(&nearBoids);

	//add lerp m_direction to desired dir
	XMFLOAT3 vDirection = lerpFloat3(m_direction, vDesiredDirection, t);
	//XMFLOAT3 vDirection = vDesiredDirection;
	
	float fSpeed = t * m_speed;
	if (m_scale != 1) fSpeed *= 1.5f;
	// set shark
	if (m_scale != 1) {
		//XMFLOAT3 vAgression = multiplyFloat3(vSeparation, -1.0f);
		//vVelocity = addFloat3(vAgression, vAlignment);
		//vVelocity = addFloat3(vVelocity, vCohesion);
	}
	
	m_direction = addFloat3(m_direction, vDirection);
	
	if (magnitudeFloat3(m_direction) > 0.0f)
	{
		XMFLOAT3 vVelocity = multiplyFloat3(m_direction, fSpeed);
		m_direction = normaliseFloat3(vVelocity);
		m_position = addFloat3(m_position, m_direction);
	}
	else
	{
		createRandomDirection();
	}
	
	DrawableGameObject::update(t);
}

XMFLOAT3 Boid::addWeightedFloat3(XMFLOAT3& dest, XMFLOAT3& source, const float multiplier)
{
	XMFLOAT3 vWeighted = multiplyFloat3(source, multiplier);
	return addFloat3(dest, vWeighted);
	//dest = addFloat3(dest, vWeighted);
	//return normaliseFloat3(dest);
}

XMFLOAT3 Boid::calculateFlockingVector(vecBoid* boidList) {
	// NOTE these functions should always return a normalised vector
	//XMFLOAT3  vSeparation = calculateSeparationVector_Group(boidList);
	XMFLOAT3  vSeparation = calculateSeparationVector_Nearest(boidList);
	XMFLOAT3  vAlignment = calculateAlignmentVector(boidList);
	XMFLOAT3  vCohesion = calculateCohesionVector(boidList);

	// set me
	//XMFLOAT3 vDesiredDirection = XMFLOAT3(0, 0, 0);
	//if (vDesiredDirection.x != 0)
	//	vDesiredDirection.x = 0;
	//if (vDesiredDirection.y != 0)
	//	vDesiredDirection.y = 0;
	//if (vDesiredDirection.z != 0)
	//	vDesiredDirection.z = 0;

	vSeparation = multiplyFloat3(vSeparation, m_seperationMultiplier);
	vAlignment = multiplyFloat3(vAlignment, m_alignmentMultiplier);
	vCohesion = multiplyFloat3(vCohesion, m_cohesionMultiplier);

	XMFLOAT3 vDirection = XMFLOAT3 (vSeparation);
	//vDesiredDirection = addFloat3(vDesiredDirection, vSeparation);
	vDirection = addFloat3(vDirection, vAlignment);
	vDirection = addFloat3(vDirection, vCohesion);

	return vDirection;
}

XMFLOAT3 Boid::calculatePredatorVector(vecBoid* boidList) 
{
	XMFLOAT3 vDesiredDirection = m_direction;

	if (boidList == nullptr || boidList->size() == 0)
		return vDesiredDirection;
	
	float nearestDistance = NEARBY_DISTANCE;// 9999.0f;
	DrawableGameObject* nearest = nullptr;

	for (Boid* boid : *boidList)
	{
		XMFLOAT3 directionOfNearest = subtractFloat3(*boid->getPosition(), m_position);
		float distance = magnitudeFloat3(directionOfNearest);
		if (distance < nearestDistance)
		{
			nearestDistance = distance;
			nearest = boid;
			vDesiredDirection = directionOfNearest;
		}
	}

	//TODO: can't delete! must mark for deletion so that another process can remove this boid.
	if (nearest != nullptr && nearestDistance <= 5.0f)
	{
		nearest->setPosition(XMFLOAT3(0,0,0));
		m_stats.AddBoid();
	}
	
	//return normaliseFloat3(vDesiredDirection);
	return vDesiredDirection;
}

XMFLOAT3 Boid::calculateSeparationVector_Group(vecBoid* boidList)
{
	// calculate average position of nearby
	if (isnan(m_direction.x))
		m_direction = m_direction;
	
	XMFLOAT3 vAverage = XMFLOAT3(0, 0, 0);
	
	if (boidList == nullptr || boidList->size() == 0)
		return vAverage;

	if (isnan(m_direction.x))
		m_direction = m_direction;
	
	for (Boid* boid : *boidList) {
		XMFLOAT3 mePos = m_position;
		XMFLOAT3 itPos = *boid->getPosition();
		if (isnan(m_direction.x))
			m_direction = m_direction;

		XMFLOAT3 directionNearest = subtractFloat3(itPos, mePos);

		if (magnitudeFloat3(m_direction) == 0)
		{
			m_direction = m_direction;
		}
//		const float nearestDistanceSquared = dotProduct(directionNearest, directionNearest);
		const float nearestDistanceSquared = dotProduct(m_direction, directionNearest);
	 
		if (isnan(m_direction.x))
			m_direction = m_direction;

		if (isnan(nearestDistanceSquared))
			mePos = mePos;

		XMFLOAT3 boidDirection = divideFloat3(directionNearest, -nearestDistanceSquared);
		vAverage = addFloat3(vAverage, boidDirection);
	}
	
	vAverage = divideFloat3(vAverage, (float)boidList->size());
	vAverage = normaliseFloat3(vAverage);

	return vAverage;
}

XMFLOAT3 Boid::calculateSeparationVector_Nearest(vecBoid* boidList)
{
	// calculate average position of nearby

	float nearestDistance = NEARBY_DISTANCE;// 9999.0f;
	DrawableGameObject* nearest = nullptr;
	XMFLOAT3 directionNearestStored;

	XMFLOAT3 nearby = XMFLOAT3(0, 0, 0);
	if (boidList == nullptr)
		return nearby;

	XMFLOAT3 mePos = m_position;

	for (Boid* boid : *boidList) 
	{
		if (boid == this)
			continue;

		XMFLOAT3 itPos = *boid->getPosition();

		XMFLOAT3 directionNearest = subtractFloat3(itPos, mePos);
		float d = magnitudeFloat3(directionNearest);
		if (d < nearestDistance)
		{
			nearestDistance = d;
			nearest = boid;
			directionNearestStored = directionNearest;
		}
	}

	if (nearest != nullptr) 
	{
		directionNearestStored = normaliseFloat3(directionNearestStored);
		if (nearestDistance < 10.0f)
			directionNearestStored = multiplyFloat3(directionNearestStored, 10.0f - nearestDistance);
		//return normaliseFloat3(directionNearestStored);
		return directionNearestStored;
	}

	return m_direction;
}

XMFLOAT3 Boid::calculateAlignmentVector(vecBoid* boidList)
{
	XMFLOAT3 vDirection = XMFLOAT3(0, 0, 0);

	if (boidList == nullptr || boidList->size() == 0)
		return vDirection;

	// your code here
	for (Boid* boid : *boidList)
		vDirection = addFloat3(vDirection, *boid->getDirection());

	vDirection = divideFloat3(vDirection, boidList->size());

	return normaliseFloat3(vDirection); // return the normalised (average) direction of nearby drawables
}

XMFLOAT3 Boid::calculateCohesionVector(vecBoid* boidList)
{
	XMFLOAT3 vDirection = XMFLOAT3(0, 0, 0);

	if (boidList == nullptr || boidList->size() == 0)
		return vDirection;

	// calculate average position of nearby
	for (Boid* boid : *boidList) 
		vDirection = addFloat3(vDirection, *boid->getPosition());

	vDirection = divideFloat3(vDirection, boidList->size());
	//vDirection = subtractFloat3(vDirection, m_position);
	vDirection = subtractFloat3(m_position, vDirection);

	return normaliseFloat3(vDirection); // nearby is the direction to where the other drawables are
}


// use but don't alter the methods below

XMFLOAT3 Boid::addFloat3(XMFLOAT3& f1, XMFLOAT3& f2)
{
	XMFLOAT3 out;
	out.x = f1.x + f2.x;
	out.y = f1.y + f2.y;
	out.z = f1.z + f2.z;

	return out;
}

XMFLOAT3 Boid::subtractFloat3(XMFLOAT3& f1, XMFLOAT3& f2)
{
	XMFLOAT3 out;
	out.x = f1.x - f2.x;
	out.y = f1.y - f2.y;
	out.z = f1.z - f2.z;

	return out;
}

XMFLOAT3 Boid::multiplyFloat3(XMFLOAT3& f1, const float scalar)
{
	XMFLOAT3 out;
	out.x = f1.x * scalar;
	out.y = f1.y * scalar;
	out.z = f1.z * scalar;

	return out;
}

XMFLOAT3 Boid::divideFloat3(XMFLOAT3& f1, const float scalar)
{
	XMFLOAT3 out;
	out.x = f1.x / scalar;
	out.y = f1.y / scalar;
	out.z = f1.z / scalar;

	return out;
}

float Boid::magnitudeFloat3(XMFLOAT3& f1)
{
	return sqrt((f1.x * f1.x) + (f1.y * f1.y) + (f1.z * f1.z));
}

float Boid::distanceFloat3(XMFLOAT3& f1, XMFLOAT3& f2)
{
	return sqrt(pow(f2.x - f1.x, 2) + pow(f2.y - f1.y, 2) * 1.0);
}

XMFLOAT3 Boid::normaliseFloat3(XMFLOAT3& f1)
{
	//float length = sqrt((f1.x * f1.x) + (f1.y * f1.y) + (f1.z * f1.z));
	float length = magnitudeFloat3(f1);
	return divideFloat3(f1, length);

	f1.x /= length;
	f1.y /= length;
	f1.z /= length;

	return f1;
}

float Boid::dotProduct(XMFLOAT3& f1, XMFLOAT3& f2)
{
	//XMFLOAT3 norm1 = normaliseFloat3(f1);
	//XMFLOAT3 norm2 = normaliseFloat3(f2);
	//return (norm1.x * norm2.x) + (norm1.y * norm2.y) + (norm1.z * norm2.z);
	return (f1.x * f2.x) + (f1.y * f2.y) + (f1.z * f2.z);
}

XMFLOAT3 Boid::lerpFloat3(const XMFLOAT3& f1, const XMFLOAT3& f2, const float scalar)
{
	//compute the linear interpolation between two vectors
	XMFLOAT3 out;
	out.x = f1.x + scalar * (f2.x - f1.x);
	out.y = f1.y + scalar * (f2.y - f1.y);
	out.z = f1.z + scalar * (f2.z - f1.z);
	
	return out;
}

vecBoid Boid::nearbyBoids(vecBoid* boidList)
{
	vecBoid nearBoids;
	if (boidList->size() == 0)
		return nearBoids;

	for (Boid* boid : *boidList) {
		// ignore self
		if (boid == this)
			continue;

		// get the distance between the two
		XMFLOAT3 vB = *(boid->getPosition());
		XMFLOAT3 vDiff = subtractFloat3(m_position, vB);
		float l = magnitudeFloat3(vDiff);
		if (l < NEARBY_DISTANCE) {
			nearBoids.push_back(boid);
		}
	}

	return nearBoids;
}

void Boid::checkIsOnScreenAndFix(const XMMATRIX&  view, const XMMATRIX&  proj)
{
	XMFLOAT4 v4;
	v4.x = m_position.x;
	v4.y = m_position.y;
	v4.z = m_position.z;
	v4.w = 1.0f;

	XMVECTOR vScreenSpace = XMLoadFloat4(&v4);
	XMVECTOR vScreenSpace2 = XMVector4Transform(vScreenSpace, view);
	XMVECTOR vScreenSpace3 = XMVector4Transform(vScreenSpace2, proj);

	XMFLOAT4 v;
	XMStoreFloat4(&v, vScreenSpace3);
	v.x /= v.w;
	v.y /= v.w;
	v.z /= v.w;
	v.w /= v.w;

	float fOffset = 10; // a suitable distance to rectify position within clip space
	if (v.x < -1 || v.x > 1 || v.y < -1 || v.y > 1)
	{
		if (v.x < -1 || v.x > 1) {
			v4.x = -v4.x + (fOffset * v.x);
		}
		else if (v.y < -1 || v.y > 1) {
			v4.y = -v4.y + (fOffset * v.y);
		}

		// throw a bit of randomness into the mix
		//createRandomDirection();
	}

	// method 1 - appear on the other side
	m_position.x = v4.x;
	m_position.y = v4.y;
	m_position.z = v4.z;


	// method2 - bounce off sides and head to centre
	/*if (v.x < -1 || v.x > 1 || v.y < -1 || v.y > 1)
	{
		m_direction = multiplyFloat3(m_direction, -1);;
		m_direction = normaliseFloat3(m_direction);
	}*/

	return;
}