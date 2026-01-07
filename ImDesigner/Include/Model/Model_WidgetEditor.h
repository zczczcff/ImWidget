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
public:
	Model_WidgetEditor(ImGuiWidget::ImWidget* rootwidget);
	bool RemoveChildWidget(ImGuiWidget::ImWidget* WidgetToRemove);
	ImGuiWidget::ImWidget* GetRootWidget() { return RootWidget; }
	bool InsertChildTo(ImGuiWidget::ImWidget* child, ImGuiWidget::ImPanelWidget* Target, int InsertIndex);
	void EditProperty(const ImGuiWidget::PropertyInfo& propInfo, const void* NewValue);
	void Undo();
	bool CanUndo();
	bool CanRedo();
public:
};