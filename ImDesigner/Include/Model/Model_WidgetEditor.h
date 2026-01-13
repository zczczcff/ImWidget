#pragma once
#include <map>
#include <string>
#include "ImWidget/ImBasicWidgetDeclaration.h"
#include "ImTools/ImDelegate.h"
#include "ImWidget/ImObject.h"
class Model_WidgetEditor
{
private:
	ImGuiWidget::ImWidget* RootWidget;
	std::unordered_set<std::string>& ExistedWidgetName;
	class EditCommandManager* m_EditCommandManager;
public:
	ImMulticastDelegate<> OnWidgetTreeChanged;
	ImMulticastDelegate<ImGuiWidget::ImObject*, const std::string&> OnPropertyEditUnDoRedo;
	ImMulticastDelegate<bool, bool> OnUndoRedoStateChanged;

	void CollectWidgetNames(ImGuiWidget::ImWidget* widget);
public:
	Model_WidgetEditor(ImGuiWidget::ImWidget* rootwidget, std::unordered_set<std::string>&);
	ImGuiWidget::ImWidget* GetRootWidget() { return RootWidget; }
	bool RemoveChildWidget(ImGuiWidget::ImWidget* WidgetToRemove);
	bool InsertChildTo(ImGuiWidget::ImWidget* child, ImGuiWidget::ImWidget* Target, int InsertIndex);
	bool InsertChildTo(const std::string& WidgetRegisterName, ImGuiWidget::ImWidget* Target, int InsertIndex);
	bool EditProperty(const ImGuiWidget::PropertyInfo& propInfo, const void* NewValue, ImGuiWidget::ImObject* Target);
	void Undo();
	void Redo();
	bool CanUndo();
	bool CanRedo();
private:
	void UpdateUndoRedoState();
public:
};