#include "Controller/Controller_MainController.h"
#include "Model/Model_MainModel.h"
#include "UI/MainUI.h"
#include "UI/UI_WidgetTreeView.h"
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
				 m_MainUI->CreateUIEditorPage(EditedFile->rootwidget, FileName, FileFullPath);
				 m_MainUI->GetWidgetTreeView()->CreateNewTreeView(FileFullPath, EditedFile->rootwidget);
			 }
		 });

	 m_MainUI->OnEditorPageClosed.Add([this](const std::string& FileFullPath)
		 {
			 m_MainModel->FinishEditFile(FileFullPath);
		 });
}
