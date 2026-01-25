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
	std::string EditedFileFullPath;
	std::vector<EditorActionID> FileActions;
	std::vector<EditorEventID> FileEvents;
public:
	//ImMulticastDelegate<const ImGuiWidget::PropertyInfo&, const void*, ImGuiWidget::ImObject*> OnPropertyChanged;
	//ImMulticastDelegate<> OnRequestUndo;
public:
	UI_DetailView(const std::string& widgetname, const std::string& EditedFileFullPath):
		ImGuiWidget::ImUserWidget(widgetname),
		CurrentWidget(nullptr),
		EditedFileFullPath(EditedFileFullPath)
	{
		SetFocusable(true);
		ActionInit();
		EventInit();
	}
private:
	virtual void OnKeyDown(ImGuiWidget::ImKeyDownEvent& e) override;
	void ActionInit();
	void EventInit();
	void ResetEvent();
	void ResetFileAction();
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

	void ExecutePropertyEditAction(const ImGuiWidget::PropertyInfo&, const void*, ImGuiWidget::ImObject*);

	void UpdatePropertyDisplay(ImGuiWidget::ImObject* Target, const std::string& PropertyName);
};