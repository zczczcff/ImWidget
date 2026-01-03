#pragma once
#include "ImWidget/ImUserWidget.h"
#include "ImWidget/ImBasicWidgetDeclaration.h"
#include <map>

class UI_DetailView : public ImGuiWidget::ImUserWidget
{
private:
	std::map<ImGuiWidget::ImWidget*, ImGuiWidget::ImVerticalBox*> CachedDetails;
	ImGuiWidget::ImWidget* CurrentWidget;
public:
	UI_DetailView(const std::string& widgetname):ImGuiWidget::ImUserWidget(widgetname){}
public:

	ImGuiWidget::ImHorizontalBox* HandleAddStringItem(const ImGuiWidget::PropertyInfo& SingleProperty, std::string& SingleString, ImGuiWidget::ImVerticalBox* StringListBox);

	void HandleSingleProperty(const ImGuiWidget::PropertyInfo& SingleProperty, ImGuiWidget::ImVerticalBox* CurrentVerticalBox);

	void SetCurrentWidget(ImGuiWidget::ImWidget* widget);
};