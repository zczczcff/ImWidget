#pragma once
#include <map>
#include <string>
#include "ImWidget/ImBasicWidgetDeclaration.h"
#include "ImTools/ImDelegate.h"
#include "ImWidget/ImWidgetProperty.h"
class Model_WidgetEditor
{
private:
	ImGuiWidget::ImWidget* RootWidget;
	std::unordered_set<std::string> ExistedWidgetName;
	class EditCommandManager* m_EditCommandManager;
public:
	ImMulticastDelegate<> OnWidgetTreeChanged;
	ImMulticastDelegate<ImGuiWidget::PropertyStruct*, const std::string&> OnPropertyEditUnDoRedo;
	ImMulticastDelegate<> OnChildEditUndoRedo;
	ImMulticastDelegate<bool, bool> OnUndoRedoStateChanged;

	void CollectWidgetNames(ImGuiWidget::ImWidget* widget);
public:
	Model_WidgetEditor(ImGuiWidget::ImWidget* rootwidget);
	ImGuiWidget::ImWidget* GetRootWidget() { return RootWidget; }
	bool RemoveChildWidget(ImGuiWidget::ImWidget* WidgetToRemove);
	bool InsertChildTo(ImGuiWidget::ImWidget* child, ImGuiWidget::ImWidget* Target, int InsertIndex);
	bool InsertChildTo(const std::string& WidgetRegisterName, ImGuiWidget::ImWidget* Target, int InsertIndex);
	void EditProperty(const ImGuiWidget::PropertyInfo& propInfo, const void* NewValue, ImGuiWidget::PropertyStruct* Target);
	void Undo();
	void Redo();
	bool CanUndo();
	bool CanRedo();
private:
	void UpdateUndoRedoState();
public:
};