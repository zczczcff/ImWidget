#pragma once
#include <map>
#include <string>
#include "ImWidget/ImBasicWidgetDeclaration.h"

class Model_WidgetEditor
{
private:
	ImGuiWidget::ImWidget* RootWidget;
public:
	Model_WidgetEditor(ImGuiWidget::ImWidget* rootwidget);
	bool RemoveChildWidget(ImGuiWidget::ImWidget* WidgetToRemove);
	ImGuiWidget::ImWidget* GetRootWidget() { return RootWidget; }
	bool InsertChildTo(ImGuiWidget::ImWidget* child, ImGuiWidget::ImPanelWidget* Target, int InsertIndex);
public:
};