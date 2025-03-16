#include "Engine/Renderer/Window.hpp"

#include "Engine/Core/EventSystem.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtils.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commdlg.h>


Window* Window::s_mainWindow = nullptr;


LRESULT CALLBACK WindowsMessageHandlingProcedure(HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam)
{

	Window* window = Window::GetMainWindowInstance();
	GUARANTEE_OR_DIE(window != nullptr, "Window was null!");

	InputSystem* input = window->GetConfig().m_inputSystem;
	GUARANTEE_OR_DIE(input != nullptr, "Window's InputSystem pointer was null!");

	switch (wmMessageCode)
	{
		case WM_CLOSE:
		{
			FireEvent("Quit");
			return 0;
		}

		case WM_CHAR:
		{
			EventArgs charEventArgs;
			charEventArgs.SetValue("KeyCode", Stringf("%d", (unsigned char)wParam));
			FireEvent("WM_CHAR", charEventArgs);
			break;
		}

		case WM_KEYDOWN:
		{
			EventArgs keyDownEventArgs;
			keyDownEventArgs.SetValue("KeyCode", Stringf("%d", (unsigned char)wParam));
			FireEvent("WM_KEYDOWN", keyDownEventArgs);
			break;
		}

		case WM_KEYUP:
		{
			EventArgs keyUpEventArgs;
			keyUpEventArgs.SetValue("KeyCode", Stringf("%d", (unsigned char)wParam));
			FireEvent("WM_KEYUP", keyUpEventArgs);
			break;
		}

		case WM_LBUTTONDOWN:
		{
			unsigned char asKey = KEYCODE_LMB;
			input->HandleKeyPressed(asKey);
			break;
		}

		case WM_LBUTTONUP:
		{
			unsigned char asKey = KEYCODE_LMB;
			input->HandleKeyReleased(asKey);
			break;
		}

		case WM_RBUTTONDOWN:
		{
			unsigned char asKey = KEYCODE_RMB;
			input->HandleKeyPressed(asKey);
			break;
		}

		case WM_RBUTTONUP:
		{
			unsigned char asKey = KEYCODE_RMB;
			input->HandleKeyReleased(asKey);
			break;
		}

		case WM_MOUSEWHEEL:
		{
			int zDelta = (int)GET_WHEEL_DELTA_WPARAM(wParam);
			EventArgs args;
			args.SetValue("ScrollValue", Stringf("%d", zDelta));
			FireEvent("WM_MOUSEWHEEL", args);
			break;
		}

		case WM_SIZE:
		{
			RECT clientRect;
			GetClientRect(reinterpret_cast<HWND>(Window::GetMainWindowInstance()->GetHwnd()), &clientRect);
			Window::GetMainWindowInstance()->m_config.m_clientAspect = fabsf((float)clientRect.right - (float)clientRect.left) / fabsf((float)clientRect.top - (float)clientRect.bottom);
			break;
		}

		case WM_SYSKEYDOWN:
		{
			if (wParam == VK_MENU)
			{
				EventArgs args;
				if (!(lParam & (1 << 24)))
				{
					input->HandleKeyPressed(KEYCODE_LEFT_ALT);
				}
			}
			break;
		}

		case WM_SYSKEYUP:
		{
			if (wParam == VK_MENU)
			{
				EventArgs args;
				if (!(lParam & (1 << 24)))
				{
					input->HandleKeyReleased(KEYCODE_LEFT_ALT);
				}
			}
			break;
		}
	}

	return DefWindowProc(windowHandle, wmMessageCode, wParam, lParam);
}


Window::Window(WindowConfig const& config)
	: m_config(config)
{
	s_mainWindow = this;
}

void Window::Startup()
{
	CreateOSWindow();
}

void Window::BeginFrame()
{
	RunMessagePump();
}

void Window::CreateOSWindow()
{
	HMODULE applicationInstanceHandle = GetModuleHandle(NULL);

	WNDCLASSEX windowClassDescription;
	memset(&windowClassDescription, 0, sizeof(windowClassDescription));
	windowClassDescription.cbSize = sizeof(windowClassDescription);
	windowClassDescription.style = CS_OWNDC;
	windowClassDescription.lpfnWndProc = static_cast<WNDPROC>(WindowsMessageHandlingProcedure);
	windowClassDescription.hInstance = applicationInstanceHandle;
	windowClassDescription.hIcon = NULL;
	windowClassDescription.hCursor = NULL;
	windowClassDescription.lpszClassName = TEXT("Simple Window Class");
	ATOM registeredClassEx = RegisterClassEx(&windowClassDescription);

	DebuggerPrintf("RegisterClassEx Result: %u\n", registeredClassEx);

	const DWORD windowStyleFlags = WS_CAPTION | WS_BORDER | WS_SYSMENU | WS_OVERLAPPED;
	const DWORD windowStyleExFlags = WS_EX_APPWINDOW;

	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect(desktopWindowHandle, &desktopRect);
	float desktopWidth = (float)(desktopRect.right - desktopRect.left);
	float desktopHeight = (float)(desktopRect.bottom - desktopRect.top);
	float desktopAspect = desktopWidth / desktopHeight;

	constexpr float maxClientFractionOfDesktop = 0.90f;
	float clientWidth = desktopWidth * maxClientFractionOfDesktop;
	float clientHeight = desktopHeight * maxClientFractionOfDesktop;
	if (m_config.m_windowSize != IntVec2(-1, -1))
	{
		clientWidth = (float)m_config.m_windowSize.x;
		clientHeight = (float)m_config.m_windowSize.y;
	}
	else
	{
		if (GetAspect() > desktopAspect)
		{
			clientHeight = clientWidth / m_config.m_clientAspect;
		}
		else
		{
			clientWidth = clientHeight * m_config.m_clientAspect;
		}
	}

	m_clientDimensions = IntVec2(RoundDownToInt(clientWidth), RoundDownToInt(clientHeight));

	float clientMarginX = 0.5f * (desktopWidth - clientWidth);
	float clientMarginY = 0.5f * (desktopHeight - clientHeight);
	if (m_config.m_windowPosition != IntVec2(-1, -1))
	{
		clientMarginX = (float)m_config.m_windowPosition.x;
		clientMarginY = (float)m_config.m_windowPosition.y;
	}

	RECT clientRect;
	clientRect.left = (int)clientMarginX;
	clientRect.right = clientRect.left + (int)clientWidth;
	clientRect.top = (int)clientMarginY;
	clientRect.bottom = clientRect.top + (int)clientHeight;

	RECT windowRect = clientRect;
	AdjustWindowRectEx(&windowRect, windowStyleFlags, FALSE, windowStyleExFlags);

	WCHAR windowTitle[1024];
	MultiByteToWideChar(GetACP(), 0, m_config.m_windowTitle.c_str(), -1, windowTitle, sizeof(windowTitle) / sizeof(windowTitle[0]));
	m_hWnd = CreateWindowEx(
		windowStyleExFlags,
		windowClassDescription.lpszClassName,
		windowTitle,
		windowStyleFlags,
		windowRect.left,
		windowRect.top,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		applicationInstanceHandle,
		NULL);

	DebuggerPrintf("Error Code: %d\n", GetLastError());

	if (m_config.m_isFullScreen)
	{
		DWORD dwStyle = GetWindowLong(reinterpret_cast<HWND>(m_hWnd), GWL_STYLE);
		SetWindowLong(reinterpret_cast<HWND>(m_hWnd), GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
		SendMessage(reinterpret_cast<HWND>(m_hWnd), WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	}

	ShowWindow(reinterpret_cast<HWND>(m_hWnd), SW_SHOW);
	SetForegroundWindow(reinterpret_cast<HWND>(m_hWnd));
	SetFocus(reinterpret_cast<HWND>(m_hWnd));

	m_displayDeviceContext = GetDC(reinterpret_cast<HWND>(m_hWnd));

	HCURSOR cursor = LoadCursor(NULL, IDC_ARROW);
	SetCursor(cursor);
}

void Window::RunMessagePump()
{
	MSG queuedMessage;
	while (true)
	{
		const BOOL wasMessagePresent = PeekMessage(&queuedMessage, NULL, 0, 0, PM_REMOVE);
		if (!wasMessagePresent)
		{
			break;
		}

		TranslateMessage(&queuedMessage);
		DispatchMessage(&queuedMessage);
	}
}

WindowConfig const& Window::GetConfig() const
{
	return m_config;
}

float Window::GetAspect() const
{
	return m_config.m_clientAspect;
}

IntVec2 Window::GetClientDimensions() const
{
	return m_clientDimensions;
}

void* Window::GetHwnd() const
{
	return reinterpret_cast<void*>(m_hWnd);
}

void* Window::GetDisplayDeviceContext() const
{
	return m_displayDeviceContext;
}

bool Window::HasFocus() const
{
	HWND hwndMain = GetActiveWindow();
	HWND windowHwnd = reinterpret_cast<HWND>(GetHwnd());

	return (hwndMain == windowHwnd);
}

Window* Window::GetMainWindowInstance()
{
	return s_mainWindow;
}

std::string Window::OpenFileBrowser()
{
	while (int windowsCursorState = ShowCursor(true))
	{
		if (windowsCursorState >= 0)
		{
			break;
		}
	}
	POINT cursorPos;
	GetCursorPos(&cursorPos);

	char filename[MAX_PATH];
	filename[0] = '\0';

	char currentDirectory[MAX_PATH];
	currentDirectory[0] = '\0';
	GetCurrentDirectoryA(MAX_PATH, currentDirectory);

	OPENFILENAMEA data = { };
	data.lStructSize = sizeof(data);
	data.lpstrFile = filename;
	data.nMaxFile = sizeof(filename);
	data.lpstrFilter = "All\0*.*\0";
	data.nFilterIndex = 1;
	data.lpstrInitialDir = currentDirectory;
	data.hwndOwner = (HWND) m_hWnd;
	data.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	bool result = GetOpenFileNameA(&data);

	SetCursorPos(cursorPos.x, cursorPos.y);
	SetCurrentDirectoryA(currentDirectory);

	if (result)
	{
		return filename;
	}

	return "";
}

void Window::EndFrame()
{
}

void Window::Shutdown()
{
}
