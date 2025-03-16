#pragma once

#include <vector>

/*! \brief A class for storing and retrieving time information
*
* The clock can be used to set up different clocks required for the game- For example, we can set up an independent clock for the gameplay and another for actions that can be performed even if the gameplay clock is not ticking
* 
*/
class Clock
{
public:
	~Clock();
	Clock();
	explicit Clock(Clock& parent);
	Clock(Clock const& copy) = delete;

	void					Reset();

	/*! \brief Whether this clock is paused
	* 
	* \return A boolean that is true if this clock is paused
	* 
	*/
	bool					IsPaused() const { return m_isPaused; }

	/*! \brief Pauses this clock
	*/
	void					Pause() { m_isPaused = true; }
	
	/*! \brief Resumes this clock
	*/
	void					Unpause() { m_isPaused = false; }

	/*! \brief Toggles the paused state of this clock
	* 
	* If this clock is paused, invoking TogglePause resumes it. If this clock is running, invoking TogglePause pauses it.
	* 
	*/
	void					TogglePause() { m_isPaused = !m_isPaused; }

	void					StepSingleFrame();

	/*! \brief Sets the timescale for this clock
	* 
	* Setting the timescale for a clock also affects the timescale for all children. In general, timescale should only be modified for clocks other than the system clock- doing otherwise might causes unexpected results.
	* \param timeScale The new timescale to be used for this clock
	* 
	*/
	void					SetTimeScale(float timeScale) { m_timeScale = timeScale; }
	
	/*! \brief Gets the timescale of this clock
	* 
	* \return The timescale of this clock
	*/
	float					GetTimeScale() const { return m_timeScale; }

	/*! \brief Gets the deltaSeconds of this clock
	* 
	* The deltaSeconds of this clock represents the time passed between two frames subject to the pause state of this clock and the timescale of this clock as well as all its ancestors.
	* \return The deltaSeconds of this clock
	* 
	*/
	float					GetDeltaSeconds() const { return m_deltaSeconds; }

	/*! \brief Gets the total seconds that this clock has been running
	* 
	* \return The total seconds since this clock was started or reset
	* 
	*/
	float					GetTotalSeconds() const { return m_totalSeconds; }
	
	/*! \brief Gets the total number of frames that this clock has been running
	* 
	* \return The number of frames since this clock was started or reset
	* 
	*/
	size_t					GetFrameCount() const { return m_frameCount; }

	void					RemoveChild(Clock* childClock);

public:
	static Clock&			GetSystemClock();
	static void				TickSystemClock();

protected:
	void					Tick();
	void					Advance(float deltaSeconds);
	void					AddChild(Clock* childClock);

protected:
	Clock*					m_parent = nullptr; //! The parent clock for this clock. All pause/resume or timescale operations are passed down the heirarchy of clocks
	std::vector<Clock*>		m_children; //! A list of child clocks of this clock

	float					m_lastUpdatedTimeInSeconds = 0.f; //! The timestamp from the start of the app when this clock was last updated
	float					m_totalSeconds = 0.f; //! The total seconds that have elapsed since this clock was started
	float					m_deltaSeconds = 0.f; //! The difference between the current time and the time that the clock was last updated, subject to pausing and timescale modifications
	size_t					m_frameCount = 0; //! The number of frames that have elapsed since this clock was started
	float					m_timeScale = 1.f; //! The current timescale of this clock- the speed of the clock is scaled by this value
	bool					m_isPaused = false; //! Whether the clock is paused
	bool					m_singleStepFrame = false; //! Whether this clock should step through a single frame
	float					m_maxDeltaSeconds = 0.1f; //! The maximum value that deltaSeconds should be clamped to
};
