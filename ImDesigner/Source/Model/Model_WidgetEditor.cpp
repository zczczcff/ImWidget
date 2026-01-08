#include "Model/Model_WidgetEditor.h"
#include "ImWidget/ImWidget.h"
#include "ImWidget/ImPanelWidget.h"
#include "Tools/JLog.h"
#include "Model/Command/CommandManager.h"

Model_WidgetEditor::Model_WidgetEditor(ImGuiWidget::ImWidget* rootwidget):
	RootWidget(rootwidget),
	m_EditCommandManager(new EditCommandManager())
{
	m_EditCommandManager->OnPropertyEditUnDoRedo.Add([this](ImGuiWidget::PropertyStruct* target, const std::string& propertyname)
		{
			OnPropertyEditUnDoRedo.Broadcast(target, propertyname);
		});

	m_EditCommandManager->OnChildEditUndoRedo.Add([this]()
		{
			OnWidgetTreeChanged.Broadcast();
		});
}

bool Model_WidgetEditor::RemoveChildWidget(ImGuiWidget::ImWidget* WidgetToRemove)
{
	if (!WidgetToRemove) return false;
	if (!WidgetToRemove->IsInTree(RootWidget)) return false;

	//if (WidgetToRemove->GetParents()->RemoveChild(WidgetToRemove))
	//{
	//	AddLogLineEx(u8"É¾³ý¿Ø¼þ£º[", WidgetToRemove->GetRegisterTypeName(), u8"]", WidgetToRemove->GetWidgetName());
	//	delete WidgetToRemove;
	//}
	//OnWidgetTreeChanged.Broadcast();
	if (m_EditCommandManager->ExecuteChildRemove(WidgetToRemove->GetParents(), WidgetToRemove))
	{
		OnWidgetTreeChanged.Broadcast();
	}
	return true;
}

bool Model_WidgetEditor::InsertChildTo(ImGuiWidget::ImWidget* child, ImGuiWidget::ImPanelWidget* Target, int InsertIndex)
{
	return false;
}

void Model_WidgetEditor::EditProperty(const ImGuiWidget::PropertyInfo& propInfo, const void* NewValue, ImGuiWidget::PropertyStruct* Target)
{
	m_EditCommandManager->ExecutePropertyEdit(propInfo, NewValue, Target);
	UpdateUndoRedoState();
}

void Model_WidgetEditor::Undo()
{
	if (CanUndo())
	{
		m_EditCommandManager->Undo();
	}
	UpdateUndoRedoState();
}

void Model_WidgetEditor::Redo()
{
	if (CanRedo())
	{
		m_EditCommandManager->Redo();
	}
	UpdateUndoRedoState();
}

bool Model_WidgetEditor::CanUndo()
{
	return m_EditCommandManager->CanUndo();
}

bool Model_WidgetEditor::CanRedo()
{
	return m_EditCommandManager->CanRedo();
}

void Model_WidgetEditor::UpdateUndoRedoState()
{
	OnUndoRedoStateChanged.Broadcast(CanUndo(), CanRedo());
}
