#pragma once

#if defined (_WIN64)

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Input/AnalogJoystick.hpp"
#include "Engine/Input/KeyButtonState.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"

#include "ThirdParty/openxr/openxr.h"

class OpenXR;

class VRController
{
	friend class OpenXR;

public:
	~VRController() = default;
	VRController(XRHand m_hand);

	void Initialize();
	void Update();
	void Destroy();

	bool IsActive() const { return m_isActive; }


	bool IsSelectButtonPressed() const { return m_selectButton.m_isPressed; }
	bool WasSelectButtonJustPressed() const { return m_selectButton.m_isPressed && !m_selectButton.m_wasPressedLastFrame; }
	bool WasSelectButtonJustReleased() const { return !m_selectButton.m_isPressed && m_selectButton.m_wasPressedLastFrame; }
	bool IsSelectButtonTouched() const { return m_selectTouch.m_isPressed; }
	bool WasSelectButtonJustTouched() const { return m_selectTouch.m_isPressed && !m_selectTouch.m_wasPressedLastFrame; }
	bool WasSelectButtonJustUntouched() const { return !m_selectTouch.m_isPressed && m_selectTouch.m_wasPressedLastFrame; }

	bool IsBackButtonPressed() const { return m_backButton.m_isPressed; }
	bool WasBackButtonJustPressed() const { return m_backButton.m_isPressed && !m_backButton.m_wasPressedLastFrame; }
	bool WasBackButtonJustReleased() const { return !m_backButton.m_isPressed && m_backButton.m_wasPressedLastFrame; }
	bool IsBackButtonTouched() const { return m_backTouch.m_isPressed; }
	bool WasBackButtonJustTouched() const { return m_backTouch.m_isPressed && !m_backTouch.m_wasPressedLastFrame; }
	bool WasBackButtonJustUntouched() const { return !m_backTouch.m_isPressed && m_backTouch.m_wasPressedLastFrame; }

	float GetTrigger() const { return m_trigger; }
	bool IsTriggerTouched() const { return  m_triggerTouch.m_isPressed; }
	bool WasTriggerJustTouched() const { return m_triggerTouch.m_isPressed && !m_triggerTouch.m_wasPressedLastFrame; }
	bool WasTriggerJustUntouched() const { return !m_triggerTouch.m_isPressed && m_triggerTouch.m_wasPressedLastFrame; }
	bool WasTriggerJustPressed() const { return m_triggerButton.m_isPressed && !m_triggerButton.m_wasPressedLastFrame; }
	bool WasTriggerJustReleased() const { return !m_triggerButton.m_isPressed && m_triggerButton.m_wasPressedLastFrame; }

	float GetGrip() const { return m_grip; }
	bool WasGripJustPressed() const { return m_gripButton.m_isPressed && !m_gripButton.m_wasPressedLastFrame; }
	bool WasGripJustReleased() const { return !m_gripButton.m_isPressed && m_gripButton.m_wasPressedLastFrame; }

	bool IsThumbRestTouched() const { return m_thumbRestTouch.m_isPressed; }
	bool WasThumbRestJustTouched() const { return m_thumbRestTouch.m_isPressed && !m_thumbRestTouch.m_wasPressedLastFrame; }
	bool WasThumbRestJustUntouched() const { return !m_thumbRestTouch.m_isPressed && m_thumbRestTouch.m_wasPressedLastFrame; }

	AnalogJoystick GetJoystick() const { return m_joystick; }
	bool IsJoystickTouched() const { return m_joystickTouch.m_isPressed; }
	bool WasJoystickJustTouched() const { return m_joystickTouch.m_isPressed && !m_joystickTouch.m_wasPressedLastFrame; }
	bool WasJoystickJustUntouched() const { return !m_joystickTouch.m_isPressed && m_joystickTouch.m_wasPressedLastFrame; }

	bool IsJoystickPressed() const { return m_joystickButton.m_isPressed; }
	bool WasJoystickJustPressed() const { return m_joystickButton.m_isPressed && !m_joystickButton.m_wasPressedLastFrame; }
	bool WasJoystickJustReleased() const { return !m_joystickButton.m_isPressed && m_joystickButton.m_wasPressedLastFrame; }

	bool IsMenuButtonPressed() const { return m_menuButton.m_isPressed; }
	bool WasMenuButtonJustPressed() const { return m_menuButton.m_isPressed && !m_menuButton.m_wasPressedLastFrame; }
	bool WasMenuButtonJustReleased() const { return !m_menuButton.m_isPressed && m_menuButton.m_wasPressedLastFrame; }

	bool WasAnyKeyJustPressed() const;

	Vec3 GetPosition_iFwd_jLeft_kUp() const;
	EulerAngles GetOrientation_iFwd_jLeft_kUp() const;
	Vec3 GetLinearVelocity_iFwd_jLeft_kUp() const;

	XRHand GetHand() const { return m_hand; }

	void ApplyHapticFeedback(float amplitude, float duration);

private:
	XrActionSet GetActionSet() const { return m_actionSet; }
	void CreateActions();
	void CreateActionBindings(std::vector<XrActionSuggestedBinding>& out_bindings);
	XrPath GetHandPath() const;

private:
	XRHand m_hand = XRHand::NONE;
	std::string m_handStr = "";

	bool m_isActive = false;

	XrPosef m_pose = {{0.f, 0.f, 0.f, 1.f}, {0.f, 0.f, 0.f}};
	XrVector3f m_linearVelocity = {0.f, 0.f, 0.f};
	AnalogJoystick m_joystick;
	KeyButtonState m_joystickTouch;
	KeyButtonState m_joystickButton;
	KeyButtonState m_selectButton; // The "select" button is the A button on the right controller and the X button on the left controller
	KeyButtonState m_selectTouch;
	KeyButtonState m_backButton; // The "back" button is the B button on the right controller and the Y button on the left controller
	KeyButtonState m_backTouch;
	float m_trigger = 0.f;
	KeyButtonState m_triggerButton;
	KeyButtonState m_triggerTouch;
	float m_grip = 0.f;
	KeyButtonState m_gripButton;
	KeyButtonState m_thumbRestTouch;
	KeyButtonState m_menuButton;

	// OpenXR variables
	static const inline std::string OCULUS_CONTROLLER_PATH_STR = "/interaction_profiles/oculus/touch_controller";
	static constexpr int NUM_ACTION_BINDINGS = 13;
	static const inline XrPosef IDENTITY_POSE = {{0, 0, 0, 1}, {0, 0, 0}};

	std::string m_selectActionPathStr = "";
	std::string m_selectTouchActionPathStr = "";
	std::string m_backActionPathStr = "";
	std::string m_backTouchActionPathStr = "";
	std::string m_triggerActionPathStr = "";
	std::string m_triggerTouchActionPathStr = "";
	std::string m_gripActionPathStr = "";
	std::string m_joystickActionPathStr = "";
	std::string m_joystickTouchActionPathStr = "";
	std::string m_poseActionPathStr = "";
	std::string m_thumbRestTouchActionPathStr = "";
	std::string m_joystickClickPathStr = "";
	std::string m_menuActionPathStr = "";
	std::string m_hapticActionPathStr = "";

	XrActionSet m_actionSet;
	XrAction m_selectAction = {};
	XrAction m_selectTouchAction = {};
	XrAction m_backAction = {};
	XrAction m_backTouchAction = {};
	XrAction m_triggerAction = {};
	XrAction m_triggerTouchAction = {};
	XrAction m_gripAction = {};
	XrAction m_joystickAction = {};
	XrAction m_joystickTouchAction = {};
	XrAction m_joystickClickAction = {};
	XrAction m_poseAction = {};
	XrAction m_thumbRestTouchAction = {};
	XrAction m_menuAction = {};
	XrSpace m_controllerSpace = {};
	XrAction m_hapticAction = {};
};

#endif
