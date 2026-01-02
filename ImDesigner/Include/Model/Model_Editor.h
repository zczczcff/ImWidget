#pragma once
#include <map>
#include <string>
#include "ImWidget/ImBasicWidgetDeclaration.h"

class Model_Editor
{
private:
	ImGuiWidget::ImWidget* RootWidget;
public:
	bool RemoveChildWidget(ImGuiWidget::ImWidget* WidgetToRemove);
	ImGuiWidget::ImWidget* GetRootWidget() { return RootWidget; }
	void SetRootWidget(ImGuiWidget::ImWidget* NewRootWidget);
	bool InsertChildTo(ImGuiWidget::ImWidget* child, ImGuiWidget::ImPanelWidget* Target, int InsertIndex);
public:
};