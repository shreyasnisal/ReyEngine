#pragma once

#include "Engine/Core/EventSystem.hpp"
#include "Engine/Input/KeyButtonState.hpp"
#include "Engine/Input/XboxController.hpp"

//! \file InputSystem.hpp

extern unsigned char const KEYCODE_INVALID;
extern unsigned char const KEYCODE_F1;
extern unsigned char const KEYCODE_F2;
extern unsigned char const KEYCODE_F3;
extern unsigned char const KEYCODE_F4;
extern unsigned char const KEYCODE_F5;
extern unsigned char const KEYCODE_F6;
extern unsigned char const KEYCODE_F7;
extern unsigned char const KEYCODE_F8;
extern unsigned char const KEYCODE_F9;
extern unsigned char const KEYCODE_F10;
extern unsigned char const KEYCODE_F11;
extern unsigned char const KEYCODE_ESC;
extern unsigned char const KEYCODE_UPARROW;
extern unsigned char const KEYCODE_DOWNARROW;
extern unsigned char const KEYCODE_LEFTARROW;
extern unsigned char const KEYCODE_RIGHTARROW;
extern unsigned char const KEYCODE_SPACE;
extern unsigned char const KEYCODE_ENTER;
extern unsigned char const KEYCODE_LMB;
extern unsigned char const KEYCODE_RMB;
extern unsigned char const KEYCODE_TILDE;
extern unsigned char const KEYCODE_BACKSPACE;
extern unsigned char const KEYCODE_HOME;
extern unsigned char const KEYCODE_END;
extern unsigned char const KEYCODE_DELETE;
extern unsigned char const KEYCODE_PERIOD;
extern unsigned char const KEYCODE_COMMA;
extern unsigned char const KEYCODE_BRACKETOPEN;
extern unsigned char const KEYCODE_BRACKETCLOSE;
extern unsigned char const KEYCODE_LEFT_ALT;
extern unsigned char const KEYCODE_CTRL;

//! The number of supported controllers
constexpr int NUM_XBOX_CONTROLLERS = 4;
constexpr int NUM_KEYS = 256;

/*! \brief Stores the state of the mouse cursor
*/
struct CursorState
{
public:
	//! The delta of the cursor from the center of the client window. (0, 0) if the cursor is in absolute mode
	IntVec2			m_cursorClientDelta;
	//! The cursor's position represented in client coordinates
	IntVec2			m_cursorClientPosition;

	//! Whether the cursor is in hidden mode or not
	bool			m_hiddenMode = false;
	//! Whether the cursor is in relative mode or not (as opposed to absolute mode)
	bool			m_relativeMode = false;

	int				m_wheelScrollDelta = 0;
};

/*! \brief A structure for the configuration to be used for this InputSytem
*
* Currently contains nothing but still must be passed in to the InputSystem constructor
*
*/
struct InputConfig
{
public:
};

/*! \brief Stores the states of keys and other input peripherals, and can be queried for the states and values for input peripherals
* 
*/
class InputSystem
{	
public:
	//! Default destructor for the InputSystem
	~InputSystem() = default;
	InputSystem(InputConfig config);

	void					Startup();
	void					Shutdown();
	void					BeginFrame();
	void					EndFrame();

	bool					WasKeyJustPressed(unsigned char keyCode) const;
	bool					WasKeyJustReleased(unsigned char keyCode) const;
	bool					IsKeyDown(unsigned char keyCode) const;
	bool					IsShiftHeld() const;

	void					HandleKeyPressed(int keyCode);
	void					HandleKeyReleased(int keyCode);
	void					HandleMouseWheelScroll(int scrollValue);

	static bool				HandleKeyPressed(EventArgs& args);
	static bool				HandleKeyReleased(EventArgs& args);
	static bool				HandleMouseWheelScroll(EventArgs& args);

	XboxController const&	GetController(int controllerID) const;
	void					UpdateXboxControllers();

	void					SetCursorMode(bool hiddenMode, bool relativeMode);
	IntVec2					GetCursorClientDelta() const;
	Vec2					GetCursorNormalizedPosition() const;

public:
	//! The current state of the mouse cursor
	CursorState				m_cursorState;

protected:
	//! An array of XboxController objects
	XboxController			m_xboxControllers[NUM_XBOX_CONTROLLERS];
	//! An array of KeyButtonStates, representing the states for different keys on the keyboard and mouse
	KeyButtonState			m_keyStates[NUM_KEYS];
	//! The configuration to be used for this InputSystem
	InputConfig				m_config;
};
