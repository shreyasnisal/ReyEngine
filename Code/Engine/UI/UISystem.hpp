#pragma once

#include "Engine/Core/EventSystem.hpp"
#include "Engine/Renderer/Camera.hpp"


class AudioSystem;
class BitmapFont;
class InputSystem;
class Renderer;
class UIWidget;


struct UISystemConfig
{
public:
	InputSystem* m_input = nullptr;
	Renderer* m_renderer = nullptr;
	Camera m_camera;
	bool m_supportKeyboard = true;
	std::string m_fontFileNameWithNoExtension = "";
	AABB2 m_screenBoundsForVRScreen = AABB2::ZERO_TO_ONE;
};

class UISystem
{
public:
	~UISystem() = default;
	UISystem() = default;
	explicit UISystem(UISystemConfig config);

	void Startup();
	void BeginFrame();
	void EndFrame();
	void Shutdown();

	void Render() const;

	UIWidget* CreateWidget(UIWidget* parent = nullptr);

	UIWidget* GetLastHoveredWidget() const { return m_lastHoveredWidget; };
	void SetLastHoveredWidget(UIWidget* lastHoveredWidget);

	UIWidget* GetPreviousWidget() const;
	UIWidget* GetNextWidget() const;

	void SetSelectedInputField(UIWidget* selectedTextInputFieldWidget);

	void SetFocus(bool focus);

	UIWidget* GetWidgetAtNormalizedCoords(Vec2 const& normalizedCoords) const;

	void Clear();

public:
	static bool Event_HandleCharacterPressed(EventArgs& args);
	static bool Event_HandleKeypressed(EventArgs& args);

public:
	UISystemConfig m_config;
	BitmapFont* m_font = nullptr;
	UIWidget* m_rootWidget = nullptr;
	UIWidget* m_lastHoveredWidget = nullptr;
	UIWidget* m_selectedInputFieldWidget = nullptr;
};
