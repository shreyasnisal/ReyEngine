#pragma once

#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/Camera.hpp"

#include <mutex>
#include <string>
#include <vector>

#if defined(ERROR)
	#undef ERROR
#endif

class BitmapFont;
class Renderer;
class Stopwatch;

/*! \brief The configuration for the DevConsole
* 
* The configuration contains the renderer that should be used by default for rendering the console, and a camera that indicates the bounds. For best results, ensure the camera is set to orthographic mode and has the same aspect ratio as the window used for the game. The config also stores the BitmapFont that should be used for all text.
* 
*/
struct DevConsoleConfig
{
public:
	Renderer*			m_renderer;
	Camera				m_camera;
	std::string			m_consoleFontFilePathWithNoExtension;
	Rgba8				m_overlayColor = Rgba8(0, 0, 0, 200);
	float				m_linesToShow = 50.5f;
	float				m_fontAspect = 0.7f;
};

/*! \brief The modes that the console can be in
* 
* Currently the console can only be in one of two states- hidden (the console does not render at all) or openfull (the console covers the entire bounds passed to it with a transparent background so that the game elements are still visible).
* More modes can be added to support the console being opened in a small area of the screen, etc. based on game requirements.
* 
*/
enum class DevConsoleMode
{
	HIDDEN,
	OPENFULL
};

/*! \brief Metadata for each line printed to the console
* 
* Each line in the console contains metadata for the color it should be rendered in, the frame number and timestamp and frame number for when it was added to the console, and whether it should show this frame number and timestamp.
* This struct also stores the text in the line to be printed.
* 
*/
struct DevConsoleLine
{
public:
	Rgba8				m_color = Rgba8::WHITE;
	std::string			m_text;
	int					m_frameNumber;
	double				m_timestamp;
	bool				m_showTimeStampAndFrameNumber = false;
};

/*! \brief A console for developer debugging
*
* This class offers a developer console that can be used to log messages, warnings and errors during the game. The console can be opened by a key that must be mapped in game code, and allows typing commands and executing those commands through the EventSystem.
* The engine already provides a global DevConsole instance (g_console). However, game code must initialize this instance before it can be used.
*\sa EventSystem
* 
*/
class DevConsole
{
public:
	DevConsole(DevConsoleConfig const& config);
	~DevConsole() = default;

	void								Startup();
	void								Shutdown();
	void								BeginFrame();
	void								EndFrame();

	void								Execute(std::string const& consoleCommandText);
	void								ExecuteXmlCommandScriptNode(XmlElement const& commandScriptXmlElement);
	void								ExecuteXmlCommandScriptFile(std::string const& commandScriptXmlFilePathName);
	void								AddLine(Rgba8 const& color, std::string const& text, bool showTimestampAndFrameNumber = false);
	void								AddLine(std::string const& text, bool showTimestampAndFrameNumber = false);
	void								Render(AABB2 const& bounds, Renderer* rendererOverride = nullptr) const;

	DevConsoleMode						GetMode() const;
	void								SetMode(DevConsoleMode mode);
	void								ToggleMode(DevConsoleMode mode);
	void								HandleCharacterPressed(unsigned char keyCode);
	void								HandleSpecialCharacterPressed(unsigned char keyCode);
	void								HandleMouseWheelScroll(int scrollValue);

	void								SetCommandEcho(bool commandEcho);

	static bool							HandleCharacterPressed(EventArgs& args);
	static bool							HandleKeyPressed(EventArgs& args);
	static bool							HandleMouseWheelScroll(EventArgs& args);
	static bool							Command_Help(EventArgs& args);
	static bool							Command_Clear(EventArgs& args);
	static bool							Command_Echo(EventArgs& args);
	static bool							Command_Exit(EventArgs& args);
	static bool							Command_EchoSpecial(EventArgs& args);

	//! The color to be used for error messages in the console (Rgba8::RED)
	static const Rgba8 ERROR;
	//! The color to be used for warning messages in the console (Rgba8::YELLOW)
	static const Rgba8 WARNING; 
	//! The color to be used for major info messages in the console (Rgba8::GREEN)
	static const Rgba8 INFO_MAJOR;
	//! The color to be used for info messages in the console. If a color for a message is not specified, the text defaults to this color (Rgba8::WHITE)
	static const Rgba8 INFO_MINOR;

protected:
	void								Render_OpenFull(AABB2 const& bounds, Renderer& renderer, BitmapFont& font, float fontAspect = 1.f) const;

protected:
	static constexpr int SCROLL_BUFFER = 10;

	DevConsoleConfig					m_config; //! The config to be used for this console
	std::atomic<DevConsoleMode>			m_mode = DevConsoleMode::HIDDEN; //! The current mode that this console is in
	mutable std::mutex					m_linesMutex;
	std::vector<DevConsoleLine>			m_lines; //! A list of lines that have been added to the console
	std::atomic<int>					m_frameNumber = 0; //! The current frame number of the app
	float								m_linesToShow = 50.5f; //! The maximum number of lines that can be shown in the console at a time. This also decides the height of each line
	std::string							m_command = ""; //! The input text that has been typed into the console
	int									m_caretPosition = 0; //! The current position of the insertion pointer in the input text
	std::vector<std::string>			m_commandHistory; //! A list of commands that have been executed through the console (also includes invalid commands that could not be executed)
	std::atomic<int>					m_commandHistoryIndex = -1; //! The current index in the command history

	bool								m_isCaretVisible = true; //! Whether the insertion pointer is visible or not (used for a blinking insertion pointer)
	Stopwatch*							m_blinkingCaretTimer; //! The Stopwatch used to toggle the insertion pointer's visibility

	bool								m_showLineMetaDataOnEcho = false;
	bool								m_commandEcho = true;

	int m_verticalPosition = 0;
};
