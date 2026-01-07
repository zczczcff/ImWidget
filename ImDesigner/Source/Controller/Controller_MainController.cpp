#include "Controller/Controller_MainController.h"
#include "Model/Model_MainModel.h"
#include "UI/MainUI.h"
#include "UI/UI_WidgetTreeView.h"
#include "UI/UI_ProjectView.h"
#include "Controller/Controller_WidgetEditor.h"
 Controller_MainController::Controller_MainController(MainUI* MainUI, Model_MainModel* MainModel)
	:
	m_MainUI(MainUI),
	m_MainModel(MainModel)
{
	 m_MainModel->OnProjectConfigChanged.Add([this](ProjectFileManager* projectmananger) 
		 {
			 m_MainUI->UpdateProjectView(projectmananger);
		 });
	 m_MainModel->Init();

	 m_MainUI->OnUIFileSelected.Add([this](const std::string& FileName, const std::string& FileFullPath)
		 {
			 auto EditedFile = m_MainModel->BeginEditFile(FileFullPath);
			 if (EditedFile)
			 {
				 m_MainUI->CreateNewWidgetEditorPage(EditedFile->rootwidget, FileName, FileFullPath);
				 m_MainUI->CreateNewWidgetTreeView(FileFullPath, EditedFile->rootwidget);
				 m_MainUI->CreateNewDetailView(FileFullPath);
				 m_MainUI->ShowWidgetTreeViewByName(FileFullPath);
				 m_MainUI->ShowDetailViewByName(FileFullPath);
				 UI_WidgetTreeView* WidgetTreeView = m_MainUI->GetWidgetTreeViewByName(FileFullPath);
				 UI_DetailView* DetailView = m_MainUI->GetDetailViewByName(FileFullPath);
				 UI_WidgetEditor* WidgetEditor = m_MainUI->GetWidgetEditorByName(FileFullPath);
				 Controller_WidgetEditor* NewController_WidgetEditor = new Controller_WidgetEditor(WidgetTreeView, WidgetEditor, DetailView, EditedFile->model_editor);
				 WidgetEidtorControllers.insert(std::make_pair(FileFullPath, NewController_WidgetEditor));
			 }
		 });

	 m_MainUI->OnEditorPageClosed.Add([this](const std::string& FileFullPath)
		 {
			 m_MainModel->FinishEditFile(FileFullPath);
			 auto it = WidgetEidtorControllers.find(FileFullPath);
			 if (it == WidgetEidtorControllers.end())
			 {
				 //±¨´í
			 }
			 else
			 {
				 delete it->second;
				 WidgetEidtorControllers.erase(it);
				 m_MainUI->HandleCloseFile(FileFullPath);
			 }
		 });

	 m_MainUI->GetProjectView()->OnRequestCreateFileInDir.Add([this](const std::string& Dir) 
		 {
			 std::string NewFilePath= m_MainModel->CteateNewUIFileInDir(Dir);
			 if (!NewFilePath.empty())
			 {
				 m_MainUI->GetProjectView()->ActivateFileRename(NewFilePath, true);
			 }
		 });
	 m_MainUI->GetProjectView()->OnFileRenamed.Add([this](const std::string& OldFullPath, const std::string& NewFullPath) 
		 {
			 if (m_MainModel->RenameFile(OldFullPath, NewFullPath))
			 {
				 auto it = WidgetEidtorControllers.find(OldFullPath);
				 if (it == WidgetEidtorControllers.end())
				 {
					 //±¨´í
				 }
				 else
				 {
					 Controller_WidgetEditor* v = it->second;
					 WidgetEidtorControllers.erase(it);
					 WidgetEidtorControllers.insert(std::make_pair(NewFullPath, v));
					 m_MainUI->GetProjectView()->ExpandToFile(NewFullPath);
					 m_MainUI->GetProjectView()->ScrollToFileWithDelay(NewFullPath);
					 m_MainUI->HandleRenameFile(OldFullPath, NewFullPath);
				 }
			 }
		 });

	 m_MainModel->OnLogUpdate = [this](std::vector<std::string>&& Logs) { m_MainUI->UpdateLog(std::move(Logs)); };
}
