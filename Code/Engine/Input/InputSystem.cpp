#include "Engine/Input/InputSystem.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

unsigned char const KEYCODE_INVALID = 0;
unsigned char const KEYCODE_F1 = VK_F1;
unsigned char const KEYCODE_F2 = VK_F2;
unsigned char const KEYCODE_F3 = VK_F3;
unsigned char const KEYCODE_F4 = VK_F4;
unsigned char const KEYCODE_F5 = VK_F5;
unsigned char const KEYCODE_F6 = VK_F6;
unsigned char const KEYCODE_F7 = VK_F7;
unsigned char const KEYCODE_F8 = VK_F8;
unsigned char const KEYCODE_F9 = VK_F9;
unsigned char const KEYCODE_F10 = VK_F10;
unsigned char const KEYCODE_F11 = VK_F11;
unsigned char const KEYCODE_ESC = VK_ESCAPE;
unsigned char const KEYCODE_UPARROW = VK_UP;
unsigned char const KEYCODE_DOWNARROW = VK_DOWN;
unsigned char const KEYCODE_LEFTARROW = VK_LEFT;
unsigned char const KEYCODE_RIGHTARROW = VK_RIGHT;
unsigned char const KEYCODE_SPACE = VK_SPACE;
unsigned char const KEYCODE_ENTER = VK_RETURN;
unsigned char const KEYCODE_LMB = VK_LBUTTON;
unsigned char const KEYCODE_RMB = VK_RBUTTON;
unsigned char const KEYCODE_TILDE = VK_OEM_3;
unsigned char const KEYCODE_BACKSPACE = VK_BACK;
unsigned char const KEYCODE_HOME = VK_HOME;
unsigned char const KEYCODE_END = VK_END;
unsigned char const KEYCODE_DELETE = VK_DELETE;
unsigned char const KEYCODE_PERIOD = VK_OEM_PERIOD;
unsigned char const KEYCODE_COMMA = VK_OEM_COMMA;
unsigned char const KEYCODE_BRACKETOPEN = VK_OEM_4;
unsigned char const KEYCODE_BRACKETCLOSE = VK_OEM_6;
unsigned char const KEYCODE_LEFT_ALT = VK_MENU;
unsigned char const KEYCODE_CTRL = VK_CONTROL;

/*! \brief Event callback function for handling key pressed events
* 
* Retrieves the keycode from the #EventArgs passed and calls the InputSystem#HandleKeyPressed method with the keycode.
* \param args An #EventArgs object containing the arguments required for this function. Must contain a value for KeyCode
* \return A bool indicating whether the event was consumed
* 
*/
bool InputSystem::HandleKeyPressed(EventArgs& args)
{
	unsigned char keycode = static_cast<unsigned char>(args.GetValue("KeyCode", KEYCODE_INVALID));
	if (keycode && g_input)
	{
		g_input->HandleKeyPressed(keycode);
		return true;
	}

	return false;
}

/*! \brief Event callback function for handling key released events
* 
* Retrives the keycode from the #EventArgs passed and calls the InputSystem#HandleKeyReleased method with the keycode.
* \param args An #EventArgs object containing the arguments required for this function. Must contain a value for KeyCode
* \return A bool indicating whether the event was consumed
* 
*/
bool InputSystem::HandleKeyReleased(EventArgs& args)
{
	unsigned char keycode = static_cast<unsigned char>(args.GetValue("KeyCode", KEYCODE_INVALID));
	if (keycode && g_input)
	{
		g_input->HandleKeyReleased(keycode);
		return true;
	}

	return false;
}

bool InputSystem::HandleMouseWheelScroll(EventArgs& args)
{
	int scrollValue = args.GetValue("ScrollValue", 0);
	g_input->HandleMouseWheelScroll(scrollValue);
	return true;
}

/*! Constructs the InputSystem from an InputSytemConfig
* 
* Sets the configuration for this InputSytem to the provided configuration.
* 
*/
InputSystem::InputSystem(InputConfig config) : m_config(config)
{
}

/*! \brief Startup method for the InputSystem
* 
* Subscribes to the Windows key events with the ::HandleKeyPressed and ::HandleKeyReleased callback functions. Initializes the XboxController array.
* 
*/
void InputSystem::Startup()
{
	SubscribeEventCallbackFunction("WM_KEYDOWN", HandleKeyPressed);
	SubscribeEventCallbackFunction("WM_KEYUP", HandleKeyReleased);
	SubscribeEventCallbackFunction("WM_MOUSEWHEEL", HandleMouseWheelScroll);

	for (int xboxControllerIndex = 0; xboxControllerIndex < NUM_XBOX_CONTROLLERS; xboxControllerIndex++)
	{
		m_xboxControllers[xboxControllerIndex].m_id = xboxControllerIndex;
	}
}

/*! \brief Shutdown method for the InputSystem
* 
* Currently does nothing but should still be called by game code when shutting down.
* 
*/
void InputSystem::Shutdown()
{
}

/*! \brief BeginFrame method for the InputSystem
* 
* Updates XboxController objects by calling the UpdateXboxControllers method, updates CursorState variables
* 
*/
void InputSystem::BeginFrame()
{
	UpdateXboxControllers();

	CURSORINFO windowsCursorInfo;
	windowsCursorInfo.cbSize = sizeof(windowsCursorInfo);
	if (!GetCursorInfo(&windowsCursorInfo))
	{
		ERROR_RECOVERABLE("Could not get cursor info from windows!");
	}
	bool windowsCursorMode = windowsCursorInfo.flags & CURSOR_SHOWING;
	if (m_cursorState.m_hiddenMode == windowsCursorMode)
	{
		while (int windowsCursorState = ShowCursor(!m_cursorState.m_hiddenMode))
		{
			if (m_cursorState.m_hiddenMode && windowsCursorState < 0)
			{
				break;
			}
			else if (!m_cursorState.m_hiddenMode && windowsCursorState >= 0)
			{
				break;
			}
		}
	}

	HWND hwnd = GetActiveWindow();
	IntVec2 previousCursorPos = m_cursorState.m_cursorClientPosition;
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	ScreenToClient(hwnd, &cursorPos);
	m_cursorState.m_cursorClientPosition = IntVec2(cursorPos.x, cursorPos.y);
	if (m_cursorState.m_relativeMode)
	{
		m_cursorState.m_cursorClientDelta = previousCursorPos - m_cursorState.m_cursorClientPosition;
		
		POINT centerPos;
		RECT clientRect;
		GetClientRect(hwnd, &clientRect);
		centerPos.x = (long)((clientRect.right - clientRect.left) * 0.5f);
		centerPos.y = (long)((clientRect.bottom - clientRect.top) * 0.5f);
		ClientToScreen(hwnd, &centerPos);
		SetCursorPos(centerPos.x, centerPos.y);
		GetCursorPos(&cursorPos);
		ScreenToClient(hwnd, &cursorPos);
		m_cursorState.m_cursorClientPosition = IntVec2(cursorPos.x, cursorPos.y);
	}
	else
	{
		m_cursorState.m_cursorClientDelta = IntVec2::ZERO;
	}
}

/*! \brief Updates XboxController objects
* 
*/
void InputSystem::UpdateXboxControllers()
{
	for (int controllerIndex = 0; controllerIndex < NUM_XBOX_CONTROLLERS; controllerIndex++)
	{
		m_xboxControllers[controllerIndex].Update();
	}
}

/*! \brief EndFrame function for the InputSystem
* 
* Updates keystates by copying their isPressed value for this frame to the wasPressedLastFrame value.
* 
*/
void InputSystem::EndFrame()
{
	for (int keyIndex = 0; keyIndex < NUM_KEYS; keyIndex++)
	{
		m_keyStates[keyIndex].m_wasPressedLastFrame = m_keyStates[keyIndex].m_isPressed;
	}

	m_cursorState.m_wheelScrollDelta = 0;
}

/*! Gets whether a specific key was pressed this frame
* 
* This method returns true only for the one frame when the key was just pressed.
* \param keyCode A char representing the keyCode for which the method should check if the key was just pressed this frame
* \return A bool indicating whether the key was just pressed this frame
* 
*/
bool InputSystem::WasKeyJustPressed(unsigned char keyCode) const
{
	return m_keyStates[keyCode].m_isPressed && !m_keyStates[keyCode].m_wasPressedLastFrame;
}

/*! Gets whether a specific key was released this frame
* 
* This method returns true only for the one frame when the key was just released.
* \param keyCode A char representing the keyCode for which the method should check if the key was just released
* \return A bool indicating whether the key was just released this frame
* 
*/
bool InputSystem::WasKeyJustReleased(unsigned char keyCode) const
{
	return !m_keyStates[keyCode].m_isPressed && m_keyStates[keyCode].m_wasPressedLastFrame;
}

/*! Gets whether a specific key is held down
* 
* This method returns true for all frames that the key is held down.
* \param keyCode A char representing the keyCode for which the method should check if the key is held down
* \return A bool indicating whether the key is being held down
* 
*/
bool InputSystem::IsKeyDown(unsigned char keyCode) const
{
	return m_keyStates[keyCode].m_isPressed;
}

/*! Gets whether the Shift key is held down
* 
* This method returns true for all frames that the Shift key is held down.
* \return A bool indicating whether the Shift key is currently held down
* 
*/
bool InputSystem::IsShiftHeld() const
{
	short shiftState = GetKeyState(VK_SHIFT);
	return shiftState & 0x1000;
}

/*! Method to inform the InputSystem that a key was just pressed
* 
* Sets the KeyButtonState::m_isPressed for the provided key. Typically, this method will be called when a WM_KEYDOWN event (or equivalent) is received from the OS.
* \param keyCode An integer representing the key that was just pressed
* 
*/
void InputSystem::HandleKeyPressed(int keyCode)
{
	m_keyStates[keyCode].m_isPressed = true;
}

/*! Method to inform the InputSystem that a key was just released
* 
* Resets the KeyButtonState::m_isPressed for the provided key. Typically, this method will be called when a WM_KEYUP event (or equivalent) is received from the OS.
* \param keyCode An integer representing the key that was just released
* 
*/
void InputSystem::HandleKeyReleased(int keyCode)
{
	m_keyStates[keyCode].m_isPressed = false;
}

void InputSystem::HandleMouseWheelScroll(int scrollValue)
{
	if (scrollValue > 0)
	{
		while (scrollValue >= 0)
		{
			m_cursorState.m_wheelScrollDelta++;
			scrollValue -= 120;
		}
	}
	else
	{
		while (scrollValue <= 0)
		{
			m_cursorState.m_wheelScrollDelta--;
			scrollValue += 120;
		}
	}
}

/*! Gets the controller corresponding to the provided controllerID
* 
* \param controllerID The controller ID for which the XboxController should be returned
* \return An XboxController corresponding to the provided controllerID
* 
*/
XboxController const& InputSystem::GetController(int controllerID) const
{
	return m_xboxControllers[controllerID];
}

/*! \brief Sets the cursor modes
* 
* \param hiddenMode A bool indicating whether the cursor should be set to hidden
* \param relativeMode A bool indicating whether the cursor should be set to relative (as opposed to absolute)
* 
*/
void InputSystem::SetCursorMode(bool hiddenMode, bool relativeMode)
{
	// Resetting the cursor position when entering or exiting relative mode
	// This prevents a camera jump when switching between relative and absolute modes
	if (relativeMode != m_cursorState.m_relativeMode)
	{
		m_cursorState.m_cursorClientDelta = IntVec2::ZERO;
		HWND hwnd = GetActiveWindow();
		POINT centerPos;
		RECT clientRect;
		GetClientRect(hwnd, &clientRect);
		centerPos.x = (long)((clientRect.right - clientRect.left) * 0.5f);
		centerPos.y = (long)((clientRect.bottom - clientRect.top) * 0.5f);
		ClientToScreen(hwnd, &centerPos);
		SetCursorPos(centerPos.x, centerPos.y);
		POINT cursorPos;
		GetCursorPos(&cursorPos);
		ScreenToClient(hwnd, &cursorPos);
		m_cursorState.m_cursorClientPosition = IntVec2(cursorPos.x, cursorPos.y);
	}

	m_cursorState.m_hiddenMode = hiddenMode;
	m_cursorState.m_relativeMode = relativeMode;
}

/*! \brief Gets the cursor delta from the center of the client window in client coordinates
* 
* The delta value of the cursor is returned as an IntVec2 representing the number of pixels in X and Y that the cursor is away from the center.
* \return An IntVec2 representing the number of pixels that the cursor is away from the center of the client window
* 
*/
IntVec2 InputSystem::GetCursorClientDelta() const
{
	if (!m_cursorState.m_relativeMode)
	{
		return IntVec2::ZERO;
	}

	return m_cursorState.m_cursorClientDelta;
}

/*! \brief Gets the normalized cursor coordinates in client coordinates
* 
* \return A Vec2 representing the position of the cursor in normalized [0.f, 1.f] client coordinates
* 
*/
Vec2 InputSystem::GetCursorNormalizedPosition() const
{
	HWND windowHandle = GetActiveWindow();
	POINT cursorCoords;
	RECT clientRect;

	GetCursorPos(&cursorCoords);
	ScreenToClient(windowHandle, &cursorCoords);
	GetClientRect(windowHandle, &clientRect);
	float cursorX = static_cast<float>(cursorCoords.x) / static_cast<float>(clientRect.right);
	float cursorY = static_cast<float>(cursorCoords.y) / static_cast<float>(clientRect.bottom);

	return Vec2(cursorX, 1.f - cursorY);
}
