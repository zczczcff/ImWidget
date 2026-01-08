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
	class EditCommandManager* m_EditCommandManager;
public:
	ImMultiDelegate<> OnWidgetTreeChanged;
	ImMulticastDelegate<ImGuiWidget::PropertyStruct*, const std::string&> OnPropertyEditUnDoRedo;
	ImMultiDelegate<bool, bool> OnUndoRedoStateChanged;
public:
	Model_WidgetEditor(ImGuiWidget::ImWidget* rootwidget);
	bool RemoveChildWidget(ImGuiWidget::ImWidget* WidgetToRemove);
	ImGuiWidget::ImWidget* GetRootWidget() { return RootWidget; }
	bool InsertChildTo(ImGuiWidget::ImWidget* child, ImGuiWidget::ImPanelWidget* Target, int InsertIndex);
	void EditProperty(const ImGuiWidget::PropertyInfo& propInfo, const void* NewValue, ImGuiWidget::PropertyStruct* Target);
	void Undo();
	void Redo();
	bool CanUndo();
	bool CanRedo();
private:
	void UpdateUndoRedoState();
public:
};