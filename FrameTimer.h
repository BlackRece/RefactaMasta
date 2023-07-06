#ifndef FRAMETIMER_H
#define FRAMETIMER_H

#include <chrono>

class FrameTimer
{
public:
	FrameTimer(float fFPS);
	~FrameTimer();

	void Start();
	void Stop();
	bool Tick();

	float GetDelta() const;
	float GetFPS() const;

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_tpStart;
	float m_fDeltaTime;
	float m_fFPS;
	float m_fDuration;
	bool m_bIsRunning;
};

#endif // !FRAMETIMER_H
