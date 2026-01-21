#include "Controller/Controller_MainController.h"
#include "Model/Model_MainModel.h"
#include "UI/MainUI.h"
#include "UI/UI_WidgetTreeView.h"
#include "UI/UI_ProjectView.h"
#include "Controller/Controller_WidgetEditor.h"
#include "Tools/JLog.h"
#include "EditorAction.h"

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

	 AddSequentialProcessor(Action::MainUI::UI_FILE_SELECTED, [this](std::string FileName, std::string FileFullPath)
		 {
			 auto EditedFile = m_MainModel->BeginEditFile(FileFullPath);
			 if (EditedFile)
			 {
				 //m_MainUI->CreateNewWidgetEditorPage(EditedFile->rootwidget, FileName, FileFullPath);
				 Publish("MainUI_CreateNewWidgetEditorPage", EditedFile->rootwidget, FileName, FileFullPath);
				 m_MainUI->CreateNewWidgetTreeView(FileFullPath, EditedFile->rootwidget);
				 m_MainUI->CreateNewDetailView(FileFullPath);

				 UI_WidgetTreeView* WidgetTreeView = m_MainUI->GetWidgetTreeViewByName(FileFullPath);
				 UI_DetailView* DetailView = m_MainUI->GetDetailViewByName(FileFullPath);
				 UI_WidgetEditor* WidgetEditor = m_MainUI->GetWidgetEditorByName(FileFullPath);
				 Controller_WidgetEditor* NewController_WidgetEditor = new Controller_WidgetEditor(WidgetTreeView, WidgetEditor, DetailView, EditedFile->model_editor);
				 WidgetEidtorControllers.insert(std::make_pair(FileFullPath, NewController_WidgetEditor));
				 SwitchEditPage(FileFullPath);
			 }
		 });

	 //m_MainUI->OnUIFileSelected.Add([this](const std::string& FileName, const std::string& FileFullPath)
		// {
		//	 auto EditedFile = m_MainModel->BeginEditFile(FileFullPath);
		//	 if (EditedFile)
		//	 {
		//		 //m_MainUI->CreateNewWidgetEditorPage(EditedFile->rootwidget, FileName, FileFullPath);
		//		 m_EditorEventbus->Publish("MainUI_CreateNewWidgetEditorPage", EditedFile->rootwidget, FileName, FileFullPath);
		//		 m_MainUI->CreateNewWidgetTreeView(FileFullPath, EditedFile->rootwidget);
		//		 m_MainUI->CreateNewDetailView(FileFullPath);

		//		 UI_WidgetTreeView* WidgetTreeView = m_MainUI->GetWidgetTreeViewByName(FileFullPath);
		//		 UI_DetailView* DetailView = m_MainUI->GetDetailViewByName(FileFullPath);
		//		 UI_WidgetEditor* WidgetEditor = m_MainUI->GetWidgetEditorByName(FileFullPath);
		//		 Controller_WidgetEditor* NewController_WidgetEditor = new Controller_WidgetEditor(WidgetTreeView, WidgetEditor, DetailView, EditedFile->model_editor);
		//		 WidgetEidtorControllers.insert(std::make_pair(FileFullPath, NewController_WidgetEditor));
		//		 SwitchEditPage(FileFullPath);
		//	 }
		// });
	 AddSequentialProcessor(Action::MainUI::EDITOR_PAGE_CLOSED, [this](std::string FileFullPath)
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
	 //m_MainUI->OnEditorPageClosed.Add([this](const std::string& FileFullPath)
		// {
		//	 m_MainModel->FinishEditFile(FileFullPath);
		//	 auto it = WidgetEidtorControllers.find(FileFullPath);
		//	 if (it == WidgetEidtorControllers.end())
		//	 {
		//		 //±¨´í
		//	 }
		//	 else
		//	 {
		//		 delete it->second;
		//		 WidgetEidtorControllers.erase(it);
		//		 m_MainUI->HandleCloseFile(FileFullPath);
		//	 }
		// });

	 AddSequentialProcessor(Action::MainUI::EDITOR_PAGE_SELECTED, [this](std::string NewPageID)
		 {
			 SwitchEditPage(NewPageID);
		 });

	 //m_MainUI->OnEditorPageSelected.Add([this](const std::string& NewPageID) 
		// {
		//	 SwitchEditPage(NewPageID);
		// });

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
					 if (CurrentFile == OldFullPath)
					 {
						 CurrentFile = NewFullPath;
					 }
				 }
			 }
		 });

	 m_MainModel->OnLogUpdate = [this](std::vector<std::string>&& Logs) { m_MainUI->UpdateLog(std::move(Logs)); };
}

 void Controller_MainController::SwitchEditPage(const std::string& PageName)
 {
	 auto it = WidgetEidtorControllers.find(CurrentFile);
	 if (it != WidgetEidtorControllers.end())
	 {
		 it->second->OnUndoRedoStateChanged.Clear();
		 //m_MainUI->OnRequestRedo.Clear();
		 //m_MainUI->OnRequestUndo.Clear();
	 }

	 auto newit = WidgetEidtorControllers.find(PageName);
	 if (newit != WidgetEidtorControllers.end())
	 {
		 m_MainUI->ShowWidgetTreeViewByName(PageName);
		 m_MainUI->ShowDetailViewByName(PageName);
		 m_MainUI->ShowWidgetEditorByName(PageName);
		 newit->second->OnUndoRedoStateChanged.Add([this](bool CanUndo, bool CanRedo)
			 {
				 m_MainUI->UpdateUndoRedoState(CanUndo, CanRedo);
			 });

		 //m_MainUI->OnRequestRedo.Add([this, WidgetEditor = newit->second]()
		 //{
			// WidgetEditor->RequestRedo();
		 //});

		 //m_MainUI->OnRequestUndo.Add([this, WidgetEditor = newit->second]()
		 //{
			// WidgetEditor->RequestUndo();
		 //});
		 newit->second->UpdateUndoRedoState();
		 CurrentFile = PageName;
	 }	 
 }
