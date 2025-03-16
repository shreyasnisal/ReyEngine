#include "Engine/UI/UISystem.hpp"

#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/DebugRenderSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/UI/UIWidget.hpp"


UISystem::UISystem(UISystemConfig config)
	: m_config(config)
{
}

void UISystem::Startup()
{
	m_font = m_config.m_renderer->CreateOrGetBitmapFont(m_config.m_fontFileNameWithNoExtension.c_str());
	m_rootWidget = new UIWidget(this);
	m_rootWidget->
		SetPosition(m_config.m_camera.GetOrthoBottomLeft())->
		SetDimensions(m_config.m_camera.GetOrthoTopRight() - m_config.m_camera.GetOrthoBottomLeft())->
		SetRaycastTarget(false);
	SubscribeEventCallbackFunction("WM_CHAR", Event_HandleCharacterPressed);
	SubscribeEventCallbackFunction("WM_KEYDOWN", Event_HandleKeypressed);
}

void UISystem::BeginFrame()
{
	if (!m_config.m_supportKeyboard)
	{
		m_lastHoveredWidget = nullptr;
	}
	m_rootWidget->Update();
}

void UISystem::EndFrame()
{
}

void UISystem::Shutdown()
{
}

void UISystem::Render() const
{
	m_config.m_renderer->BeginRenderEvent("UISystem");
	m_config.m_renderer->BeginCamera(m_config.m_camera);
	m_rootWidget->Render();
	m_config.m_renderer->EndCamera(m_config.m_camera);
	m_config.m_renderer->EndRenderEvent("UISystem");
}

UIWidget* UISystem::CreateWidget(UIWidget* parent)
{
	UIWidget* newWidget = new UIWidget(this);
	if (parent)
	{
		parent->AddChild(newWidget);
	}
	else
	{
		if (m_rootWidget->m_children.empty())
		{
			m_lastHoveredWidget = newWidget;
		}
		m_rootWidget->AddChild(newWidget);
	}
	return newWidget;
}

void UISystem::SetLastHoveredWidget(UIWidget* lastHoveredWidget)
{
	m_lastHoveredWidget = lastHoveredWidget;
	SetSelectedInputField(nullptr);
}

UIWidget* UISystem::GetPreviousWidget() const
{
	if (!m_lastHoveredWidget)
	{
		return m_rootWidget->m_children[m_rootWidget->m_children.size() - 1];
	}

	if (m_lastHoveredWidget == m_rootWidget->m_children[0])
	{
		return m_rootWidget->m_children[m_rootWidget->m_children.size() - 1];
	}

	for (int widgetIndex = 0; widgetIndex < (int)m_rootWidget->m_children.size(); widgetIndex++)
	{
		if (m_rootWidget->m_children[widgetIndex] == m_lastHoveredWidget)
		{
			return m_rootWidget->m_children[widgetIndex - 1];
		}
	}

	return nullptr;
}

UIWidget* UISystem::GetNextWidget() const
{
	if (!m_lastHoveredWidget)
	{
		return m_rootWidget->m_children[0];
	}

	if (m_lastHoveredWidget == m_rootWidget->m_children[m_rootWidget->m_children.size() - 1])
	{
		return m_rootWidget->m_children[0];
	}

	for (int widgetIndex = 0; widgetIndex < (int)m_rootWidget->m_children.size(); widgetIndex++)
	{
		if (m_rootWidget->m_children[widgetIndex] == m_lastHoveredWidget)
		{
			return m_rootWidget->m_children[widgetIndex + 1];
		}
	}

	return nullptr;
}

void UISystem::SetSelectedInputField(UIWidget* selectedTextInputFieldWidget)
{
	if (m_selectedInputFieldWidget && m_selectedInputFieldWidget->m_text.empty())
	{
		m_selectedInputFieldWidget->m_text = m_selectedInputFieldWidget->m_previousText;
	}

	m_selectedInputFieldWidget = selectedTextInputFieldWidget;
}

void UISystem::SetFocus(bool focus)
{
	m_rootWidget->SetFocus(focus);
}

UIWidget* UISystem::GetWidgetAtNormalizedCoords(Vec2 const& normalizedCoords) const
{
	Mat44 rootWidgetModelMatrix = m_rootWidget->GetModelMatrix();
	Vec2 viewSpaceCoords = rootWidgetModelMatrix.TransformPosition2D(normalizedCoords);

	for (int widgetIndex = 0; widgetIndex < (int)m_rootWidget->m_children.size(); widgetIndex++)
	{
		UIWidget* const& widget = m_rootWidget->m_children[widgetIndex];

		UIWidget* impactWidget = widget->GetWidgetAtViewSpaceCoords(viewSpaceCoords);
		if (impactWidget)
		{
			return impactWidget;
		}
	}

	return nullptr;
}

void UISystem::Clear()
{
	for (int childIndex = 0; childIndex < (int)m_rootWidget->m_children.size(); childIndex++)
	{
		delete m_rootWidget->m_children[childIndex];
	}
	m_rootWidget->m_children.clear();
	m_lastHoveredWidget = nullptr;
}

bool UISystem::Event_HandleCharacterPressed(EventArgs& args)
{
	if (!g_ui || !g_ui->m_selectedInputFieldWidget)
	{
		return false;
	}

	unsigned char keyCode = static_cast<unsigned char>(args.GetValue("KeyCode", KEYCODE_INVALID));

	if (keyCode == KEYCODE_PERIOD)
	{
		g_ui->m_selectedInputFieldWidget->HandleCharacterPressed('.');
		return true;
	}

	if (keyCode >= 32 && keyCode <= 126 && keyCode != '~' && keyCode != '`')
	{
		g_ui->m_selectedInputFieldWidget->HandleCharacterPressed(keyCode);
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

bool UISystem::Event_HandleKeypressed(EventArgs& args)
{
	if (!g_ui || !g_ui->m_selectedInputFieldWidget)
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
		g_ui->m_selectedInputFieldWidget->HandleKeyPressed(keyCode);
		return true;
	}

	if (keyCode >= 32 && keyCode <= 126 && keyCode != '~' && keyCode != '`')
	{
		return true;
	}

	return false;
}
