#include "Engine/UI/UIWidget.hpp"

#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/DebugRenderSystem.hpp"
#include "Engine/UI/UISystem.hpp"


UIWidget::~UIWidget()
{
	for (int selfIndexInParent = 0; selfIndexInParent < (int)m_parent->m_children.size(); selfIndexInParent++)
	{
		if (m_parent->m_children[selfIndexInParent] == this)
		{
			m_parent->m_children.erase(m_parent->m_children.begin() + selfIndexInParent);
			break;
		}
	}

	for (int childIndex = 0; childIndex < (int)m_children.size(); childIndex++)
	{
		delete m_children[childIndex];
	}
}

UIWidget::UIWidget(UISystem* uiSystem)
	: m_uiSystem(uiSystem)
	, m_blinkingCaretTimer(new Stopwatch(0.5f))
{
}

void UIWidget::Update()
{
	if (!m_hasFocus)
	{
		AABB2 const bounds = GetViewSpaceBounds();
		m_renderBounds = bounds.GetBoxAtUVs(Vec2::ZERO, Vec2::ONE);
		m_renderBounds.m_mins += Vec2(m_borderRadius, m_borderRadius);
		m_renderBounds.m_maxs -= Vec2(m_borderRadius, m_borderRadius);
		return;
	}

	if (m_isTextInputField && m_uiSystem->m_selectedInputFieldWidget)
	{
		while (m_blinkingCaretTimer->DecrementDurationIfElapsed())
		{
			m_isCaretVisible = !m_isCaretVisible;
		}
	}

	Camera const& camera = m_uiSystem->m_config.m_camera;
	AABB2 const bounds = GetViewSpaceBounds();

	Vec2 cursorNormalizedPosition = m_uiSystem->m_config.m_input->GetCursorNormalizedPosition();
	float cursorViewPositionX = RangeMap(cursorNormalizedPosition.x, 0.f, 1.f, camera.GetOrthoBottomLeft().x, camera.GetOrthoTopRight().x);
	float cursorViewPositionY = RangeMap(cursorNormalizedPosition.y, 0.f, 1.f, camera.GetOrthoBottomLeft().y, camera.GetOrthoTopRight().y);
	Vec2 cursorViewPosition(cursorViewPositionX, cursorViewPositionY);

	if (bounds.IsPointInside(cursorViewPosition))
	{
		if (m_isScrollable)
		{
			AddScroll((float)g_input->m_cursorState.m_wheelScrollDelta);
		}

		if (!m_clickEventName.empty())
		{
			m_uiSystem->SetLastHoveredWidget(this);
		}

		if (m_isTextInputField && m_uiSystem->m_config.m_input->WasKeyJustPressed(KEYCODE_LMB))
		{
			m_previousText = m_text;
			m_text = "";
			m_caretPosition = 0;
			m_uiSystem->SetSelectedInputField(this);
			m_blinkingCaretTimer->Start();
		}
		else if (!m_clickEventName.empty() && m_uiSystem->m_config.m_input->WasKeyJustPressed(KEYCODE_LMB))
		{
			m_consumedClickStart = true;
			m_renderBounds = bounds.GetBoxAtUVs(Vec2(0.01f, 0.01f), Vec2(0.99f, 0.99f));
			m_renderBounds.m_mins += Vec2(m_borderRadius, m_borderRadius);
			m_renderBounds.m_maxs -= Vec2(m_borderRadius, m_borderRadius);
			m_fontSizeMultiplier = 0.98f;
			m_isClicked = true;
		}
		else if (!m_clickEventName.empty() && m_fontSizeMultiplier != 1.f && m_uiSystem->m_config.m_input->WasKeyJustReleased(KEYCODE_LMB))
		{
			m_isClicked = false;
			m_renderBounds = bounds;
			m_renderBounds.m_mins += Vec2(m_borderRadius, m_borderRadius);
			m_renderBounds.m_maxs -= Vec2(m_borderRadius, m_borderRadius);
			m_fontSizeMultiplier = 1.f;
			FireEvent(m_clickEventName);
		}
	}
	else
	{
		m_fontSizeMultiplier = 1.f;
		m_renderBounds = bounds;
		m_renderBounds.m_mins += Vec2(m_borderRadius, m_borderRadius);
		m_renderBounds.m_maxs -= Vec2(m_borderRadius, m_borderRadius);
		m_isClicked = false;
	}

	if (!m_isClicked && !m_isVRClicked)
	{
		m_renderBounds = bounds;
		m_renderBounds.m_mins += Vec2(m_borderRadius, m_borderRadius);
		m_renderBounds.m_maxs -= Vec2(m_borderRadius, m_borderRadius);
	}

	for (int childIndex = 0; childIndex < (int)m_children.size(); childIndex++)
	{
		m_children[childIndex]->Update();
	}

	m_isVRHovered = false;
	m_isVRClicked = false;
}

void UIWidget::Render() const
{
	if (!m_visible)
	{
		return;
	}
	if (m_parent && !DoAABB2Overlap(GetViewSpaceBounds(), m_parent->GetViewSpaceBounds()))
	{
		return;
	}

	Camera const& camera = m_uiSystem->m_config.m_camera;
	AABB2 const bounds = GetViewSpaceBounds();

	Rgba8 backgroundColor = m_backgroundColor;
	Rgba8 color = m_color;
	Rgba8 borderColor = m_borderColor;

	Vec2 cursorNormalizedPosition = g_input->GetCursorNormalizedPosition();
	float cursorViewPositionX = RangeMap(cursorNormalizedPosition.x, 0.f, 1.f, camera.GetOrthoBottomLeft().x, camera.GetOrthoTopRight().x);
	float cursorViewPositionY = RangeMap(cursorNormalizedPosition.y, 0.f, 1.f, camera.GetOrthoBottomLeft().y, camera.GetOrthoTopRight().y);
	Vec2 cursorViewPosition = Vec2(cursorViewPositionX, cursorViewPositionY);

	if (m_hasFocus && bounds.IsPointInside(cursorViewPosition))
	{
		backgroundColor = m_hoverBackgroundColor;
		color = m_hoverColor.a != 0 ? m_hoverColor : m_color;
		borderColor = m_hoverBorderColor;
	}

	if (m_uiSystem->m_config.m_supportKeyboard && m_uiSystem->GetLastHoveredWidget() == this)
	{
		backgroundColor = m_hoverBackgroundColor;
		color = m_hoverColor.a != 0 ? m_hoverColor : m_color;
		borderColor = m_hoverBorderColor;
	}

	if (m_isVRHovered)
	{
		backgroundColor = m_hoverBackgroundColor;
		color = m_hoverColor.a != 0 ? m_hoverColor : m_color;
		borderColor = m_hoverBorderColor;
	}

	m_uiSystem->m_config.m_renderer->SetModelConstants();

	std::vector<Vertex_PCU> vertexes;
	AddVertsForAABB2(vertexes, m_renderBounds, backgroundColor);
	AddVertsForAABB2(vertexes, AABB2(m_renderBounds.m_mins + m_borderRadius * Vec2::SOUTH, m_renderBounds.m_mins + m_renderBounds.GetDimensions().x * Vec2::EAST), backgroundColor);
	AddVertsForAABB2(vertexes, AABB2(m_renderBounds.m_mins + m_borderRadius * Vec2::WEST, m_renderBounds.m_mins + m_renderBounds.GetDimensions().y * Vec2::NORTH), backgroundColor);
	AddVertsForAABB2(vertexes, AABB2(m_renderBounds.m_mins + m_renderBounds.GetDimensions().y * Vec2::NORTH, m_renderBounds.m_maxs + m_borderRadius * Vec2::NORTH), backgroundColor);
	AddVertsForAABB2(vertexes, AABB2(m_renderBounds.m_maxs + m_renderBounds.GetDimensions().y * Vec2::SOUTH, m_renderBounds.m_maxs + m_borderRadius * Vec2::EAST), backgroundColor);
	AddVertsForOrientedSector2D(vertexes, m_renderBounds.m_mins, 225.f, 90.f, m_borderRadius, backgroundColor);
	AddVertsForOrientedSector2D(vertexes, Vec2(m_renderBounds.m_mins.x, m_renderBounds.m_maxs.y), 135.f, 90.f, m_borderRadius, backgroundColor);
	AddVertsForOrientedSector2D(vertexes, m_renderBounds.m_maxs, 45.f, 90.f, m_borderRadius, backgroundColor);
	AddVertsForOrientedSector2D(vertexes, Vec2(m_renderBounds.m_maxs.x, m_renderBounds.m_mins.y), 315.f, 90.f, m_borderRadius, backgroundColor);

	AddVertsForLineSegment2D(vertexes, m_renderBounds.m_mins + m_borderRadius * Vec2::WEST, Vec2(m_renderBounds.m_mins.x - m_borderRadius, m_renderBounds.m_maxs.y), m_borderWidth, borderColor);
	AddVertsForLineSegment2D(vertexes, Vec2(m_renderBounds.m_mins.x, m_renderBounds.m_maxs.y + m_borderRadius), m_renderBounds.m_maxs + m_borderRadius * Vec2::NORTH, m_borderWidth, borderColor);
	AddVertsForLineSegment2D(vertexes, m_renderBounds.m_maxs + m_borderRadius * Vec2::EAST, Vec2(m_renderBounds.m_maxs.x + m_borderRadius, m_renderBounds.m_mins.y), m_borderWidth, borderColor);
	AddVertsForLineSegment2D(vertexes, Vec2(m_renderBounds.m_maxs.x, m_renderBounds.m_mins.y - m_borderRadius), m_renderBounds.m_mins + m_borderRadius * Vec2::SOUTH, m_borderWidth, borderColor);
	AddVertsForArc2D(vertexes, m_renderBounds.m_mins, m_borderRadius, m_borderWidth * 1.5f, 180.f, 270.f, borderColor);
	AddVertsForArc2D(vertexes, Vec2(m_renderBounds.m_mins.x, m_renderBounds.m_maxs.y), m_borderRadius, m_borderWidth * 1.5f, 90.f, 180.f, borderColor);
	AddVertsForArc2D(vertexes, m_renderBounds.m_maxs, m_borderRadius, m_borderWidth * 1.5f, 0.f, 90.f, borderColor);
	AddVertsForArc2D(vertexes, Vec2(m_renderBounds.m_maxs.x, m_renderBounds.m_mins.y), m_borderRadius, m_borderWidth * 1.5f, 270.f, 360.f, borderColor);

	m_uiSystem->m_config.m_renderer->BindTexture(m_backgroundTexture);
	m_uiSystem->m_config.m_renderer->DrawVertexArray(vertexes);

	if (m_isTextInputField)
	{
		if (m_text.empty())
		{
			std::vector<Vertex_PCU> textInfoVertexes;
			m_uiSystem->m_font->AddVertsForTextInBox2D(textInfoVertexes, m_renderBounds, m_fontSize * m_fontSizeMultiplier, m_textInputFieldInfoText, color, 0.5f, m_alignment);
			m_uiSystem->m_config.m_renderer->SetSamplerMode(SamplerMode::BILINEAR_WRAP);
			m_uiSystem->m_config.m_renderer->BindTexture(m_uiSystem->m_font->GetTexture());
			m_uiSystem->m_config.m_renderer->DrawVertexArray(textInfoVertexes);
		}
		else
		{
			std::vector<Vertex_PCU> textVertexes;
			m_uiSystem->m_font->AddVertsForTextInBox2D(textVertexes, m_renderBounds, m_fontSize * m_fontSizeMultiplier, m_text, color, 0.5f, m_alignment);
			m_uiSystem->m_config.m_renderer->SetSamplerMode(SamplerMode::BILINEAR_WRAP);
			m_uiSystem->m_config.m_renderer->BindTexture(m_uiSystem->m_font->GetTexture());
			m_uiSystem->m_config.m_renderer->DrawVertexArray(textVertexes);
		}
		if (m_uiSystem->m_selectedInputFieldWidget == this)
		{
			if (m_isCaretVisible)
			{
				std::vector<Vertex_PCU> caretVerts;
				std::string caretPositionText = "";
				for (int commandCharacterIndex = 0; commandCharacterIndex <= static_cast<int>(m_text.size()); commandCharacterIndex++)
				{
					if (commandCharacterIndex == m_caretPosition)
					{
						caretPositionText += '_';
						continue;
					}
					caretPositionText += ' ';
				}
				m_uiSystem->m_font->AddVertsForTextInBox2D(caretVerts, m_renderBounds, m_fontSize * m_fontSizeMultiplier, caretPositionText, color, 0.5f, m_alignment);
				m_uiSystem->m_config.m_renderer->SetSamplerMode(SamplerMode::BILINEAR_WRAP);
				m_uiSystem->m_config.m_renderer->BindTexture(m_uiSystem->m_font->GetTexture());
				m_uiSystem->m_config.m_renderer->DrawVertexArray(caretVerts);
			}
		}
	}
	else if (!m_text.empty())
	{
		std::vector<Vertex_PCU> textVertexes;
		m_uiSystem->m_font->AddVertsForTextInBox2D(textVertexes, m_renderBounds, m_fontSize * m_fontSizeMultiplier, m_text, color, 0.5f, m_alignment);
		m_uiSystem->m_config.m_renderer->SetSamplerMode(SamplerMode::BILINEAR_WRAP);
		m_uiSystem->m_config.m_renderer->BindTexture(m_uiSystem->m_font->GetTexture());
		m_uiSystem->m_config.m_renderer->DrawVertexArray(textVertexes);
	}
	else if (m_image)
	{
		std::vector<Vertex_PCU> imageVertexes;
		float imageAspect = (float)m_image->GetDimensions().x / (float)m_image->GetDimensions().y;
		float renderBoundsAspect = m_renderBounds.GetDimensions().x / m_renderBounds.GetDimensions().y;
		if (imageAspect > renderBoundsAspect)
		{
			// Image aspect is greater than render bounds aspect, reduce render bounds aspect Y
			m_renderBounds.SetDimensions(Vec2(m_renderBounds.GetDimensions().x, m_renderBounds.GetDimensions().x / imageAspect));
		}
		else if (imageAspect < renderBoundsAspect)
		{
			// Image aspect is less than render bounds aspect, reduce render bounds aspect X
			m_renderBounds.SetDimensions(Vec2(m_renderBounds.GetDimensions().y * imageAspect, m_renderBounds.GetDimensions().y));
		}
		AddVertsForAABB2(imageVertexes, m_renderBounds, color);
		m_uiSystem->m_config.m_renderer->BindTexture(m_image);
		m_uiSystem->m_config.m_renderer->DrawVertexArray(imageVertexes);
	}

	for (int childIndex = 0; childIndex < (int)m_children.size(); childIndex++)
	{
		m_children[childIndex]->Render();
	}
}

UIWidget* UIWidget::SetText(std::string const& text)
{
	m_text = text;
	return this;
}

UIWidget* UIWidget::SetImage(std::string const& imagePath)
{
	m_image = m_uiSystem->m_config.m_renderer->CreateOrGetTextureFromFile(imagePath.c_str());
	return this;
}

UIWidget* UIWidget::SetPosition(Vec2 const& relativePosition)
{
	m_position = relativePosition;
	return this;
}

UIWidget* UIWidget::SetDimensions(Vec2 const& relativeDimensions)
{
	m_dimensions = relativeDimensions;
	return this;
}

UIWidget* UIWidget::SetOrientation(float orientationZ)
{
	m_orientationZ = orientationZ;
	return this;
}

UIWidget* UIWidget::SetVisible(bool visible)
{
	m_visible = visible;
	return this;
}

UIWidget* UIWidget::SetFocus(bool focus)
{
	m_hasFocus = focus;
	if (m_isTextInputField && m_uiSystem->m_selectedInputFieldWidget == this)
	{
		m_uiSystem->SetSelectedInputField(nullptr);
	}
	return this;
}

UIWidget* UIWidget::SetBackgroundTexture(std::string const& texturePath)
{
	m_backgroundTexture = m_uiSystem->m_config.m_renderer->CreateOrGetTextureFromFile(texturePath.c_str());
	return this;
}

UIWidget* UIWidget::SetBackgroundColor(Rgba8 const& color)
{
	m_backgroundColor = color;
	return this;
}

UIWidget* UIWidget::SetHoverBackgroundColor(Rgba8 const& color)
{
	m_hoverBackgroundColor = color;
	return this;
}

UIWidget* UIWidget::SetColor(Rgba8 const& color)
{
	m_color = color;
	return this;
}

UIWidget* UIWidget::SetHoverColor(Rgba8 const& color)
{
	m_hoverColor = color;
	return this;
}

UIWidget* UIWidget::SetFontSize(float fontSize)
{
	m_fontSize = fontSize * m_uiSystem->m_rootWidget->m_dimensions.y / 100.f;
	return this;
}

UIWidget* UIWidget::SetPivot(Vec2 const& pivot)
{
	m_pivot = pivot;
	return this;
}

UIWidget* UIWidget::SetAlignment(Vec2 const& alignment)
{
	m_alignment = alignment;
	return this;
}

UIWidget* UIWidget::SetBorderWidth(float borderWidth)
{
	m_borderWidth = borderWidth * m_uiSystem->m_rootWidget->m_dimensions.y / 100.f;
	return this;
}

UIWidget* UIWidget::SetBorderColor(Rgba8 const& color)
{
	m_borderColor = color;
	return this;
}

UIWidget* UIWidget::SetHoverBorderColor(Rgba8 const& color)
{
	m_hoverBorderColor = color;
	return this;
}

UIWidget* UIWidget::SetBorderRadius(float borderRadius)
{
	m_borderRadius = borderRadius * m_uiSystem->m_rootWidget->m_dimensions.y / 100.f;
	return this;
}

UIWidget* UIWidget::SetClickEventName(std::string const& clickEventName)
{
	m_clickEventName = clickEventName;
	return this;
}

UIWidget* UIWidget::SetRaycastTarget(bool isRaycastTarget)
{
	m_isRaycastTarget = isRaycastTarget;
	return this;
}

UIWidget* UIWidget::SetScrollable(bool scrollable)
{
	m_isScrollable = scrollable;
	return this;
}

UIWidget* UIWidget::SetScrollValue(float scrollValue)
{
	m_scrollValue = scrollValue;
	return this;
}

UIWidget* UIWidget::AddScroll(float scrollToAdd)
{
	if (scrollToAdd > 0.f)
	{
		if (GetMinYValueInChildren() - m_scrollBuffer <= GetViewSpaceBounds().m_mins.y)
		{
			m_scrollValue += scrollToAdd * (m_uiSystem->m_config.m_camera.GetOrthoTopRight().y - m_uiSystem->m_config.m_camera.GetOrthoBottomLeft().y) / 100.f;
			if (m_scrollValue < 0.f)
			{
				m_scrollValue = 0.f;
			}
		}
	}
	else
	{
		m_scrollValue += scrollToAdd * (m_uiSystem->m_config.m_camera.GetOrthoTopRight().y - m_uiSystem->m_config.m_camera.GetOrthoBottomLeft().y) / 100.f;
		if (m_scrollValue < 0.f)
		{
			m_scrollValue = 0.f;
		}
	}

	return this;
}

UIWidget* UIWidget::SetScrollBuffer(float scrollBuffer)
{
	m_scrollBuffer = scrollBuffer;
	return this;
}

UIWidget* UIWidget::SetTextInputField(bool isTextInputField)
{
	m_isTextInputField = isTextInputField;
	return this;
}

UIWidget* UIWidget::SetTextInputFieldInfoText(std::string const& infoText)
{
	m_textInputFieldInfoText = infoText;
	return this;
}

Mat44 UIWidget::GetModelMatrix() const
{
	Mat44 modelMatrix = Mat44::IDENTITY;
	if (m_parent)
	{
		modelMatrix.Append(m_parent->GetModelMatrix());
	}

	modelMatrix.AppendTranslation2D(m_position);
	modelMatrix.AppendZRotation(m_orientationZ);
	modelMatrix.AppendScaleNonUniform2D(m_dimensions);

	return modelMatrix;
}

AABB2 UIWidget::GetViewSpaceBounds() const
{
	AABB2 localBounds(Vec2::ZERO, Vec2::ONE);
	localBounds.Translate(-m_pivot);

	Mat44 const modelMatrix = GetModelMatrix();

	AABB2 viewSpaceBounds;
	viewSpaceBounds.m_mins = modelMatrix.TransformPosition2D(localBounds.m_mins);
	viewSpaceBounds.m_maxs = modelMatrix.TransformPosition2D(localBounds.m_maxs);

	viewSpaceBounds.m_mins.y += GetScroll();
	viewSpaceBounds.m_maxs.y += GetScroll();

	return viewSpaceBounds;
}

float UIWidget::GetScroll() const
{
	float scrollValue = 0.f;
	UIWidget* currentWidget = m_parent;
	while (currentWidget)
	{
		scrollValue += currentWidget->m_scrollValue;
		currentWidget = currentWidget->m_parent;
	}
	return scrollValue;
}

UIWidget* UIWidget::GetWidgetAtViewSpaceCoords(Vec2 const& viewSpaceCoords)
{
	if (!m_visible)
	{
		return nullptr;
	}
	if (!m_hasFocus)
	{
		return nullptr;
	}

	for (int childIndex = 0; childIndex < (int)m_children.size(); childIndex++)
	{
		UIWidget* const& widget = m_children[childIndex];
		UIWidget* resultWidget = widget->GetWidgetAtViewSpaceCoords(viewSpaceCoords);
		if (resultWidget)
		{
			return resultWidget;
		}
	}

	if (GetViewSpaceBounds().IsPointInside(viewSpaceCoords) && (m_isTextInputField || !m_clickEventName.empty()) && m_isRaycastTarget)
	{
		return this;
	}
	else
	{
		return nullptr;
	}
}

float UIWidget::GetMaxYValueInChildren() const
{
	float maxYValue = -FLT_MAX;
	for (int childIndex = 0; childIndex < (int)m_children.size(); childIndex++)
	{
		AABB2 const& viewSpaceBounds = m_children[childIndex]->GetViewSpaceBounds();
		if (viewSpaceBounds.m_maxs.y > maxYValue)
		{
			maxYValue = viewSpaceBounds.m_mins.y;
		}
	}
	return maxYValue;
}

float UIWidget::GetMinYValueInChildren() const
{
	float minYValue = FLT_MAX;
	for (int childIndex = 0; childIndex < (int)m_children.size(); childIndex++)
	{
		AABB2 const& viewSpaceBounds = m_children[childIndex]->GetViewSpaceBounds();
		if (viewSpaceBounds.m_mins.y < minYValue)
		{
			minYValue = viewSpaceBounds.m_mins.y;
		}
	}

	return minYValue;
}

UIWidget* UIWidget::AddChild(UIWidget* widget)
{
	m_children.push_back(widget);
	widget->m_parent = this;

	return this;
}

void UIWidget::RemoveChild(UIWidget* childWidget)
{
	for (int childIndex = 0; childIndex < (int)m_children.size(); childIndex++)
	{
		if (m_children[childIndex] == childWidget)
		{
			m_children.erase(m_children.begin() + childIndex);
			return;
		}
	}
}

void UIWidget::HandleCharacterPressed(unsigned char character)
{
	m_text.insert(m_text.begin() + m_caretPosition, character);
	m_caretPosition++;
}

void UIWidget::HandleKeyPressed(unsigned char keyCode)
{
	if (keyCode == KEYCODE_BACKSPACE)
	{
		if (m_caretPosition == 0)
		{
			return;
		}
		m_text.erase(m_caretPosition - 1, 1);
		m_caretPosition--;
	}
	else if (keyCode == KEYCODE_DELETE)
	{
		if (m_caretPosition == (int)m_text.size())
		{
			return;
		}
		m_text.erase(m_caretPosition, 1);
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
		if (m_caretPosition == (int)m_text.size())
		{
			return;
		}
		m_caretPosition++;
	}
	else if (keyCode == KEYCODE_ENTER)
	{
		if (m_text.empty())
		{
			m_text = m_previousText;
		}
		m_caretPosition = 0;
		m_blinkingCaretTimer->Stop();
		m_isCaretVisible = false;
		m_uiSystem->SetSelectedInputField(nullptr);
	}
	else if (keyCode == KEYCODE_HOME)
	{
		m_caretPosition = 0;
	}
	else if (keyCode == KEYCODE_END)
	{
		m_caretPosition = static_cast<int>(m_text.size());
	}
	else if (keyCode == KEYCODE_ESC)
	{
		m_caretPosition = 0;
		m_text = m_previousText;
		m_uiSystem->SetSelectedInputField(nullptr);
		m_blinkingCaretTimer->Stop();
		m_isCaretVisible = false;
	}

	m_blinkingCaretTimer->Restart();
}
