#include "Model/Model_WidgetEditor.h"
#include "ImWidget/ImWidget.h"
#include "ImWidget/ImPanelWidget.h"
#include "Tools/JLog.h"

Model_WidgetEditor::Model_WidgetEditor(ImGuiWidget::ImWidget* rootwidget):
	RootWidget(rootwidget)
{
}

bool Model_WidgetEditor::RemoveChildWidget(ImGuiWidget::ImWidget* WidgetToRemove)
{
	if (!WidgetToRemove) return false;
	if (!WidgetToRemove->IsInTree(RootWidget)) return false;
	if (WidgetToRemove->GetParents()->RemoveChild(WidgetToRemove))
	{
		AddLogLineEx(u8"É¾³ý¿Ø¼þ£º[", WidgetToRemove->GetRegisterTypeName(), u8"]", WidgetToRemove->GetWidgetName());
		delete WidgetToRemove;
	}
	OnWidgetTreeChanged.Broadcast();
	
	return true;
}

bool Model_WidgetEditor::InsertChildTo(ImGuiWidget::ImWidget* child, ImGuiWidget::ImPanelWidget* Target, int InsertIndex)
{
	return false;
}
