#include "Model/Model_WidgetEditor.h"
#include "ImWidget/ImWidget.h"
#include "ImWidget/ImPanelWidget.h"
#include "Tools/JLog.h"
#include "Model/Command/CommandManager.h"
#include "ImWidget/ImWidgetFactory.h"
#include <queue>
#include "EditorAction.h"

void Model_WidgetEditor::CollectWidgetNames(ImGuiWidget::ImWidget* widget)
{
	std::queue<ImGuiWidget::ImWidget*> WidgetQueue;
	WidgetQueue.push(widget);
	while (!WidgetQueue.empty())
	{
		ImGuiWidget::ImWidget* Current = WidgetQueue.front();
		ExistedWidgetName.insert(Current->GetWidgetName());
		for (int i = 0; i < Current->GetChildNum(); i++)
		{
			WidgetQueue.push(Current->GetChildAt(i));
		}
		WidgetQueue.pop();
	}
}

void Model_WidgetEditor::ActionInit()
{
	ResetFileAction();
	AddSequentialProcessor(Action::ProjectView::RENAME_FILE, [this](const std::string& OldFullPath, const std::string& NewFullPath)
		{
			if (OldFullPath == EditedFileFullPath)
			{
				EditedFileFullPath = NewFullPath;
				ResetFileAction();
			}
		});
}

void Model_WidgetEditor::ResetFileAction()
{
	for (auto& id : FileActions)
	{
		RemoveProcessor(id);
	}
	FileActions.clear();
	FileActions.push_back(AddValidator(EditedFileFullPath + Action::UIFileView::REQUEST_DELETE_WIDGET, [this](ImGuiWidget::ImWidget* deletedwidget)
		{
			return RemoveChildWidget(deletedwidget);
		}));

	FileActions.push_back(AddValidator(EditedFileFullPath + Action::UIFileView::REQUEST_INSERT_WIDGET, [this](ImGuiWidget::ImWidget* Target, int InsertIndex, const std::string& WidgetRegisterName)
		{
			return InsertChildTo(WidgetRegisterName, Target, InsertIndex);
		}));
}

Model_WidgetEditor::Model_WidgetEditor(ImGuiWidget::ImWidget* rootwidget, const std::string& EditedFileFullPath):
	RootWidget(rootwidget),
	m_EditCommandManager(new EditCommandManager()),
	EditedFileFullPath(EditedFileFullPath)
{
	m_EditCommandManager->OnPropertyEditUnDoRedo.Add([this](ImGuiWidget::ImObject* target, const std::string& propertyname)
		{
			OnPropertyEditUnDoRedo.Broadcast(target, propertyname);
		});

	m_EditCommandManager->OnChildEditUndoRedo.Add([this]()
		{
			OnWidgetTreeChanged.Broadcast();
		});

	CollectWidgetNames(rootwidget);
	ActionInit();
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
		UpdateUndoRedoState();
	}
	return true;
}

bool Model_WidgetEditor::InsertChildTo(ImGuiWidget::ImWidget* child, ImGuiWidget::ImWidget* Target, int InsertIndex)
{
	if (!child || !Target) return false;
	if (m_EditCommandManager->ExecuteChildInsert(Target, child, InsertIndex))
	{
		CollectWidgetNames(child);
		OnWidgetTreeChanged.Broadcast();
		UpdateUndoRedoState();
	}
	return true;
}

bool Model_WidgetEditor::InsertChildTo(const std::string& WidgetRegisterName, ImGuiWidget::ImWidget* Target, int InsertIndex)
{
	ImGuiWidget::ImWidget* NewWidget = ImGuiWidget::ImWidgetFactory::GetInstance().CreateWidget(WidgetRegisterName, "");
	if (NewWidget)
	{
		int i = 0;
		while (ExistedWidgetName.find(WidgetRegisterName + std::to_string(i)) != ExistedWidgetName.end())
		{
			i++;
		}
		const std::string widgetname = WidgetRegisterName + std::to_string(i);
		NewWidget->SetWidgetName(widgetname);
		return InsertChildTo(NewWidget, Target, InsertIndex);
	}
	return false;
}

void Model_WidgetEditor::EditProperty(const ImGuiWidget::PropertyInfo& propInfo, const void* NewValue, ImGuiWidget::ImObject* Target)
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
