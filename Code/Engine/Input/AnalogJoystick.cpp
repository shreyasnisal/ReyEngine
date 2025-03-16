#include "Engine/Input/AnalogJoystick.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtils.hpp"

#include <limits.h>

/*! \brief Gets the deadzone-corrected cartesian position of the joystick
* 
* \return A Vec2 containing the deadzone-corrected cartesian coordinates of the joystick
* 
*/
Vec2 AnalogJoystick::GetPosition() const
{
	return m_deadZoneCorrectedCartesianPosition;
}

/*! \brief Gets the deadzone-corrected magnitude of the joystick
* 
* \return A floating point number [0.f, 1.f] containing the magnitude of the joystick
* 
*/
float AnalogJoystick::GetMagnitude() const
{
	return m_deadZoneCorrectedPolarCoordsR;
}

/*! \brief Gets the deadzone-corrected (== raw) orientation of the joystick with the +X axis
* 
* \return A floating point number representing the deadzone-corrected (==raw) orientation of the joystick with the +X axis
* 
*/
float AnalogJoystick::GetOrientationDegrees() const
{
	return m_deadZoneCorrectedPolarCoordsDegrees;
}

/*! \brief Gets the raw cartesian position of the joystick
* 
* \return A Vec2 containing the raw cartesian coordinates of the joystick
* 
*/
Vec2 AnalogJoystick::GetRawUncorrectedPosition() const
{
	return m_rawPosition;
}

/*! \brief Gets the inner deadzone threshold for this joystick
* 
* \return A floating point number [0.f, 1.f] representing the inner deadzone threshold of this joystick
*/
float AnalogJoystick::GetInnerDeadZoneFraction() const
{
	return m_innerDeadZoneFraction;
}

/*! \brief Gets the outer deadzone threshold for this joystick
* 
* \return A floating point number [0.f, 1.f] representing the outer deadzone threshold of this joystick
* 
*/
float AnalogJoystick::GetOuterDeadZoneFraction() const
{
	return m_outerDeadZoneFraction;
}

/*! \brief Resets the joystick by setting all values to 0
* 
*/
void AnalogJoystick::Reset()
{
	m_rawPosition = Vec2();
	m_deadZoneCorrectedCartesianPosition = Vec2();
	m_deadZoneCorrectedPolarCoordsDegrees = 0.f;
	m_deadZoneCorrectedPolarCoordsR = 0.f;
}

/*! \brief Sets the deadzone thresholds to the provided values
* 
* \param normalizedInnerDeadzoneThreshold A floating point number [0.f, 1.f] representing the new value for the inner deadzone threshold
* \param normalizzedOuterDeadzoneThreshold A floating point number [0.f, 1.f] representing the new value for the outer deadzone threshold
* 
*/
void AnalogJoystick::SetDeadZoneThresholds(float normalizedInnerDeadzoneThreshold, float normalizedOuterDeadzoneThreshold)
{
	m_innerDeadZoneFraction = normalizedInnerDeadzoneThreshold;
	m_outerDeadZoneFraction = normalizedOuterDeadzoneThreshold;
}

/*! \brief Updates the position of the joystick
* 
* Based on the raw XY coordinates, computes the deadzone-corrected coordinates and updates variables for the raw and deadzone-corected coordinates.
* \param rawNormalizedX A floating point number [0.f, 1.f] representing the new raw X position of the joystick
* \param rawNormalizedY A floating point number [0.f, 1.f[ representing the new raw Y position of the joystick
* 
*/
void AnalogJoystick::UpdatePosition(float rawNormalizedX, float rawNormalizedY)
{
	m_rawPosition = Vec2(rawNormalizedX, rawNormalizedY);

	float rawNormalizedMagnitude = m_rawPosition.GetLength();
	m_deadZoneCorrectedPolarCoordsR = RangeMapClamped(rawNormalizedMagnitude, m_innerDeadZoneFraction, m_outerDeadZoneFraction, 0.f, 1.f);
	m_deadZoneCorrectedPolarCoordsDegrees = m_rawPosition.GetOrientationDegrees();

	m_deadZoneCorrectedCartesianPosition = Vec2::MakeFromPolarDegrees(m_deadZoneCorrectedPolarCoordsDegrees, m_deadZoneCorrectedPolarCoordsR);
}
