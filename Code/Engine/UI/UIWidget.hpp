#pragma once

#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/Texture.hpp"


class UISystem;


class UIWidget
{
public:
	~UIWidget();
	UIWidget() = default;
	explicit UIWidget(UISystem* uiSystem);

	void Update();
	void Render() const;

	UIWidget* SetText(std::string const& text);
	UIWidget* SetImage(std::string const& imagePath);
	UIWidget* SetPosition(Vec2 const& relativePosition);
	UIWidget* SetDimensions(Vec2 const& relativeDimensions);
	UIWidget* SetOrientation(float orientationZ);
	UIWidget* SetVisible(bool visible);
	UIWidget* SetFocus(bool focus);
	UIWidget* SetBackgroundTexture(std::string const& texturePath);
	UIWidget* SetBackgroundColor(Rgba8 const& color);
	UIWidget* SetHoverBackgroundColor(Rgba8 const& color);
	UIWidget* SetColor(Rgba8 const& color);
	UIWidget* SetHoverColor(Rgba8 const& color);
	UIWidget* SetFontSize(float fontSize);
	UIWidget* SetPivot(Vec2 const& pivot);
	UIWidget* SetAlignment(Vec2 const& alignment);
	UIWidget* SetBorderWidth(float borderWidth);
	UIWidget* SetBorderColor(Rgba8 const& color);
	UIWidget* SetHoverBorderColor(Rgba8 const& color);
	UIWidget* SetBorderRadius(float borderRadius);
	UIWidget* SetClickEventName(std::string const& clickEventName);
	UIWidget* SetRaycastTarget(bool isRaycastTarget);
	UIWidget* SetScrollable(bool scrollable);
	UIWidget* SetScrollValue(float scrollValue);
	UIWidget* AddScroll(float scrollToAdd);
	UIWidget* SetScrollBuffer(float scrollBuffer);
	UIWidget* SetTextInputField(bool isTextInputField);
	UIWidget* SetTextInputFieldInfoText(std::string const& infoText);

	Mat44 GetModelMatrix() const;
	AABB2 GetViewSpaceBounds() const;
	float GetScroll() const;

	UIWidget* GetWidgetAtViewSpaceCoords(Vec2 const& viewSpaceCoords);
	float GetMaxYValueInChildren() const;
	float GetMinYValueInChildren() const;

	UIWidget* AddChild(UIWidget* widget);
	void RemoveChild(UIWidget* childWidget);
	void HandleCharacterPressed(unsigned char character);
	void HandleKeyPressed(unsigned char keyCode);

public:
	UISystem* m_uiSystem = nullptr;
	std::vector<UIWidget*> m_children;
	UIWidget* m_parent = nullptr;
	bool m_visible = true;
	bool m_hasFocus = true;
	bool m_consumedClickStart = false;
	Texture* m_backgroundTexture = nullptr;
	Rgba8 m_backgroundColor = Rgba8::TRANSPARENT_BLACK;
	Rgba8 m_hoverBackgroundColor = Rgba8::TRANSPARENT_BLACK;
	Rgba8 m_color = Rgba8::TRANSPARENT_BLACK;
	Rgba8 m_hoverColor = Rgba8::TRANSPARENT_BLACK;
	float m_fontSize = 0.f;
	Vec2 m_pivot = Vec2::ZERO;
	Vec2 m_alignment = Vec2::ZERO;
	float m_borderWidth = 0.f;
	Rgba8 m_borderColor = Rgba8::TRANSPARENT_BLACK;
	Rgba8 m_hoverBorderColor = Rgba8::TRANSPARENT_BLACK;
	float m_borderRadius = 0.f;
	Vec2 m_position = Vec2::ZERO;
	Vec2 m_dimensions = Vec2::ZERO;
	float m_orientationZ = 0.f;
	std::string m_clickEventName = "";
	std::string m_text = "";
	Texture* m_image = nullptr;
	bool m_isHovered = false;
	bool m_isVRHovered = false;
	bool m_isRaycastTarget = true;
	bool m_isVRClicked = false;
	float m_scrollValue = 0.f;
	bool m_isScrollable = false;
	float m_scrollBuffer = 0.f;

	bool m_isTextInputField = false;
	std::string m_textInputFieldInfoText = "";
	int m_caretPosition = 0;
	bool m_isCaretVisible = false;
	Stopwatch* m_blinkingCaretTimer = nullptr;
	std::string m_previousText = "";

	mutable AABB2 m_renderBounds;
	float m_fontSizeMultiplier = 1.f;
	bool m_isClicked = false;
};
