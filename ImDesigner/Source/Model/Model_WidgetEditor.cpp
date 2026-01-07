#include "Model/Model_WidgetEditor.h"
#include "ImWidget/ImWidget.h"
#include "ImWidget/ImPanelWidget.h"
#include "Tools/JLog.h"
#include "Model/Command/CommandManager.h"

Model_WidgetEditor::Model_WidgetEditor(ImGuiWidget::ImWidget* rootwidget):
	RootWidget(rootwidget),
	m_EditCommandManager(new EditCommandManager())
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

void Model_WidgetEditor::EditProperty(const ImGuiWidget::PropertyInfo& propInfo, const void* NewValue)
{
	m_EditCommandManager->ExecutePropertyEdit(propInfo, NewValue);
}

void Model_WidgetEditor::Undo()
{
	if (CanUndo())
	{
		m_EditCommandManager->Undo();
	}
}

bool Model_WidgetEditor::CanUndo()
{
	return m_EditCommandManager->CanUndo();
}

bool Model_WidgetEditor::CanRedo()
{
	return m_EditCommandManager->CanRedo();
}
