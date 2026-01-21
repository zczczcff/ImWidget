#include "Controller/Controller_WidgetEditor.h"

#include "UI/UI_DetailView.h"
#include "UI/UI_WidgetEditor.h"
#include "UI/UI_WidgetTreeView.h"
#include "Model/Model_WidgetEditor.h"

Controller_WidgetEditor::Controller_WidgetEditor(UI_WidgetTreeView* In_UI_WidgetTreeView, UI_WidgetEditor* In_UI_WidgetEditor, UI_DetailView* In_UI_FileDetail, Model_WidgetEditor* In_Model_WidgetEditor)
	:m_UI_WidgetTreeView(In_UI_WidgetTreeView),
	m_UI_WidgetEditor(In_UI_WidgetEditor),
	m_UI_FileDetail(In_UI_FileDetail),
	m_Model_WidgetEditor(In_Model_WidgetEditor)
{
	//m_UI_WidgetEditor->OnWidgetSelected.Add([this](ImGuiWidget::ImWidget* SelectedWidget) 
	//	{
	//		SetSelectedWidget(SelectedWidget);
	//	});

	//m_UI_WidgetTreeView->OnWidgetSelectedButtonClicked.Add([this](ImGuiWidget::ImWidget* SelectedWidget)
	//	{
	//		SetSelectedWidget(SelectedWidget);
	//	});

	//m_UI_WidgetTreeView->OnRequestWidgetDeleted.Add([this](ImGuiWidget::ImWidget* deletedwidget)
	//	{
	//		m_Model_WidgetEditor->RemoveChildWidget(deletedwidget);
	//	});

	m_UI_FileDetail->OnPropertyChanged.Add([this](const ImGuiWidget::PropertyInfo& propInfo, const void* newValue, ImGuiWidget::ImObject* Target)
		{
			m_Model_WidgetEditor->EditProperty(propInfo, newValue, Target);
		});

	m_Model_WidgetEditor->OnWidgetTreeChanged.Add([this]() 
		{
			m_UI_WidgetTreeView->Refresh();
			UpdateUndoRedoState();
		});

	m_Model_WidgetEditor->OnPropertyEditUnDoRedo.Add([this](ImGuiWidget::ImObject* Target,const std::string& PropertyName)
		{
			m_UI_FileDetail->UpdatePropertyDisplay(Target, PropertyName);
		});

	m_UI_WidgetEditor->OnRequestUndo.Add([this]() 
		{
			m_Model_WidgetEditor->Undo();
		});

	m_UI_FileDetail->OnRequestUndo.Add([this]() 
		{
			m_Model_WidgetEditor->Undo();
		});
	m_Model_WidgetEditor->OnUndoRedoStateChanged.Add([this](bool CanUndo,bool CanRedo) 
		{
			OnUndoRedoStateChanged.Broadcast(CanUndo, CanRedo);
		});

	//m_UI_WidgetTreeView->OnRequestInsertWidget.Add([this](ImGuiWidget::ImWidget* Target, int InsertIndex, const std::string& WidgetRegisterName)
	//	{
	//		m_Model_WidgetEditor->InsertChildTo(WidgetRegisterName, Target, InsertIndex);
	//	});

}

//void Controller_WidgetEditor::SetSelectedWidget(ImGuiWidget::ImWidget* SelectedWidget)
//{
//	m_UI_WidgetTreeView->SetSelectedWidget(SelectedWidget);
//	m_UI_WidgetEditor->SetSelectedWidget(SelectedWidget);
//	m_UI_FileDetail->SetCurrentWidget(SelectedWidget);
//}

void Controller_WidgetEditor::RequestUndo()
{
	m_Model_WidgetEditor->Undo();
}

void Controller_WidgetEditor::RequestRedo()
{
	m_Model_WidgetEditor->Redo();
}

void Controller_WidgetEditor::UpdateUndoRedoState()
{
	OnUndoRedoStateChanged.Broadcast(m_Model_WidgetEditor->CanUndo(), m_Model_WidgetEditor->CanRedo());
}
