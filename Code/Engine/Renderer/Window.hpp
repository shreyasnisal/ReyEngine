#pragma once

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"

#include <string>

class InputSystem;

struct WindowConfig
{
	InputSystem*	m_inputSystem = nullptr;
	std::string		m_windowTitle = "Untitled App";
	float			m_clientAspect = 2.f;
	bool			m_isFullScreen = false;
	IntVec2			m_windowSize = IntVec2(-1, -1);
	IntVec2			m_windowPosition = IntVec2(-1, -1);
};

class Window
{
public:
	~Window() = default;
	Window(WindowConfig const& config);

	void					Startup();
	void					BeginFrame();
	void					EndFrame();
	void					Shutdown();

	WindowConfig const&		GetConfig() const;
	float					GetAspect() const;
	IntVec2					GetClientDimensions() const;
	void*					GetHwnd() const;
	void*					GetDisplayDeviceContext() const;

	bool					HasFocus() const;

	static Window*			GetMainWindowInstance();

	std::string				OpenFileBrowser();

	void					CreateOSWindow();
	void					RunMessagePump();

	WindowConfig	m_config;
	static Window*	s_mainWindow;
	void*			m_hWnd = nullptr;
	IntVec2			m_clientDimensions;
	void*			m_displayDeviceContext = nullptr;
};
