#include "Engine/Core/Clock.hpp"

#include "Engine/Core/Time.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//! A root-level system clock that can be accessed through static methods. Acts as the parent clock for all clocks without an explicitly defined parent.
static Clock* systemClock = new Clock();

/*! \brief Returns the system clock
* 
* The system clock is used as a parent clock for all clocks without an explicit parent.
* \return The system clock
* 
*/
Clock& Clock::GetSystemClock()
{
	return *systemClock;
}

/*! \brief Ticks the system clock
* 
* Ticking the system clock invokes the Tick method on the system clock which in turn invokes the Advance method on the system clock as well as all child clocks (all clocks except if the hierarchy has been broken through game code).
* 
*/
void Clock::TickSystemClock()
{
	systemClock->Tick();
}

/*! \brief Constructor to create a clock with the system call as its parent
* 
*/
Clock::Clock()
{
	if (systemClock)
	{
		systemClock->AddChild(this);
	}
}

/*! \brief Constructor to create a clock with an explicit parent
* 
* \param parent A Clock to be used as the parent for this clock
* 
*/
Clock::Clock(Clock& parent)
	: m_parent(&parent)
{
	m_parent->AddChild(this);
}

/*! \brief Destructor for clocks
* 
* Orphans all child clocks before destroying this clock. Game code must fix the hierarchy if required.
* 
*/
Clock::~Clock()
{
	this->m_parent = 0;

	for (int childIndex = 0; childIndex < static_cast<int>(m_children.size()); childIndex++)
	{
		m_children[childIndex]->m_parent = nullptr;
	}
}

/*! \brief Resets the clock
* 
* Resets all bookkeeping variables and sets the lastUpdatedTime to the current time (seconds since the App started).
* The total seconds and frame count that this clock has been running for also gets reset when this function is invoked, and all subsequent retrievals of totalSeconds/frameCount will only account for time/frames since the clock was reset.
* 
*/
void Clock::Reset()
{
	m_totalSeconds = 0.f;
	m_deltaSeconds = 0.f;
	m_frameCount = 0;
	m_lastUpdatedTimeInSeconds = static_cast<float>(GetCurrentTimeSeconds());
}

/*! Sets the clock to single-step-frame mode
* 
* When the single-step-frame mode is active, each tick on this clock only results in a single frame update followed by pausing this clock
* 
*/
void Clock::StepSingleFrame()
{
	m_singleStepFrame = true;
}

/*! Ticks this clock
* 
* In general, only the system clock should be ticked by the game code, and all clocks must be in the hierarchy of clocks so that they get ticked based on the pause state and timescale of all its ancestors.
* Updates the lastUpdatedTime to the current time (seconds since the App started)
* 
*/
void Clock::Tick()
{
	float currentTime = static_cast<float>(GetCurrentTimeSeconds());
	float deltaTimeSeconds = GetClamped(currentTime - m_lastUpdatedTimeInSeconds, 0.f, m_maxDeltaSeconds);
	Advance(deltaTimeSeconds);
	m_lastUpdatedTimeInSeconds = currentTime;
}

/*! \brief Advances this clock
* 
* Advancing clocks leads to an increment in the frame count, and the total seconds (by the deltaSeconds scaled by the pause state and timescale). The method also handles single frame stepping if the corresponding data member is set, and invokes Advance on all its children.
* \param deltaTimeSeconds A float indicating the time (in seconds) since this clock was last updated
* 
*/
void Clock::Advance(float deltaTimeSeconds)
{
	if (m_singleStepFrame)
	{
		m_isPaused = false;
	}

	deltaTimeSeconds *= m_timeScale;
	if (m_isPaused)
	{
		deltaTimeSeconds *= 0.f;
	}

	m_totalSeconds += deltaTimeSeconds;
	m_deltaSeconds = deltaTimeSeconds;
	m_frameCount++;

	for (int childIndex = 0; childIndex < static_cast<int>(m_children.size()); childIndex++)
	{
		m_children[childIndex]->Advance(deltaTimeSeconds);
	}

	if (m_singleStepFrame)
	{
		m_isPaused = true;
		m_singleStepFrame = false;
	}
}

/*! \brief Adds a clock to this clock's list of children
* 
* Also sets the parent for the child clock to this clock.
* \param childClock The Clock to be added as a child
* 
*/
void Clock::AddChild(Clock* childClock)
{
	m_children.push_back(childClock);
	childClock->m_parent = this;
}

/*! \brief Removes a clock from this clock's list of children
* 
* \param childClock The clock to be removed
* 
*/
void Clock::RemoveChild(Clock* childClock)
{
	for (int childIndex = 0; childIndex < static_cast<int>(m_children.size()); childIndex++)
	{
		if (m_children[childIndex] == childClock)
		{
			m_children.erase(m_children.begin() + childIndex);
			childClock->m_parent = nullptr;
		}
	}
}
