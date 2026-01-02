#pragma once
#include "ImWidget/ImUserWidget.h"

class UI_WidgetEditor :public ImGuiWidget::ImUserWidget
{
private:
	ImGuiWidget::ImWidget* EditorRootWidget;
public:
	UI_WidgetEditor(const std::string& name, ImGuiWidget::ImWidget* EditorRootWidget) :
		ImUserWidget(name),
		EditorRootWidget(EditorRootWidget)
	{

	}

};