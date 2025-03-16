#pragma once

/* \brief Stores the state of the keys or buttons (digital input peripheral values)
* 
* The InputSystem can use data members of this class to decide whether a key was just pressed this frame, has been held down for longer than a frame or just released this frame.
* 
*/
class KeyButtonState
{
public:
	//! Whether the key is pressed this frame
	bool	m_isPressed = false;
	//! Whether the key was pressed last frame
	bool	m_wasPressedLastFrame = false;
};
