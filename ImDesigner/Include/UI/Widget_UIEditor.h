#pragma once
#include "ImWidget/ImUserWidget.h"

class Widget_UIEditor :public ImGuiWidget::ImUserWidget
{
private:
	ImGuiWidget::ImWidget* EditorRootWidget;
public:
	Widget_UIEditor(const std::string& name, ImGuiWidget::ImWidget* EditorRootWidget) :
		ImUserWidget(name),
		EditorRootWidget(EditorRootWidget)
	{

	}

};