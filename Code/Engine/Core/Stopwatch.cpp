#include "Engine/Core/Stopwatch.hpp"

#include "Engine/Core/Clock.hpp"

/*! \brief Constructs a Stopwatch from a duration only
* 
* Uses the system clock as the parent clock to keep track of time.
* \param duration A floating point number indicating the duration that this Stopwatch should track
* 
*/
Stopwatch::Stopwatch(float duration)
	: m_clock(&Clock::GetSystemClock())
	, m_duration(duration)
{
}

/*! \brief Constructs a Stopwatch from a Clock and duration
* 
* Uses the Clock passed in as the parent clock to keep track of time.
* \param clock A pointer to the clock to be used as the parent
* \param duration A floating point number indicating the duration that this Stopwatch shold track
* 
*/
Stopwatch::Stopwatch(Clock* const clock, float duration)
	: m_clock(clock)
	, m_duration(duration)
{
}

/*! \brief Starts this Stopwatch
* 
* Sets the start time of this Stopwatch to the current time according to the parent Clock
* 
*/
void Stopwatch::Start()
{
	m_startTime = m_clock->GetTotalSeconds();
	if (m_startTime == 0.f)
	{
		m_startTime += 0.001f;
	}
}

/*! \brief Restarts this Stopwatch
* 
* Sets the start time of this Stopwatch to the current time according to the parent clock. Only works if the Stopwatch has already been started.
*/
void Stopwatch::Restart()
{
	if (m_startTime != 0.f)
	{
		m_startTime = m_clock->GetTotalSeconds();
	}
}

/*! \brief Stops this Stopwatch
* 
* Resets the start time of this Stopwatch.
* 
*/
void Stopwatch::Stop()
{
	m_startTime = 0.f;
}

/*! \brief Gets the time in seconds that has elapsed since this Stopwatch was started
* 
* Returns zero if this Stopwatch is not running.
* \return A floating point number that indicates the amount of time that has passed since this Stopwatch was started
*/
float Stopwatch::GetElapsedTime() const
{
	if (m_startTime == 0.f)
	{
		return 0.f;
	}

	return (m_clock->GetTotalSeconds() - m_startTime);
}

/*! \brief Gets the ratio of time elapsed since this Stopwatch was started to the duration of this Stopwatch
* 
* Returns zero if this Stopwatch is not running.
* \return A floating point number representing the ratio of time elapsed since this Stopwatch was started to the duration of this Stopwatch
* 
*/
float Stopwatch::GetElapsedFraction() const
{
	if (m_duration == 0.f)
	{
		return 0.f;
	}

	float elapsedTime = GetElapsedTime();
	return elapsedTime / m_duration;
}

/*! \brief Gets whether this Stopwatch is stopped
* 
* \return A boolean that is true if this Stopwatch is stopped
* 
*/
bool Stopwatch::IsStopped() const
{
	return m_startTime == 0.f;
}

/*! \brief Gets whether this Stopwatch has been running for longer than its duration
* 
* Only returns true if the Stopwatch is still running.
* \return A boolean that is true if this Stopwatch has been running for longer than its duration
* 
*/
bool Stopwatch::HasDurationElapsed() const
{
	return ((GetElapsedTime() > m_duration) && !IsStopped());
}

/*! \brief Gets whether this Stopwatch has been running for longer than its duration and decrements the elapsed time by the duration
* 
* This method decrements the duration by incrementing the start time by the duration. The method should generally be called inside a loop in case multiple durations have elapsed since the last time this method was called.
* \return A boolean that is true if this Stopwatch has been running for longer than its duration and the start time was incremented
* 
*/
bool Stopwatch::DecrementDurationIfElapsed()
{
	if (HasDurationElapsed())
	{
		m_startTime += m_duration;
		return true;
	}

	return false;
}
