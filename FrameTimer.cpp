#include "FrameTimer.h"

FrameTimer::FrameTimer(float fFPS) :
    m_tpStart(),
	m_fDeltaTime(0.0f),
	m_fFPS(fFPS),
    m_bIsRunning(false)
{
    m_fDuration = 1.0f / m_fFPS;
}

FrameTimer::~FrameTimer()
{
}

void FrameTimer::Start()
{
    if(m_bIsRunning)
		return;

	m_tpStart = std::chrono::high_resolution_clock::now();
	m_bIsRunning = true;
}

void FrameTimer::Stop()
{
	m_bIsRunning = false;
}

bool FrameTimer::Tick()
{
    if (!m_bIsRunning)
		return false;

    auto timeCur = std::chrono::high_resolution_clock::now();
    auto timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(timeCur - m_tpStart);

    m_tpStart = timeCur;

    m_fDeltaTime += timeDiff.count() / 1000.0f;
    if (m_fDeltaTime >= m_fDuration)
        m_fDeltaTime -= m_fDuration;

    return m_fDeltaTime < m_fDuration;
}

float FrameTimer::GetDelta() const
{
    return m_fDeltaTime;
}

float FrameTimer::GetFPS() const
{
    return m_fFPS;
}
