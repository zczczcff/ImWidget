#pragma once
#include "ImWidget/ImUserWidget.h"
#include "ImWidget/ImBasicWidgetDeclaration.h"
#include "ImTools/ImDelegate.h"
#include <map>
#include "EditorGlobalInterface.h"

class UI_DetailView : public ImGuiWidget::ImUserWidget,public EditorGlobalInterface
{
private:
	struct PropertyInfor
	{
		ImGuiWidget::ImWidget* WidgetOwner;
		std::unordered_map<std::string, std::function<void()>> Updaters;
	};
private:
	std::map<ImGuiWidget::ImWidget*, ImGuiWidget::ImVerticalBox*> CachedDetails;
	std::unordered_map<ImGuiWidget::ImObject*, PropertyInfor*> CachedPropertyInfors;
	ImGuiWidget::ImWidget* CurrentWidget;
public:
	ImMulticastDelegate<const ImGuiWidget::PropertyInfo&, const void*, ImGuiWidget::ImObject*> OnPropertyChanged;
	ImMulticastDelegate<> OnRequestUndo;
public:
	UI_DetailView(const std::string& widgetname):
		ImGuiWidget::ImUserWidget(widgetname),
		CurrentWidget(nullptr)
	{
		SetFocusable(true);
		ActionInit();
	}
private:
	virtual void OnKeyDown(ImGuiWidget::ImKeyDownEvent& e) override;
	void ActionInit();
public:

	ImGuiWidget::ImHorizontalBox* HandleAddStringItem
	(const ImGuiWidget::PropertyInfo& SingleProperty, 
		std::string& SingleString,
		ImGuiWidget::ImVerticalBox* StringListBox,
		ImGuiWidget::ImObject* Target,
		ImGuiWidget::ImWidget* WidgetOwner);

	void HandleSingleProperty(
		const ImGuiWidget::PropertyInfo& SingleProperty, 
		ImGuiWidget::ImVerticalBox* CurrentVerticalBox,
		ImGuiWidget::ImObject* Target,
		ImGuiWidget::ImWidget* WidgetOwner,
		std::unordered_map<std::string, std::function<void()>>& Updaters);

	void SetCurrentWidget(ImGuiWidget::ImWidget* widget);

	void UpdatePropertyDisplay(ImGuiWidget::ImObject* Target, const std::string& PropertyName);
};