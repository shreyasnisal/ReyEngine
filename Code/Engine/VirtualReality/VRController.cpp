#include "Engine/VirtualReality/VRController.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/VirtualReality/OpenXR.hpp"

#include "ThirdParty/openxr/openxr.h"


VRController::VRController(XRHand hand)
	: m_hand (hand)
{
	if (m_hand == XRHand::LEFT)
	{
		m_handStr = "left";
		m_selectActionPathStr = "/user/hand/left/input/x/click";
		m_selectTouchActionPathStr = "/user/hand/left/input/x/touch";
		m_backActionPathStr = "/user/hand/left/input/y/click";
		m_backTouchActionPathStr = "/user/hand/left/input/y/touch";
		m_joystickActionPathStr = "/user/hand/left/input/thumbstick";
		m_joystickTouchActionPathStr = "/user/hand/left/input/thumbstick/touch";
		m_gripActionPathStr = "/user/hand/left/input/squeeze/value";
		m_triggerActionPathStr = "/user/hand/left/input/trigger/value";
		m_triggerTouchActionPathStr = "/user/hand/left/input/trigger/touch";
		m_poseActionPathStr = "/user/hand/left/input/aim/pose";
		m_thumbRestTouchActionPathStr = "/user/hand/left/input/thumbrest/touch";
		m_joystickClickPathStr = "/user/hand/left/input/thumbstick/click";
		m_menuActionPathStr = "/user/hand/left/input/menu/click";
		m_hapticActionPathStr = "/user/hand/left/output/haptic";
	}
	else if (m_hand == XRHand::RIGHT)
	{
		m_handStr = "right";
		m_selectActionPathStr = "/user/hand/right/input/a/click";
		m_selectTouchActionPathStr = "/user/hand/right/input/a/touch";
		m_backActionPathStr = "/user/hand/right/input/b/click";
		m_backTouchActionPathStr = "/user/hand/right/input/b/touch";
		m_joystickActionPathStr = "/user/hand/right/input/thumbstick";
		m_joystickTouchActionPathStr = "/user/hand/right/input/thumbstick/touch";
		m_gripActionPathStr = "/user/hand/right/input/squeeze/value";
		m_triggerActionPathStr = "/user/hand/right/input/trigger/value";
		m_triggerTouchActionPathStr = "/user/hand/right/input/trigger/touch";
		m_poseActionPathStr = "/user/hand/right/input/aim/pose";
		m_thumbRestTouchActionPathStr = "/user/hand/right/input/thumbrest/touch";
		m_joystickClickPathStr = "/user/hand/right/input/thumbstick/click";
		m_hapticActionPathStr = "/user/hand/right/output/haptic";
	}
	else
	{
		ERROR_AND_DIE("Attempted to construct a VR controller with an invalid hand index!");
	}
}

void VRController::Initialize()
{
	XrResult result;
	XrInstance xrInstance = g_openXR->GetInstance();
	XrSession xrSession = g_openXR->GetSession();

	// Create action set
	XrActionSetCreateInfo actionSetInfo = { XR_TYPE_ACTION_SET_CREATE_INFO };
	strcpy_s(actionSetInfo.actionSetName, (m_handStr + "_actions").c_str());
	strcpy_s(actionSetInfo.localizedActionSetName, (m_handStr + " Actions").c_str());
	actionSetInfo.priority = 0;
	result = xrCreateActionSet(xrInstance, &actionSetInfo, &m_actionSet);
	if (result != XR_SUCCESS)
	{
		ERROR_AND_DIE(Stringf("Could not create action set \"%s\"!", actionSetInfo.actionSetName));
	}

	CreateActions();

	// Create space for pose actions
	XrActionSpaceCreateInfo actionSpaceInfo = { XR_TYPE_ACTION_SPACE_CREATE_INFO };
	actionSpaceInfo.action = m_poseAction;
	actionSpaceInfo.poseInActionSpace = m_pose;
	actionSpaceInfo.subactionPath = GetHandPath();
	xrCreateActionSpace(xrSession, &actionSpaceInfo, &m_controllerSpace);
}

Vec3 VRController::GetPosition_iFwd_jLeft_kUp() const
{
	return Vec3(-m_pose.position.z, -m_pose.position.x, m_pose.position.y);
}

EulerAngles VRController::GetOrientation_iFwd_jLeft_kUp() const
{
	return GetEulerAnglesFromQuaternion(-m_pose.orientation.z, -m_pose.orientation.x, m_pose.orientation.y, m_pose.orientation.w);
}

Vec3 VRController::GetLinearVelocity_iFwd_jLeft_kUp() const
{
	return Vec3(-m_linearVelocity.z, -m_linearVelocity.x, m_linearVelocity.y);
}

void VRController::ApplyHapticFeedback(float amplitude, float duration)
{
	XrHapticVibration hapticVibration = { XR_TYPE_HAPTIC_VIBRATION };
	hapticVibration.amplitude = amplitude;
	// Duration must be passed in NANO seconds, no idea why this isn't just milliseconds or seconds!
	hapticVibration.duration = int64_t(duration * 1000000000);
	hapticVibration.frequency = XR_FREQUENCY_UNSPECIFIED;

	XrHapticActionInfo hapticActionInfo = { XR_TYPE_HAPTIC_ACTION_INFO };
	hapticActionInfo.subactionPath = GetHandPath();
	hapticActionInfo.action = m_hapticAction;

	xrApplyHapticFeedback(g_openXR->GetSession(), &hapticActionInfo, (XrHapticBaseHeader*)&hapticVibration);
}

void VRController::CreateActions()
{
	XrPath handPath = GetHandPath();

	{
		// Select click boolean action
		XrActionCreateInfo actionInfo = { XR_TYPE_ACTION_CREATE_INFO };
		actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
		actionInfo.countSubactionPaths = 1;
		actionInfo.subactionPaths = &handPath;
		strcpy_s(actionInfo.actionName, (m_handStr + "_select_click").c_str());
		strcpy_s(actionInfo.localizedActionName, (m_handStr + " Select Button").c_str());
		XrResult result = xrCreateAction(m_actionSet, &actionInfo, &m_selectAction);
		if (result != XR_SUCCESS)
		{
			ERROR_AND_DIE(Stringf("Could not create action \"%s\"!", actionInfo.actionName));
		}
	}

	{
		// Select touch boolean action
		XrActionCreateInfo actionInfo = { XR_TYPE_ACTION_CREATE_INFO };
		actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
		actionInfo.countSubactionPaths = 1;
		actionInfo.subactionPaths = &handPath;
		strcpy_s(actionInfo.actionName, (m_handStr + "_select_touch").c_str());
		strcpy_s(actionInfo.localizedActionName, (m_handStr + " Select Touch").c_str());
		XrResult result = xrCreateAction(m_actionSet, &actionInfo, &m_selectTouchAction);
		if (result != XR_SUCCESS)
		{
			ERROR_AND_DIE(Stringf("Could not create action \"%s\"!", actionInfo.actionName));
		}
	}

	{
		// Back click boolean action
		XrActionCreateInfo actionInfo = { XR_TYPE_ACTION_CREATE_INFO };
		actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
		actionInfo.countSubactionPaths = 1;
		actionInfo.subactionPaths = &handPath;
		strcpy_s(actionInfo.actionName, (m_handStr + "_back_click").c_str());
		strcpy_s(actionInfo.localizedActionName, (m_handStr + " Back Click").c_str());
		XrResult result = xrCreateAction(m_actionSet, &actionInfo, &m_backAction);
		if (result != XR_SUCCESS)
		{
			ERROR_AND_DIE(Stringf("Could not create action \"%s\"!", actionInfo.actionName));
		}
	}

	{
		// Back touch boolean action
		XrActionCreateInfo actionInfo = { XR_TYPE_ACTION_CREATE_INFO };
		actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
		actionInfo.countSubactionPaths = 1;
		actionInfo.subactionPaths = &handPath;
		strcpy_s(actionInfo.actionName, (m_handStr + "_back_touch").c_str());
		strcpy_s(actionInfo.localizedActionName, (m_handStr + " Back Touch").c_str());
		XrResult result = xrCreateAction(m_actionSet, &actionInfo, &m_backTouchAction);
		if (result != XR_SUCCESS)
		{
			ERROR_AND_DIE(Stringf("Could not create action \"%s\"!", actionInfo.actionName));
		}
	}

	{
		// Joystick Vec2 action
		XrActionCreateInfo actionInfo = { XR_TYPE_ACTION_CREATE_INFO };
		actionInfo.actionType = XR_ACTION_TYPE_VECTOR2F_INPUT;
		actionInfo.countSubactionPaths = 1;
		actionInfo.subactionPaths = &handPath;
		strcpy_s(actionInfo.actionName, (m_handStr + "_joystick_vector").c_str());
		strcpy_s(actionInfo.localizedActionName, (m_handStr + " Joystick Vector").c_str());
		XrResult result = xrCreateAction(m_actionSet, &actionInfo, &m_joystickAction);
		if (result != XR_SUCCESS)
		{
			ERROR_AND_DIE(Stringf("Could not create action \"%s\"!", actionInfo.actionName));
		}
	}

	{
		// Joystick touch action
		XrActionCreateInfo actionInfo = { XR_TYPE_ACTION_CREATE_INFO };
		actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
		actionInfo.countSubactionPaths = 1;
		actionInfo.subactionPaths = &handPath;
		strcpy_s(actionInfo.actionName, (m_handStr + "_joystick_touch").c_str());
		strcpy_s(actionInfo.localizedActionName, (m_handStr + " Joystick Touch").c_str());
		XrResult result = xrCreateAction(m_actionSet, &actionInfo, &m_joystickTouchAction);
		if (result != XR_SUCCESS)
		{
			ERROR_AND_DIE(Stringf("Could not create action \"%s\"!", actionInfo.actionName));
		}
	}

	{
		// Joystick click boolean action
		XrActionCreateInfo actionInfo = { XR_TYPE_ACTION_CREATE_INFO };
		actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
		actionInfo.countSubactionPaths = 1;
		actionInfo.subactionPaths = &handPath;
		strcpy_s(actionInfo.actionName, (m_handStr + "_joystick_click").c_str());
		strcpy_s(actionInfo.localizedActionName, (m_handStr + " Joystick Click").c_str());
		XrResult result = xrCreateAction(m_actionSet, &actionInfo, &m_joystickClickAction);
		if (result != XR_SUCCESS)
		{
			ERROR_AND_DIE(Stringf("Could not create action \"%s\"!", actionInfo.actionName));
		}
	}

	{
		// Trigger float action
		XrActionCreateInfo actionInfo = { XR_TYPE_ACTION_CREATE_INFO };
		actionInfo.actionType = XR_ACTION_TYPE_FLOAT_INPUT;
		actionInfo.countSubactionPaths = 1;
		actionInfo.subactionPaths = &handPath;
		strcpy_s(actionInfo.actionName, (m_handStr + "_trigger_value").c_str());
		strcpy_s(actionInfo.localizedActionName, (m_handStr + " Trigger Value").c_str());
		XrResult result = xrCreateAction(m_actionSet, &actionInfo, &m_triggerAction);
		if (result != XR_SUCCESS)
		{
			ERROR_AND_DIE(Stringf("Could not create action \"%s\"!", actionInfo.actionName));
		}
	}

	{
		// Trigger touch action
		XrActionCreateInfo actionInfo = { XR_TYPE_ACTION_CREATE_INFO };
		actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
		actionInfo.countSubactionPaths = 1;
		actionInfo.subactionPaths = &handPath;
		strcpy_s(actionInfo.actionName, (m_handStr + "_trigger_touch").c_str());
		strcpy_s(actionInfo.localizedActionName, (m_handStr + " Trigger Touch").c_str());
		XrResult result = xrCreateAction(m_actionSet, &actionInfo, &m_triggerTouchAction);
		if (result != XR_SUCCESS)
		{
			ERROR_AND_DIE(Stringf("Could not create action \"%s\"!", actionInfo.actionName));
		}
	}

	{
		// Grip float action
		XrActionCreateInfo actionInfo = { XR_TYPE_ACTION_CREATE_INFO };
		actionInfo.actionType = XR_ACTION_TYPE_FLOAT_INPUT;
		actionInfo.countSubactionPaths = 1;
		actionInfo.subactionPaths = &handPath;
		strcpy_s(actionInfo.actionName, (m_handStr + "_grip_value").c_str());
		strcpy_s(actionInfo.localizedActionName, (m_handStr + " Grip Value").c_str());
		XrResult result = xrCreateAction(m_actionSet, &actionInfo, &m_gripAction);
		if (result != XR_SUCCESS)
		{
			ERROR_AND_DIE(Stringf("Could not create action \"%s\"!", actionInfo.actionName));
		}
	}

	{
		// Pose action
		XrActionCreateInfo actionInfo = { XR_TYPE_ACTION_CREATE_INFO };
		actionInfo.actionType = XR_ACTION_TYPE_POSE_INPUT;
		actionInfo.countSubactionPaths = 1;
		actionInfo.subactionPaths = &handPath;
		strcpy_s(actionInfo.actionName, (m_handStr + "_pose").c_str());
		strcpy_s(actionInfo.localizedActionName, (m_handStr + " Pose").c_str());
		XrResult result = xrCreateAction(m_actionSet, &actionInfo, &m_poseAction);
		if (result != XR_SUCCESS)
		{
			ERROR_AND_DIE(Stringf("Could not create action \"%s\"!", actionInfo.actionName));
		}
	}

	{
		// Thumbrest touch action
		XrActionCreateInfo actionInfo = { XR_TYPE_ACTION_CREATE_INFO };
		actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
		actionInfo.countSubactionPaths = 1;
		actionInfo.subactionPaths = &handPath;
		strcpy_s(actionInfo.actionName, (m_handStr + "_thumbrest_touch").c_str());
		strcpy_s(actionInfo.localizedActionName, (m_handStr + " ThumbRest Touch").c_str());
		XrResult result = xrCreateAction(m_actionSet, &actionInfo, &m_thumbRestTouchAction);
		if (result != XR_SUCCESS)
		{
			ERROR_AND_DIE(Stringf("Could not create action \"%s\"!", actionInfo.actionName));
		}
	}

	{
		// Haptic output action
		XrActionCreateInfo actionInfo = { XR_TYPE_ACTION_CREATE_INFO };
		actionInfo.actionType = XR_ACTION_TYPE_VIBRATION_OUTPUT;
		actionInfo.countSubactionPaths = 1;
		actionInfo.subactionPaths = &handPath;
		strcpy_s(actionInfo.actionName, (m_handStr + "_haptic_vibration").c_str());
		strcpy_s(actionInfo.localizedActionName, (m_handStr + " Haptic Vibration").c_str());
		XrResult result = xrCreateAction(m_actionSet, &actionInfo, &m_hapticAction);
		if (result != XR_SUCCESS)
		{
			ERROR_AND_DIE(Stringf("Could not create action \"%s\"!", actionInfo.actionName));
		}
	}

	if (m_hand == XRHand::LEFT)
	{
		// Menu Click action
		XrActionCreateInfo actionInfo = { XR_TYPE_ACTION_CREATE_INFO };
		actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
		actionInfo.countSubactionPaths = 1;
		actionInfo.subactionPaths = &handPath;
		strcpy_s(actionInfo.actionName, (m_handStr + "_menu_click").c_str());
		strcpy_s(actionInfo.localizedActionName, (m_handStr + " Menu Click").c_str());
		XrResult result = xrCreateAction(m_actionSet, &actionInfo, &m_menuAction);
		if (result != XR_SUCCESS)
		{
			ERROR_AND_DIE(Stringf("Could not create action \"%s\"!", actionInfo.actionName));
		}
	}
}

void VRController::CreateActionBindings(std::vector<XrActionSuggestedBinding>& out_bindings)
{
	XrResult result;
	XrInstance xrInstance = g_openXR->GetInstance();


	XrPath selectClickPath;
	result = xrStringToPath(xrInstance, m_selectActionPathStr.c_str(), &selectClickPath);
	if (result != XR_SUCCESS)
	{
		ERROR_AND_DIE(Stringf("Could not create path for \"%s\"", m_selectActionPathStr.c_str()));
	}
	out_bindings.push_back({m_selectAction, selectClickPath});


	XrPath selectTouchPath;
	result = xrStringToPath(xrInstance, m_selectTouchActionPathStr.c_str(), &selectTouchPath);
	if (result != XR_SUCCESS)
	{
		ERROR_AND_DIE(Stringf("Could not create path for \"%s\"", m_selectTouchActionPathStr.c_str()));
	}
	out_bindings.push_back({ m_selectTouchAction, selectTouchPath });


	XrPath backClickPath;
	result = xrStringToPath(xrInstance, m_backActionPathStr.c_str(), &backClickPath);
	if (result != XR_SUCCESS)
	{
		ERROR_AND_DIE(Stringf("Could not create path for \"%s\"", m_backActionPathStr.c_str()));
	}
	out_bindings.push_back({m_backAction, backClickPath});


	XrPath backTouchPath;
	result = xrStringToPath(xrInstance, m_backTouchActionPathStr.c_str(), &backTouchPath);
	if (result != XR_SUCCESS)
	{
		ERROR_AND_DIE(Stringf("Could not create path for \"%s\"", m_backTouchActionPathStr.c_str()));
	}
	out_bindings.push_back({ m_backTouchAction, backTouchPath });


	XrPath triggerPath;
	result = xrStringToPath(xrInstance, m_triggerActionPathStr.c_str(), &triggerPath);
	if (result != XR_SUCCESS)
	{
		ERROR_AND_DIE(Stringf("Could not create path for \"%s\"", m_triggerActionPathStr.c_str()));
	}
	out_bindings.push_back({m_triggerAction, triggerPath});


	XrPath triggerTouchPath;
	result = xrStringToPath(xrInstance, m_triggerTouchActionPathStr.c_str(), &triggerTouchPath);
	if (result != XR_SUCCESS)
	{
		ERROR_AND_DIE(Stringf("Could not create path for \"%s\"", m_triggerTouchActionPathStr.c_str()));
	}
	out_bindings.push_back({ m_triggerTouchAction, triggerTouchPath });


	XrPath gripPath;
	result = xrStringToPath(xrInstance, m_gripActionPathStr.c_str(), &gripPath);
	if (result != XR_SUCCESS)
	{
		ERROR_AND_DIE(Stringf("Could not create path for \"%s\"", m_gripActionPathStr.c_str()));
	}
	out_bindings.push_back({m_gripAction, gripPath});


	XrPath joystickPath;
	result = xrStringToPath(xrInstance, m_joystickActionPathStr.c_str(), &joystickPath);
	if (result != XR_SUCCESS)
	{
		ERROR_AND_DIE(Stringf("Could not create path for \"%s\"", m_joystickActionPathStr.c_str()));
	}
	out_bindings.push_back({m_joystickAction, joystickPath});


	XrPath joystickTouchPath;
	result = xrStringToPath(xrInstance, m_joystickTouchActionPathStr.c_str(), &joystickTouchPath);
	if (result != XR_SUCCESS)
	{
		ERROR_AND_DIE(Stringf("Could not create path for \"%s\"", m_joystickTouchActionPathStr.c_str()));
	}
	out_bindings.push_back({ m_joystickTouchAction, joystickTouchPath });


	XrPath joystickClickPath;
	result = xrStringToPath(xrInstance, m_joystickClickPathStr.c_str(), &joystickClickPath);
	if (result != XR_SUCCESS)
	{
		ERROR_AND_DIE(Stringf("Could not create path for \"%s\"", m_joystickClickPathStr.c_str()));
	}
	out_bindings.push_back({ m_joystickClickAction, joystickClickPath });


	XrPath posePath;
	result = xrStringToPath(xrInstance, m_poseActionPathStr.c_str(), &posePath);
	if (result != XR_SUCCESS)
	{
		ERROR_AND_DIE(Stringf("Could not create path for \"%s\"", m_poseActionPathStr.c_str()));
	}
	out_bindings.push_back({ m_poseAction, posePath });


	XrPath thumbrestTouchPath;
	result = xrStringToPath(xrInstance, m_thumbRestTouchActionPathStr.c_str(), &thumbrestTouchPath);
	if (result != XR_SUCCESS)
	{
		ERROR_AND_DIE(Stringf("Could not create path for \"%s\"", m_thumbRestTouchActionPathStr.c_str()));
	}
	out_bindings.push_back({ m_thumbRestTouchAction, thumbrestTouchPath });

	XrPath hapticVibrationPath;
	result = xrStringToPath(xrInstance, m_hapticActionPathStr.c_str(), &hapticVibrationPath);
	if (result != XR_SUCCESS)
	{
		ERROR_AND_DIE(Stringf("Could not create path for \"%s\"", m_hapticActionPathStr.c_str()));
	}
	out_bindings.push_back({ m_hapticAction, hapticVibrationPath });

	if (m_hand == XRHand::LEFT)
	{
		XrPath menuActionPath;
		result = xrStringToPath(xrInstance, m_menuActionPathStr.c_str(), &menuActionPath);
		if (result != XR_SUCCESS)
		{
			ERROR_AND_DIE(Stringf("Could not create path for \"%s\"", m_thumbRestTouchActionPathStr.c_str()));
		}
		out_bindings.push_back({ m_menuAction, menuActionPath });
	}
}

XrPath VRController::GetHandPath() const
{
	XrInstance xrInstance = g_openXR->GetInstance();
	XrPath handPath;
	xrStringToPath(xrInstance, ("/user/hand/" + m_handStr).c_str(), &handPath);

	return handPath;
}

void VRController::Update()
{
	m_isActive = false;

	XrPath handPath = GetHandPath();
	XrSession xrSession = g_openXR->GetSession();
	XrSpace xrAppSpace = g_openXR->GetAppSpace();

	// Select click action
	XrActionStateGetInfo selectClickGetInfo = { XR_TYPE_ACTION_STATE_GET_INFO };
	selectClickGetInfo.action = m_selectAction;
	selectClickGetInfo.subactionPath = handPath;
	XrActionStateBoolean selectClickState = { XR_TYPE_ACTION_STATE_BOOLEAN };
	xrGetActionStateBoolean(xrSession, &selectClickGetInfo, &selectClickState);
	if (selectClickState.isActive)
	{
		m_selectButton.m_wasPressedLastFrame = m_selectButton.m_isPressed;
		m_selectButton.m_isPressed = selectClickState.currentState;
	}

	// Select touch action
	XrActionStateGetInfo selectTouchGetInfo = { XR_TYPE_ACTION_STATE_GET_INFO };
	selectTouchGetInfo.action = m_selectTouchAction;
	selectTouchGetInfo.subactionPath = handPath;
	XrActionStateBoolean selectTouchState = { XR_TYPE_ACTION_STATE_BOOLEAN };
	xrGetActionStateBoolean(xrSession, &selectTouchGetInfo, &selectTouchState);
	if (selectTouchState.isActive)
	{
		m_selectTouch.m_wasPressedLastFrame = m_selectTouch.m_isPressed;
		m_selectTouch.m_isPressed = selectTouchState.currentState;
	}

	// Back click action
	XrActionStateGetInfo backButtonGetInfo = { XR_TYPE_ACTION_STATE_GET_INFO };
	backButtonGetInfo.action = m_backAction;
	backButtonGetInfo.subactionPath = handPath;
	XrActionStateBoolean backButtonState = { XR_TYPE_ACTION_STATE_BOOLEAN };
	xrGetActionStateBoolean(xrSession, &backButtonGetInfo, &backButtonState);
	if (backButtonState.isActive)
	{
		m_backButton.m_wasPressedLastFrame = m_backButton.m_isPressed;
		m_backButton.m_isPressed = backButtonState.currentState;
	}

	// Back touch action
	XrActionStateGetInfo backTouchGetInfo = { XR_TYPE_ACTION_STATE_GET_INFO };
	backTouchGetInfo.action = m_backTouchAction;
	backTouchGetInfo.subactionPath = handPath;
	XrActionStateBoolean backTouchState = { XR_TYPE_ACTION_STATE_BOOLEAN };
	xrGetActionStateBoolean(xrSession, &backTouchGetInfo, &backTouchState);
	if (backTouchState.isActive)
	{
		m_backTouch.m_wasPressedLastFrame = m_backTouch.m_isPressed;
		m_backTouch.m_isPressed = backTouchState.currentState;
	}

	// Joystick vector action
	XrActionStateGetInfo joystickVectorGetInfo = { XR_TYPE_ACTION_STATE_GET_INFO };
	joystickVectorGetInfo.action = m_joystickAction;
	joystickVectorGetInfo.subactionPath = handPath;
	XrActionStateVector2f joystickVector = { XR_TYPE_ACTION_STATE_VECTOR2F };
	xrGetActionStateVector2f(xrSession, &joystickVectorGetInfo, &joystickVector);
	if (joystickVector.isActive)
	{
		Vec2 joystickVec2 = Vec2(joystickVector.currentState.x, joystickVector.currentState.y).GetNormalized();
		m_joystick.UpdatePosition(joystickVec2.x, joystickVec2.y);
	}

	// Joystick touch action
	XrActionStateGetInfo joystickTouchGetInfo = { XR_TYPE_ACTION_STATE_GET_INFO };
	joystickTouchGetInfo.action = m_joystickTouchAction;
	joystickTouchGetInfo.subactionPath = handPath;
	XrActionStateBoolean joystickTouchState = { XR_TYPE_ACTION_STATE_BOOLEAN };
	xrGetActionStateBoolean(xrSession, &joystickTouchGetInfo, &joystickTouchState);
	if (joystickTouchState.isActive)
	{
		m_joystickTouch.m_wasPressedLastFrame = m_joystickTouch.m_isPressed;
		m_joystickTouch.m_isPressed = joystickTouchState.currentState;
	}

	// Joystick click action
	XrActionStateGetInfo joystickClickGetInfo = { XR_TYPE_ACTION_STATE_GET_INFO };
	joystickClickGetInfo.action = m_joystickClickAction;
	joystickClickGetInfo.subactionPath = handPath;
	XrActionStateBoolean joystickClickState = { XR_TYPE_ACTION_STATE_BOOLEAN };
	xrGetActionStateBoolean(xrSession, &joystickClickGetInfo, &joystickClickState);
	if (joystickClickState.isActive)
	{
		m_joystickButton.m_wasPressedLastFrame = m_joystickButton.m_isPressed;
		m_joystickButton.m_isPressed = joystickClickState.currentState;
	}

	// Trigger value action
	XrActionStateGetInfo triggerValueGetInfo = { XR_TYPE_ACTION_STATE_GET_INFO };
	triggerValueGetInfo.action = m_triggerAction;
	triggerValueGetInfo.subactionPath = handPath;
	XrActionStateFloat triggerValueState = { XR_TYPE_ACTION_STATE_FLOAT };
	xrGetActionStateFloat(xrSession, &triggerValueGetInfo, &triggerValueState);
	if (triggerValueState.isActive)
	{
		m_trigger = triggerValueState.currentState;
		m_triggerButton.m_wasPressedLastFrame = m_triggerButton.m_isPressed;
		m_triggerButton.m_isPressed = (m_trigger > 0.f);
	}

	// Trigger touch action
	XrActionStateGetInfo triggerTouchGetInfo = { XR_TYPE_ACTION_STATE_GET_INFO };
	triggerTouchGetInfo.action = m_triggerTouchAction;
	triggerTouchGetInfo.subactionPath = handPath;
	XrActionStateBoolean triggerTouchState = { XR_TYPE_ACTION_STATE_BOOLEAN };
	xrGetActionStateBoolean(xrSession, &triggerTouchGetInfo, &triggerTouchState);
	if (triggerTouchState.isActive)
	{
		m_triggerTouch.m_wasPressedLastFrame = m_triggerTouch.m_isPressed;
		m_triggerTouch.m_isPressed = triggerTouchState.currentState;
	}

	// Grip value action
	XrActionStateGetInfo gripValueGetInfo = { XR_TYPE_ACTION_STATE_GET_INFO };
	gripValueGetInfo.action = m_gripAction;
	gripValueGetInfo.subactionPath = handPath;
	XrActionStateFloat gripValueState = { XR_TYPE_ACTION_STATE_FLOAT };
	xrGetActionStateFloat(xrSession, &gripValueGetInfo, &gripValueState);
	if (gripValueState.isActive)
	{
		m_grip = gripValueState.currentState;
		m_gripButton.m_wasPressedLastFrame = m_gripButton.m_isPressed;
		m_gripButton.m_isPressed = (m_grip > 0.f);
	}

	// Pose action
	XrActionStateGetInfo poseGetInfo = { XR_TYPE_ACTION_STATE_GET_INFO };
	poseGetInfo.action = m_poseAction;
	poseGetInfo.subactionPath = handPath;
	XrActionStatePose poseState = { XR_TYPE_ACTION_STATE_POSE };
	xrGetActionStatePose(xrSession, &poseGetInfo, &poseState);
	if (poseState.isActive)
	{
		m_isActive = true;
		
		XrTime predictedTime = g_openXR->GetPredictedFrameTime();
		XrSpaceLocation spaceRelation = { XR_TYPE_SPACE_LOCATION };
		XrSpaceVelocity spaceVelocity = { XR_TYPE_SPACE_VELOCITY };
		spaceRelation.next = &spaceVelocity;
		xrLocateSpace(m_controllerSpace, xrAppSpace, predictedTime, &spaceRelation);
		if (spaceRelation.locationFlags & XR_SPACE_LOCATION_POSITION_VALID_BIT)
		{
			m_pose = spaceRelation.pose;
		}
		if (spaceRelation.locationFlags & XR_SPACE_VELOCITY_LINEAR_VALID_BIT)
		{
			m_linearVelocity = spaceVelocity.linearVelocity;
		}
	}

	// ThumbRest touch action
	XrActionStateGetInfo thumbRestTouchGetInfo = { XR_TYPE_ACTION_STATE_GET_INFO };
	thumbRestTouchGetInfo.action = m_thumbRestTouchAction;
	thumbRestTouchGetInfo.subactionPath = handPath;
	XrActionStateBoolean thumbRestTouchState = { XR_TYPE_ACTION_STATE_BOOLEAN };
	xrGetActionStateBoolean(xrSession, &thumbRestTouchGetInfo, &thumbRestTouchState);
	if (thumbRestTouchState.isActive)
	{
		m_thumbRestTouch.m_wasPressedLastFrame = m_thumbRestTouch.m_isPressed;
		m_thumbRestTouch.m_isPressed = thumbRestTouchState.currentState;
	}

	if (m_hand == XRHand::LEFT)
	{
		// Menu action
		XrActionStateGetInfo menuClickGetInfo = { XR_TYPE_ACTION_STATE_GET_INFO };
		menuClickGetInfo.action = m_menuAction;
		menuClickGetInfo.subactionPath = handPath;
		XrActionStateBoolean menuButtonState = { XR_TYPE_ACTION_STATE_BOOLEAN };
		xrGetActionStateBoolean(xrSession, &menuClickGetInfo, &menuButtonState);
		if (menuButtonState.isActive)
		{
			m_menuButton.m_wasPressedLastFrame = m_menuButton.m_isPressed;
			m_menuButton.m_isPressed = menuButtonState.currentState;
		}
	}
}

void VRController::Destroy()
{
	if (m_controllerSpace != XR_NULL_HANDLE)
	{
		xrDestroySpace(m_controllerSpace);
	}
}

bool VRController::WasAnyKeyJustPressed() const
{
	if (m_selectButton.m_isPressed && !m_selectButton.m_wasPressedLastFrame)
	{
		return true;
	}

	if (m_backButton.m_isPressed && !m_selectButton.m_wasPressedLastFrame)
	{
		return true;
	}

	if (m_triggerButton.m_isPressed && !m_triggerButton.m_wasPressedLastFrame)
	{
		return true;
	}

	if (m_gripButton.m_isPressed && !m_triggerButton.m_wasPressedLastFrame)
	{
		return true;
	}

	return false;
}
