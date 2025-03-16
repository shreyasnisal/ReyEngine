#pragma once

#include "Engine/Math/Vec2.hpp"

/*! \file AnalogJoystick.hpp */

//! The default value for the inner deadzone threshold
constexpr float DEFAULT_INNER_DEADZONE_FRACTION = 0.3f;
//! The default value for the outer deadzone threshold
constexpr float DEFAULT_OUTER_DEADZONE_FRACTION = 0.95f;

/*! \brief Represents an analog joystick on a controller
* 
* Stores the raw and deadzone-corrected values for the joystick
* 
*/
class AnalogJoystick
{
public:
	~AnalogJoystick() = default;
	AnalogJoystick() = default;

	Vec2	GetPosition() const;
	float	GetMagnitude() const;
	float	GetOrientationDegrees() const;
	Vec2	GetRawUncorrectedPosition() const;
	float	GetInnerDeadZoneFraction() const;
	float	GetOuterDeadZoneFraction() const;

	void	Reset();
	void	SetDeadZoneThresholds(float normalizedInnerDeadzoneThreshold, float normalizedOuterDeadzoneThreshold);
	void	UpdatePosition(float rawNormalizedX, float rawNormalizedY);

protected:
	//! The raw position of the joystick
	Vec2	m_rawPosition;
	//! The deadzone correct position of the joystick
	Vec2	m_deadZoneCorrectedCartesianPosition;
	//! The inner deadzone threshold
	float	m_innerDeadZoneFraction = DEFAULT_INNER_DEADZONE_FRACTION;
	//! The outer deadzone threshold
	float	m_outerDeadZoneFraction = DEFAULT_OUTER_DEADZONE_FRACTION;
	//! The deadzone-corrected angle with the +X axis
	float	m_deadZoneCorrectedPolarCoordsDegrees = 0.f;
	//! The deadzone-corrected magnitude of the joystick
	float	m_deadZoneCorrectedPolarCoordsR = 0.f;
};
