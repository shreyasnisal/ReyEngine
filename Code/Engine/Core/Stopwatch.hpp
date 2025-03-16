#pragma once

class Clock;

/*! \brief A stopwatch class that helps keep track of specific time intervals (laps)
* 
* Useful for events that need to happen at a fixed interval. Uses a clock to keep track of time.
* \sa Clock
* 
*/
class Stopwatch
{
public:
	//! Default destructor for Stopwatch instances
	~Stopwatch() = default;
	//! Default constructor for Stopwatch instances
	Stopwatch() = default;
	//! Default copy constructor for Stopwatch instances
	Stopwatch(Stopwatch const& copyFrom) = default;
	explicit Stopwatch(float duration);
	Stopwatch(Clock* const clock, float duration);
	
	void					Start();
	void					Restart();
	void					Stop();
	float					GetElapsedTime() const;
	float					GetElapsedFraction() const;
	bool					IsStopped() const;
	bool					HasDurationElapsed() const;
	bool					DecrementDurationIfElapsed();

public:
	//! The clock that this Stopwatch should use to keep track of time
	Clock*					m_clock = nullptr;
	//! The clock time that this Stopwatch starts at
	float					m_startTime = 0.f;
	//! The fixed duration that this Stopwatch should track
	float					m_duration = 0.f;
};
