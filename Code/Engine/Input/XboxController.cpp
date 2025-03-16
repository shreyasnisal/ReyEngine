#include "Engine/Input/XboxController.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Xinput.h>
#pragma comment(lib, "xinput9_1_0")
#include <limits.h>

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Input/InputSystem.hpp"

/*! \brief Gets whether the controller is connected or not
* 
* \return A boolean indicating whether the controller is connected
* 
*/
bool XboxController::IsConnected() const
{
	return m_isConnected;
}

/*! \brief Gets the controller ID assigned to this controller
* 
* \return An integer representing this controller's ID
* 
*/
int XboxController::GetControllerID() const
{
	return m_id;
}

/*! \brief Gets the left thumbstick of this controller
* 
* \return An AnalogStick representing the left thumbstick of this controller
* 
*/
AnalogJoystick const& XboxController::GetLeftStick() const
{
	return m_leftStick;
}

/*! \brief Gets the right thumbstick of this controller
* 
* \return An AnalogStick representing the right thumbstick of this controller
* 
*/
AnalogJoystick const& XboxController::GetRightStick() const
{
	return m_rightStick;
}

/*! \brief Gets the value of the left trigger of this controller
* 
* \return A normalized [0.f, 1.f] floating point number representing the left trigger value of this controller
* 
*/
float XboxController::GetLeftTrigger() const
{
	return m_leftTrigger;
}

/*! \brief Gets the value of the right trigger of this controller
* 
* \return A normalized [0.f, 1.f] floating point number representing the right trigger value of this controller
* 
*/
float XboxController::GetRightTrigger() const
{
	return m_rightTrigger;
}

/*! \brief Gets the KeyButtonState for a specific button on this controller
* 
* \param buttonID An XboxButtonID representing the button ID for which KeyButtonState should be returned
* \return A KeyButtonState containing the button specified by the provided buttonID
* 
*/
KeyButtonState const& XboxController::GetButton(XboxButtonID buttonID) const
{
	return m_buttons[buttonID];
}

/*! \brief Gets whether a specific button on this controller is held down or not.
* 
* This method returns true for all frames that the provided button is held down.
* \param buttonID The XboxButtonID for which the method should check if it is held down
* \return A boolean representing whether the button is held down
* 
*/
bool XboxController::IsButtonDown(XboxButtonID buttonID) const
{
	return m_buttons[buttonID].m_isPressed;
}

/*! \brief Gets whether a specific button on this controller was just pressed this frame
* 
* This method returns true only for the one frame when this button was just pressed.
* \param buttonID The XboxButtonID for which the method should check if it was just pressed this frame
* \return A boolean representing whether the button was just pressed this frame
* 
*/
bool XboxController::WasButtonJustPressed(XboxButtonID buttonID) const
{
	return m_buttons[buttonID].m_isPressed && !m_buttons[buttonID].m_wasPressedLastFrame;
}

/*! \brief Gets whether a specific button on this controller was just released this frame
* 
* This method returns true only for the one frame when this button was just released.
* \param buttonID The XboxButtonID for which the method should check if it was just released this frame
* \return A boolean representing whether the button was just released this frame
* 
*/
bool XboxController::WasButtonJustReleased(XboxButtonID buttonID) const
{
	return !m_buttons[buttonID].m_isPressed && m_buttons[buttonID].m_wasPressedLastFrame;
}

/*! \brief Resets all values for this controller
* 
* Resets all buttons to not pressed (for this frame and the previous frame), resets the values of triggers to 0.f and resets the joysticks to the center (Vec2::ZERO) position.
* 
*/
void XboxController::Reset()
{
	// Reset KeyButton states
	for (int keyButtonIndex = 0; keyButtonIndex < NUM_KEYBUTTONS; keyButtonIndex++)
	{
		m_buttons[keyButtonIndex].m_isPressed = false;
		m_buttons[keyButtonIndex].m_wasPressedLastFrame = false;
	}

	// Reset AnalogJoystick states
	m_leftStick.Reset();
	m_rightStick.Reset();

	// Reset ID and triggers
	m_leftTrigger = 0.f;
	m_rightTrigger = 0.f;
}

/*! \brief Gets the physical controller state from XInput and sets variable values accordingly
* 
* If the controller was disconnected or XInput reports an error in getting values from the physical controller, resets the controller values by calling Reset.
* 
*/
void XboxController::Update()
{
	XINPUT_STATE state;

	int xboxControllerErrorCode = XInputGetState(m_id, &state);

	if (xboxControllerErrorCode == ERROR_SUCCESS)
	{
		m_isConnected = true;

		// Update joysticks
		UpdateJoystick(m_leftStick,		state.Gamepad.sThumbLX,		state.Gamepad.sThumbLY);
		UpdateJoystick(m_rightStick,	state.Gamepad.sThumbRX,		state.Gamepad.sThumbRY);

		// Update triggers
		UpdateTrigger(m_leftTrigger,	state.Gamepad.bLeftTrigger);
		UpdateTrigger(m_rightTrigger,	state.Gamepad.bRightTrigger);

		// Update buttons
		UpdateButton	(XBOX_BUTTON_DPAD_UP,			state.Gamepad.wButtons,		XINPUT_GAMEPAD_DPAD_UP);
		UpdateButton	(XBOX_BUTTON_DPAD_DOWN,			state.Gamepad.wButtons,		XINPUT_GAMEPAD_DPAD_DOWN);
		UpdateButton	(XBOX_BUTTON_DPAD_LEFT,			state.Gamepad.wButtons,		XINPUT_GAMEPAD_DPAD_LEFT);
		UpdateButton	(XBOX_BUTTON_DPAD_RIGHT,		state.Gamepad.wButtons,		XINPUT_GAMEPAD_DPAD_RIGHT);
		UpdateButton	(XBOX_BUTTON_START,				state.Gamepad.wButtons,		XINPUT_GAMEPAD_START);
		UpdateButton	(XBOX_BUTTON_BACK,				state.Gamepad.wButtons,		XINPUT_GAMEPAD_BACK);
		UpdateButton	(XBOX_BUTTON_LEFT_THUMB,		state.Gamepad.wButtons,		XINPUT_GAMEPAD_LEFT_THUMB);
		UpdateButton	(XBOX_BUTTON_RIGHT_THUMB,		state.Gamepad.wButtons,		XINPUT_GAMEPAD_RIGHT_THUMB);
		UpdateButton	(XBOX_BUTTON_LEFT_SHOULDER,		state.Gamepad.wButtons,		XINPUT_GAMEPAD_LEFT_SHOULDER);
		UpdateButton	(XBOX_BUTTON_RIGHT_SHOULDER,	state.Gamepad.wButtons,		XINPUT_GAMEPAD_RIGHT_SHOULDER);
		UpdateButton	(XBOX_BUTTON_A,					state.Gamepad.wButtons,		XINPUT_GAMEPAD_A);
		UpdateButton	(XBOX_BUTTON_B,					state.Gamepad.wButtons,		XINPUT_GAMEPAD_B);
		UpdateButton	(XBOX_BUTTON_X,					state.Gamepad.wButtons,		XINPUT_GAMEPAD_X);
		UpdateButton	(XBOX_BUTTON_Y,					state.Gamepad.wButtons,		XINPUT_GAMEPAD_Y);
	}
	else
	{
		Reset();
		m_isConnected = false;
	}
}

/*! \brief Updates the AnalogJoysick raw and deadzone-corrected values based on the raw XY values provided
* 
* The AnalogJoystick for which the values are updated should be passed in (by reference) as the first argument, and raw XY values should be passed in as shorts.
* \param out_joystick The AnalogJoystick for which the raw and deadzone-corrected values should be updated
* \param rawX A short representing the raw X value
* \param rawY A short representing the raw y value
* 
*/
void XboxController::UpdateJoystick(AnalogJoystick& out_joystick, short rawX, short rawY)
{
	float normalizedRawY = RangeMap(rawY, static_cast<float>(SHRT_MIN), static_cast<float>(SHRT_MAX), -1.f, 1.f);
	float normalizedRawX = RangeMap(rawX, static_cast<float>(SHRT_MIN), static_cast<float>(SHRT_MAX), -1.f, 1.f);
	out_joystick.UpdatePosition(normalizedRawX, normalizedRawY);
}

/*! \brief Updates the trigger value based on the raw value provided
* 
* The trigger for which the value is updated should be passed in (by reference) as the first argument, and the new raw value for the trigger should be passed in as a byte/unsigned char.
* \param out_triggerValue A float reference for which the value should be updated
* \param rawValue A byte/unsigned char representing the new raw value for the trigger
* 
*/
void XboxController::UpdateTrigger(float& out_triggerValue, unsigned char rawValue)
{
	out_triggerValue = RangeMap(rawValue, 0.f, static_cast<float>(UCHAR_MAX), 0.f, 1.f);
}

/*! \brief Updates the button state for a specific button on the controller based on the flags received from XInput and the flag for the specific button
* 
* Uses the flag provided for the specific button as a bit-mask and the buttonFlags received from XInput as the bit-field to determine whether the button is pressed.
* \param buttonID The XboxButtonID for which the value should be updated
* \param buttonFlags A short representing the buttons flags for all buttons as received from XInput
* \param buttonFlag A short representing the buttonFlag for the specific button for which the value should be updated
* 
*/
void XboxController::UpdateButton(XboxButtonID buttonID, unsigned short buttonFlags, unsigned short buttonFlag)
{
	KeyButtonState& button = m_buttons[buttonID];
	button.m_wasPressedLastFrame = button.m_isPressed;
	button.m_isPressed = ((buttonFlags & buttonFlag) == buttonFlag);
}
