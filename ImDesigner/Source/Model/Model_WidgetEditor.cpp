#include "Model/Model_WidgetEditor.h"
#include "ImWidget/ImWidget.h"
#include "ImWidget/ImPanelWidget.h"

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
		delete WidgetToRemove;
	}
	OnWidgetTreeChanged.Broadcast();
	return true;
}

bool Model_WidgetEditor::InsertChildTo(ImGuiWidget::ImWidget* child, ImGuiWidget::ImPanelWidget* Target, int InsertIndex)
{
	return false;
}
