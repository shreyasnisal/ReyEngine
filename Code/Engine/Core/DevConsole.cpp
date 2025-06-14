#include "Engine/Core/DevConsole.hpp"

#include "Engine/Core/Clock.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Renderer.hpp"

#if defined(ERROR)
	#undef ERROR
#endif

const Rgba8 DevConsole::WARNING = Rgba8::YELLOW;
const Rgba8 DevConsole::ERROR = Rgba8::RED;
const Rgba8 DevConsole::INFO_MAJOR = Rgba8::GREEN;
const Rgba8 DevConsole::INFO_MINOR = Rgba8::WHITE;

/*! Event callback for when a character on the keyboard was pressed
* 
* This callback function does nothing when the console is hidden, and adds the character in the correct position in the input text when it is open.
* The function also consumes the event so that no other callback functions are called by the EventSystem after this, so that the game does not use these keys for input when the console is open.
* \param args An #EventArgs (NamedStrings) structure containing the arguments required for this function. Must include the keyCode as an integer for correct results
* \return A boolean indicating whether the event was consumed
* 
*/
bool DevConsole::HandleCharacterPressed(EventArgs& args)
{
	if (!g_console || g_console->m_mode == DevConsoleMode::HIDDEN)
	{
		return false;
	}

	unsigned char keyCode = static_cast<unsigned char>(args.GetValue("KeyCode", KEYCODE_INVALID));
	
	if (keyCode == KEYCODE_PERIOD)
	{
		g_console->HandleCharacterPressed('.');
		return true;
	}

	if (keyCode >= 32 && keyCode <= 126 && keyCode != '~' && keyCode != '`')
	{
		g_console->HandleCharacterPressed(keyCode);
		return true;
	}

	if (keyCode == KEYCODE_BACKSPACE ||
		keyCode == KEYCODE_ENTER ||
		keyCode == KEYCODE_LEFTARROW ||
		keyCode == KEYCODE_RIGHTARROW ||
		keyCode == KEYCODE_UPARROW ||
		keyCode == KEYCODE_DOWNARROW ||
		keyCode == KEYCODE_HOME ||
		keyCode == KEYCODE_END ||
		keyCode == KEYCODE_DELETE ||
		keyCode == KEYCODE_ESC)
	{
		return true;
	}

	return false;
}

/*! Event callback for when a key on an input peripheral was pressed
*
* This callback function does nothing when the console is hidden, and handles special key input for the keyboard if the key is mapped to a console action.
* The function also consumes the event so that no other callback functions are called by the EventSystem after this, so that the game does not use these keys for input when the console is open.
* \param args An #EventArgs (NamedStrings) structure containing the arguments required for this function. Must include the keyCode as an integer for correct results
* \return A boolean indicating whether the event was consumed
*
*/
bool DevConsole::HandleKeyPressed(EventArgs& args)
{
	if (!g_console || g_console->m_mode == DevConsoleMode::HIDDEN)
	{
		return false;
	}

	unsigned char keyCode = static_cast<unsigned char>(args.GetValue("KeyCode", KEYCODE_INVALID));

	if (keyCode == KEYCODE_BACKSPACE ||
		keyCode == KEYCODE_ENTER ||
		keyCode == KEYCODE_LEFTARROW ||
		keyCode == KEYCODE_RIGHTARROW ||
		keyCode == KEYCODE_UPARROW ||
		keyCode == KEYCODE_DOWNARROW ||
		keyCode == KEYCODE_HOME ||
		keyCode == KEYCODE_END ||
		keyCode == KEYCODE_DELETE ||
		keyCode == KEYCODE_ESC)
	{
		g_console->HandleSpecialCharacterPressed(keyCode);
		return true;
	}

	if (keyCode >= 32 && keyCode <= 126 && keyCode != '~' && keyCode != '`')
	{
		return true;
	}

	return false;
}

bool DevConsole::HandleMouseWheelScroll(EventArgs& args)
{
	if (!g_console || g_console->m_mode == DevConsoleMode::HIDDEN)
	{
		return false;
	}

	int scrollValue = args.GetValue("ScrollValue", 0);
	g_console->HandleMouseWheelScroll(scrollValue);
	return true;
}

/*! Event callback for the help command
*
* The help command displays a list of all registered commands with the exception of WM_KEYDOWN, WM_KEYUP and WM_CHAR. This console function calls a member function in EventSystem to iterate through the list of registered commands the add correspoding lines to the conosle.
* \param args An #EventArgs (NamedStrings) structure containing the arguments required for this function. May optionally include a boolean value for the "help" flag, in which case the function prints more info on the help command to the console
* \return A boolean indicating whether the event was consumed
* \sa EventSystem#ListAllCommands
* 
*/
bool DevConsole::Command_Help(EventArgs& args)
{
	if (!g_console || g_console->m_mode == DevConsoleMode::HIDDEN)
	{
		return false;
	}

	bool help = args.GetValue("help", false);
	if (help)
	{
		g_console->AddLine(DevConsole::WARNING, "You need help on the help command? You should get help!", false);
		return true;
	}

	g_eventSystem->ListAllCommands();
	return true;
}

/*! Event callback for the clear command
*
* The clear command removes all lines from the list of lines stored by the console. As a result, the console no longer shows any previous lines that were added.
* \param args An #EventArgs (NamedStrings) structure containing the arguments required for this function. May optionally include a boolean value for the "help" flag, in which case the function prints more info on the clear command to the console
* \return A boolean indicating whether the event was consumed
*
*/
bool DevConsole::Command_Clear(EventArgs& args)
{
	UNUSED(args);

	bool help = args.GetValue("help", false);
	if (help)
	{
		g_console->AddLine("The clear command clears the console. No parameters, nothing. What more information do you need?", false);
		return true;
	}

	g_console->m_lines.clear();
	return true;
}

/*! Event callback for the echo command
*
* The echo command prints any text following the command name to the console. An exception to this is made is the text has the string "help" with no leading or trailing whitespaces.
* \param args An #EventArgs (NamedStrings) structure containing the arguments required for this function. Must include a value for "EchoArg" for correct results, and may optionally contain a boolean value for the "help" flag, in which case the function prints more info on the echo command to the console.
* \return A boolean indicating whether the event was consumed
*
*/
bool DevConsole::Command_Echo(EventArgs& args)
{
	bool help = args.GetValue("help", false);
	if (help)
	{
		g_console->AddLine("Displays messages on the console", false);
		g_console->AddLine("Example Usage: > echo Hello, World!", false);
		return true;
	}
	std::string stringToEcho = args.GetValue("EchoArg", "");
	g_console->AddLine(stringToEcho, g_console->m_showLineMetaDataOnEcho);
	return true;
}

/*! Event callback for the exit command
* 
* The exit command closes the console (not the app).
* \param args An #EventArgs (NamedStrings) structure containing the arguments required for the exit command. May optionally include a boolean value for the "help" flag, in which case the function prints more info about the exit command to the console.
* \return A boolean indicating whether the event was consumed
*/
bool DevConsole::Command_Exit(EventArgs& args)
{
	bool help = args.GetValue("help", false);
	if (help)
	{
		g_console->AddLine("Exits the console (not the app). No parameters, just a convenience thing", false);
		return true;
	}

	g_console->SetMode(DevConsoleMode::HIDDEN);
	return true;
}

bool DevConsole::Command_EchoSpecial(EventArgs& args)
{
	std::string specialCommand = args.GetValue("specialCommand", "");

	if (!strcmp(specialCommand.c_str(), "off"))
	{
		g_console->m_commandEcho = false;
		return true;
	}
	else if (!strcmp(specialCommand.c_str(), "on"))
	{
		g_console->m_commandEcho = true;
		return true;
	}

	return false;
}

/*! \brief Constructor for the DevConsole
* 
* \param config The configuration to be used for the console being created.
* 
*/
DevConsole::DevConsole(DevConsoleConfig const& config)
	: m_config(config)
{
}

/*! \brief Startup function that performs most of the initialization
* 
* Subscribes to all required events with the appropriate callback functions
* 
*/
void DevConsole::Startup()
{
	SubscribeEventCallbackFunction("WM_CHAR", HandleCharacterPressed);
	SubscribeEventCallbackFunction("WM_KEYDOWN", HandleKeyPressed);
	SubscribeEventCallbackFunction("WM_MOUSEWHEEL", HandleMouseWheelScroll);

	SubscribeEventCallbackFunction("Help", Command_Help, "Displays a list of all available commands");
	SubscribeEventCallbackFunction("Echo", Command_Echo, "Displays messages on the console");
	SubscribeEventCallbackFunction("Clear", Command_Clear, "Clears the console");
	SubscribeEventCallbackFunction("Exit", Command_Exit, "Exits the console");
	SubscribeEventCallbackFunction("@Echo", Command_EchoSpecial, "Special command to set console command echo");

	//AddLine(DevConsole::WARNING, "While command names are case insensitive, arguments are not and must always be typed in all lowercase", false);
	AddLine("", false);
	AddLine("", false);

	m_blinkingCaretTimer = new Stopwatch(0.5f);
	m_linesToShow = m_config.m_linesToShow;
}

/*! \brief Method that should be called by game code at the beginning of every frame
* 
* Used for keeping count of the current frame number and for updating the visibility of the blinking insertion pointer.
* 
*/
void DevConsole::BeginFrame()
{
	m_frameNumber++;

	while (m_blinkingCaretTimer->DecrementDurationIfElapsed())
	{
		m_isCaretVisible = !m_isCaretVisible;
	}
}

/*! \brief Method to be called for rendering the console
* 
* In general, this method should be called by game code during every render. The method handles rendering only when the console is not in hidden mode (game code should not handle this).
* \param bounds An AABB2 indicating the bounds inside the console camera for where the console should be rendered
* \param rendererOverride (Optional) A Renderer to be used for rendering the console. If none is provided, the method uses the renderer provided in its config
* 
*/
void DevConsole::Render(AABB2 const& bounds, Renderer* rendererOverride) const
{
	Renderer* renderer = m_config.m_renderer;
	if (rendererOverride)
	{
		renderer = rendererOverride;
	}

	BitmapFont* font = renderer->CreateOrGetBitmapFont(m_config.m_consoleFontFilePathWithNoExtension.c_str());

	switch (m_mode)
	{
		case DevConsoleMode::HIDDEN:																				return;
		case DevConsoleMode::OPENFULL:			Render_OpenFull(bounds, *renderer, *font, m_config.m_fontAspect);	return;
	}
}

/*! \brief Renders the console over the entire bounds
* 
* This method handles rendering a transparent background, lines that have been added to the console and the typed input text.
* Any lines that go beyond the bounds of the camera are excluded for drawing for improved performance.
* \param bounds An AABB2 indicating the bounds over which the console should be drawn
* \param renderer The Renderer to be used for drawing
* \param font The BitmapFont to be used for any text being rendered
* \param fontAspect (Optional) A float indicating the aspect ratio to be used for text glyphs
* 
*/
void DevConsole::Render_OpenFull(AABB2 const& bounds, Renderer& renderer, BitmapFont& font, float fontAspect) const
{
	renderer.BeginCamera(m_config.m_camera);
	renderer.BeginRenderEvent("DevConsole");

	std::vector<Vertex_PCU> devConsoleVerts;
	std::vector<Vertex_PCU> devConsoleTextVerts;
	devConsoleTextVerts.reserve(1000);

	AddVertsForAABB2(devConsoleVerts, bounds, m_config.m_overlayColor);

	float lineHeight = (m_config.m_camera.GetOrthoTopRight().y - m_config.m_camera.GetOrthoBottomLeft().y) / m_linesToShow;
	
	m_linesMutex.lock();
	
	int numLines = static_cast<int>(m_lines.size());

	AddVertsForLineSegment2D(devConsoleVerts, Vec2(bounds.m_mins.x, bounds.m_mins.y + lineHeight * 2.f), Vec2(bounds.m_maxs.x, bounds.m_mins.y + lineHeight * 2.f), lineHeight * 0.1f, Rgba8::DODGER_BLUE);
	font.AddVertsForTextInBox2D(devConsoleTextVerts, AABB2(bounds.m_mins + Vec2((m_config.m_camera.GetOrthoTopRight().x - m_config.m_camera.GetOrthoBottomLeft().x) * 0.01f, lineHeight * 0.3f), Vec2(bounds.m_maxs.x, bounds.m_mins.y + lineHeight * 1.3f)), lineHeight, m_command, Rgba8::DODGER_BLUE, fontAspect);
	
	if (m_isCaretVisible)
	{
		std::string caretPositionText = "";
		for (int commandCharacterIndex = 0; commandCharacterIndex <= static_cast<int>(m_command.size()); commandCharacterIndex++)
		{
			if (commandCharacterIndex == m_caretPosition)
			{
				caretPositionText += '_';
				continue;
			}
			caretPositionText += ' ';
		}
		font.AddVertsForTextInBox2D(devConsoleTextVerts, AABB2(bounds.m_mins + Vec2((m_config.m_camera.GetOrthoTopRight().x - m_config.m_camera.GetOrthoBottomLeft().x) * 0.01f, lineHeight * 0.3f), Vec2(bounds.m_maxs.x, bounds.m_mins.y + lineHeight * 1.3f)), lineHeight, caretPositionText, Rgba8::RED, fontAspect);
	}

	for (int lineIndex = numLines - 1; lineIndex >= 0; lineIndex--)
	{
		AABB2 lineBounds(Vec2(bounds.m_mins.x + (m_config.m_camera.GetOrthoTopRight().x - m_config.m_camera.GetOrthoBottomLeft().x) * 0.01f, m_verticalPosition * lineHeight + bounds.m_mins.y + lineHeight * 1.1f * static_cast<float>(numLines - lineIndex + 1)), Vec2(bounds.m_maxs.x, m_verticalPosition * lineHeight + bounds.m_mins.y + 2.f + lineHeight * 0.9f * static_cast<float>(numLines - lineIndex + 2)));
		if (lineBounds.m_mins.y > bounds.m_maxs.y)
		{
			break;
		}
		if (lineBounds.m_mins.y < bounds.m_mins.y + lineHeight * 2.f)
		{
			continue;
		}
		std::string lineText = "";
		if (m_lines[lineIndex].m_showTimeStampAndFrameNumber)
		{
			lineText = Stringf("%.2f (Frame #%d) %s", m_lines[lineIndex].m_timestamp, m_lines[lineIndex].m_frameNumber, m_lines[lineIndex].m_text.c_str());
		}
		else
		{
			lineText = Stringf("%s", m_lines[lineIndex].m_text.c_str());
		}
		font.AddVertsForTextInBox2D(devConsoleTextVerts, AABB2(lineBounds.m_mins + Vec2(1.f, -1.f) * lineHeight * 0.25f, lineBounds.m_maxs + Vec2(1.f, -1.f) * lineHeight * 0.25f), lineHeight, lineText, Rgba8::BLACK, fontAspect);
		font.AddVertsForTextInBox2D(devConsoleTextVerts, lineBounds, lineHeight, lineText, m_lines[lineIndex].m_color, fontAspect);
	}

	m_linesMutex.unlock();

	renderer.BindTexture(nullptr);
	renderer.DrawVertexArray(devConsoleVerts);

	renderer.BindTexture(font.GetTexture());
	renderer.DrawVertexArray(devConsoleTextVerts);

	renderer.EndRenderEvent("DevConsole");
	renderer.EndCamera(m_config.m_camera);
}

/*! \brief Currently does nothing but should be called at the end of each frame
*/
void DevConsole::EndFrame()
{
}

/*! \brief Current does nothing but should be called when the App is shutting down
*/
void DevConsole::Shutdown()
{
}

/*! \brief Adds character to the input text at the cursor position
* 
* This also restarts the cursor blink timer.
* \param keyCode The ASCII value for the character to be added to the input text
* 
*/
void DevConsole::HandleCharacterPressed(unsigned char keyCode)
{
	m_command.insert(m_command.begin() + m_caretPosition, 1, keyCode);
	m_caretPosition++;
	m_blinkingCaretTimer->Restart();
}

/*! \brief Handles special character input for the console
* 
* Characters handled:
* - Backspace: Removes the character to the left of the insertion pointer
* - Delete: Removes the character at the position of the insertion pointer
* - Left Arrow Key: Moves the insertion pointer one position left
* - Right Arrow Key: Moves the insertion pointer one position right
* - Up Arrow Key: Fills the input text with the next newest command in the command history
* - Down Arrow Key: Fills the input text with the next oldest command in the command history
* - Home: Moves the insertion pointer to the beginning of the input line
* - End: Moves the insertion pointer to the end of the input line
* - Enter: Executes the command in the input text. If the input text is empty, closes the console
* - Escape: Clears the input line. If the input line is empty, closes the console
* 
* \param keyCode The ASCII value for the key to be processed
* 
*/
void DevConsole::HandleSpecialCharacterPressed(unsigned char keyCode)
{
	if (keyCode == KEYCODE_BACKSPACE)
	{
		if (m_caretPosition == 0)
		{
			return;
		}
		m_command.erase(m_caretPosition - 1, 1);
		m_caretPosition--;
	}
	else if (keyCode == KEYCODE_DELETE)
	{
		if (m_caretPosition == (int)m_command.size())
		{
			return;
		}
		m_command.erase(m_caretPosition, 1);
	}
	else if (keyCode == KEYCODE_LEFTARROW)
	{
		if (m_caretPosition == 0)
		{
			return;
		}
		m_caretPosition--;
	}
	else if (keyCode == KEYCODE_RIGHTARROW)
	{
		if (m_caretPosition == (int)m_command.size())
		{
			return;
		}
		m_caretPosition++;
	}
	else if (keyCode == KEYCODE_UPARROW)
	{
		if (m_commandHistoryIndex <= 0)
		{
			return;
		}

		m_commandHistoryIndex--;
		m_command = m_commandHistory[m_commandHistoryIndex];
		m_caretPosition = static_cast<int>(m_command.size());
	}
	else if (keyCode == KEYCODE_DOWNARROW)
	{
		if (m_commandHistoryIndex == static_cast<int>(m_commandHistory.size()) - 1)
		{
			return;
		}

		m_commandHistoryIndex++;
		m_command = m_commandHistory[m_commandHistoryIndex];
		m_caretPosition = static_cast<int>(m_command.size());
	}
	else if (keyCode == KEYCODE_ENTER)
	{
		if (m_command == "")
		{
			m_mode = DevConsoleMode::HIDDEN;
			return;
		}

		Execute(m_command);
		m_commandHistory.push_back(m_command);
		m_commandHistoryIndex = static_cast<int>(m_commandHistory.size());
		m_command = "";
		m_caretPosition = 0;
	}
	else if (keyCode == KEYCODE_HOME)
	{
		m_caretPosition = 0;
	}
	else if (keyCode == KEYCODE_END)
	{
		m_caretPosition = static_cast<int>(m_command.size());
	}
	else if (keyCode == KEYCODE_ESC)
	{
		if (m_command == "")
		{
			m_mode = DevConsoleMode::HIDDEN;
			return;
		}

		m_command = "";
		m_caretPosition = 0;
	}

	m_blinkingCaretTimer->Restart();
}

void DevConsole::HandleMouseWheelScroll(int scrollValue)
{
	m_linesMutex.lock();
	int numLines = (int)m_lines.size();
	m_linesMutex.unlock();

	if (scrollValue > 0 && m_verticalPosition >= 0)
	{
		return;
	}
	if (scrollValue < 0 && (int)fabsf((float)m_verticalPosition) + m_linesToShow - SCROLL_BUFFER >= numLines)
	{
		return;
	}

	while (fabsf((float)scrollValue) > 0.f)
	{
		m_verticalPosition += scrollValue > 0 ? 1 : -1;
		scrollValue += scrollValue > 0 ? -120 : 120;
	}
}

void DevConsole::SetCommandEcho(bool commandEcho)
{
	m_commandEcho = commandEcho;
}

/*! \brief Executes the command in the input text
* 
* Checks if the text is the echo command, and adds the text that follows to the EchoArg.
* If not, the input line is split using spaces first for the command followed by key-value pairs, and split using '=' next to separate keys and values.
* This method can also be used to execute multiple commands at the same time using separate lines for each command.
* Commands are executed by firing events with the event name being the command.
* \param consoleCommandText A string containing the string to be parsed
* \sa EventSystem::FireEvent
*/
void DevConsole::Execute(std::string const& consoleCommandText)
{
	//AddLine("", false);
	if (m_commandEcho)
	{
		AddLine("", false);
		AddLine(Rgba8::SILVER, consoleCommandText, false);
	}

	Strings commandLines;
	int numCommandLines = SplitStringOnDelimiter(commandLines, consoleCommandText, '\n');
	for (int commandIndex = 0; commandIndex < numCommandLines; commandIndex++)
	{
		Strings commandNameAndArgs;
		int numArgs = SplitStringOnDelimiter(commandNameAndArgs, commandLines[commandIndex], ' ', '"', false);
		std::string commandName = commandNameAndArgs[0];

		// convert to lower case to check if this command is the echo command
		std::transform(commandName.begin(), commandName.end(), commandName.begin(), [](unsigned char c){ return static_cast<unsigned char>(std::tolower(static_cast<int>(c))); });

		if (!strcmp(commandName.c_str(), "echo"))
		{
			EventArgs eventArgs;
			std::string echoArg;
			if (numArgs == 1)
			{
				return;
			}
			for (int i = 1; i < numArgs; i++)
			{
				if (!strcmp(commandNameAndArgs[i].c_str(), "\\help"))
				{
					eventArgs.SetValue("help", "true");
					FireEvent(commandName, eventArgs);
					return;
				}
				
				echoArg += commandNameAndArgs[i] + " ";
			}
			echoArg.pop_back();
			eventArgs.SetValue("EchoArg", echoArg);
			FireEvent(commandName, eventArgs);
			return;
		}

		if (!strcmp(commandName.c_str(), "@echo"))
		{
			EventArgs eventArgs;
			std::string echoArg;
			if (numArgs == 1)
			{
				return;
			}
			for (int i = 1; i < numArgs; i++)
			{
				if (!strcmp(commandNameAndArgs[i].c_str(), "off"))
				{
					eventArgs.SetValue("specialCommand", "off");
				}
				else if (!strcmp(commandNameAndArgs[i].c_str(), "on"))
				{
					eventArgs.SetValue("specialCommand", "on");
				}
			}

			FireEvent(commandName, eventArgs);
			return;
		}

		if (!strcmp(commandName.c_str(), "@echometa"))
		{
			EventArgs eventArgs;
			std::string echoArg;
			if (numArgs == 1)
			{
				return;
			}
			for (int i = 1; i < numArgs; i++)
			{
				if (!strcmp(commandNameAndArgs[i].c_str(), "off"))
				{
					eventArgs.SetValue("specialCommand", "off");
				}
				else if (!strcmp(commandNameAndArgs[i].c_str(), "on"))
				{
					eventArgs.SetValue("specialCommand", "on");
				}
			}

			FireEvent(commandName, eventArgs);
			return;
		}

		EventArgs eventArgs;
		for (int argIndex = 1; argIndex < numArgs; argIndex++)
		{
			Strings keyValuePair;
			SplitStringOnDelimiter(keyValuePair, commandNameAndArgs[argIndex], '=', '"');
			
			if (keyValuePair.size() == 1)
			{
				//AddLine(DevConsole::ERROR, "Invalid command arguments, please see use the 'help' command for information on available commands and argument format");
				keyValuePair.push_back("true");
			}

			eventArgs.SetValue(keyValuePair[0], keyValuePair[1]);
		}
		FireEvent(commandName, eventArgs);
	}
}

void DevConsole::ExecuteXmlCommandScriptNode(XmlElement const& commandScriptXmlElement)
{
	XmlElement const* currentElement = commandScriptXmlElement.FirstChildElement();
	while (currentElement)
	{
		char const* commandName = currentElement->Name();
		EventArgs args;
		XmlAttribute const* currentAttribute = currentElement->FirstAttribute();
		while (currentAttribute)
		{
			args.SetValue(currentAttribute->Name(), currentAttribute->Value());
			currentAttribute = currentAttribute->Next();
		}
		FireEvent(commandName, args);

		currentElement = currentElement->NextSiblingElement();
	}
}

void DevConsole::ExecuteXmlCommandScriptFile(std::string const& commandScriptXmlFilePathName)
{
	XmlDocument doc;
	XmlResult result = doc.LoadFile(commandScriptXmlFilePathName.c_str());
	if (result != XmlResult::XML_SUCCESS)
	{
		g_console->AddLine(DevConsole::ERROR, Stringf("Could not find or read file %s!", commandScriptXmlFilePathName.c_str()));
		return;
	}

	XmlElement const* rootElement = doc.RootElement();
	ExecuteXmlCommandScriptNode(*rootElement);
}

/*! \brief Gets the mode that the console is currently in
* 
* \return A DevConsoleMode indicating the current console mode
* 
*/
DevConsoleMode DevConsole::GetMode() const
{
	return m_mode;
}

/*! \brief Sets the mode for the console
* 
* \param mode The DevConsoleMode to set the current mode to
* 
*/
void DevConsole::SetMode(DevConsoleMode mode)
{
	m_mode = mode;

	if (m_mode != DevConsoleMode::HIDDEN)
	{
		m_blinkingCaretTimer->Start();
	}
}

/*! \brief Toggles the mode for the console between hidden and the mode provided
* 
* If the mode is currently in the mode provided, sets the mode to hidden. Otherwise, sets the mode of the console to the one provided.
* \param mode The DevConsoleMode to set the mode to if the console is not already in it
* 
*/
void DevConsole::ToggleMode(DevConsoleMode mode)
{
	if (m_mode == mode)
	{
		m_mode = DevConsoleMode::HIDDEN;
	}
	else
	{
		m_mode = mode;
		m_blinkingCaretTimer->Start();
	}
}

/*! \brief Adds a line to the console
* 
* Sets frame number and timestamp for the line being added.
* \param color The Rgba8 color to be used for this line
* \param text The text to be rendered in this line
* \param showTimestampAndFrameNumber A boolean indicating whether the timestamp and frame number should be shown when this line is rendered
* 
*/
void DevConsole::AddLine(Rgba8 const& color, std::string const& text, bool showTimestampAndFrameNumber)
{
	DevConsoleLine line;
	line.m_text = text;
	line.m_color = color;
	line.m_frameNumber = m_frameNumber;
	line.m_timestamp = GetCurrentTimeSeconds();
	line.m_showTimeStampAndFrameNumber = showTimestampAndFrameNumber;
	m_linesMutex.lock();
	m_lines.push_back(line);
	m_linesMutex.unlock();
}

/*! \brief Adds a line to the console
* 
* Uses the default console color (DevConsole::INFO_MINOR) as the color for this line
* \param text The text to be rendered in this line
* \param showTimestampAndFrameNumber A boolean indicating whether the timestamp and frame number should be shown then this line is rendered
* \sa AddLine(Rgba8 const&, std::string const&, bool)
* 
*/
void DevConsole::AddLine(std::string const& text, bool showTimestampAndFrameNumber)
{
	AddLine(DevConsole::INFO_MINOR, text, showTimestampAndFrameNumber);
}

