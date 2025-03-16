#pragma once

#include "Engine/Input/KeyButtonState.hpp"
#include "Engine/Input/AnalogJoystick.hpp"

constexpr int NUM_KEYBUTTONS = 14;

/*! \brief An enumeration for the buttons on an Xbox Controller
* 
* These values do not map directly to the buttons values for the Xbox Controller since two buttons on the controller (black and white) have been removed and only exist on legacy controllers. This enumeration (and by extension the XboxController class) does not support the black and white buttons on legacy controllers.
* 
*/
enum XboxButtonID
{
	XBOX_BUTTON_INVALID = -1,

	XBOX_BUTTON_DPAD_UP,
	XBOX_BUTTON_DPAD_DOWN,
	XBOX_BUTTON_DPAD_LEFT,
	XBOX_BUTTON_DPAD_RIGHT,
	XBOX_BUTTON_START,
	XBOX_BUTTON_BACK,
	XBOX_BUTTON_LEFT_THUMB,
	XBOX_BUTTON_RIGHT_THUMB,
	XBOX_BUTTON_LEFT_SHOULDER,
	XBOX_BUTTON_RIGHT_SHOULDER,
	XBOX_BUTTON_A,
	XBOX_BUTTON_B,
	XBOX_BUTTON_X,
	XBOX_BUTTON_Y,

	XBOX_BUTTON_NUM
};

/*! \brief Representation of an XBox Controller
* 
* Stores the values for buttons, triggers and joysticks on the controller
* 
* \note <b>Deadzone Correction</b>: Analog joysticks on controllers do not reliably reset to the center when they are released. There is often some residual value in these joystick positions, and joysticks often do not always achieve the maximum value when pulled in a particular direction either. We thus use "deadzone-correction" wherein we ignore any values lower than the inner deadzone threshold (treating these values as zero) and mapping any values above the outer deadzone threshold to the maximum value.
* 
*/
class XboxController
{
	friend class InputSystem;

public:
	//! Default constructor for XboxController objects
	XboxController() = default;
	//! Default destructor for XboxController objects
	~XboxController() = default;
	bool IsConnected() const;
	int GetControllerID() const;
	AnalogJoystick const& GetLeftStick() const;
	AnalogJoystick const& GetRightStick() const;
	float GetLeftTrigger() const;
	float GetRightTrigger() const;
	KeyButtonState const& GetButton(XboxButtonID buttonID) const;
	bool IsButtonDown(XboxButtonID buttonID) const;
	bool WasButtonJustPressed(XboxButtonID buttonID) const;
	bool WasButtonJustReleased(XboxButtonID buttonID) const;
	
private:
	void Update();
	void Reset();
	void UpdateJoystick(AnalogJoystick& out_joystick, short rawX, short rawY);
	void UpdateTrigger(float& out_triggerValue, unsigned char rawValue);
	void UpdateButton(XboxButtonID buttonID, unsigned short buttonFlags, unsigned short buttonFlag);

private:
	//! The controller ID assigned to this controller by the InputSystem
	int m_id = -1;

	/*! \brief Whether this controller is connected or not
	* 
	* The InputSystem constructs XboxController objects even in the absence of physical controllers, and we thus keep a check for whether a physical controller corresponding to this XboxController object is connected. When no controller is connected, all controller values are simply reset every frame so as to not interfere with input to the game.
	* 
	*/
	bool m_isConnected = false;
	//! The normalized [0.f, 1.f] analog value of the controller's left trigger
	float m_leftTrigger = 0.f;
	//! The normalized [0.f, 1.f] analog value of the controller's right trigger
	float m_rightTrigger = 0.f;
	//! The KeyButtonState for each digital button on the controller
	KeyButtonState m_buttons[XBOX_BUTTON_NUM];
	//! The left thumstick on the controller
	AnalogJoystick m_leftStick;
	//! The right thumbstick on the controller
	AnalogJoystick m_rightStick;
};
