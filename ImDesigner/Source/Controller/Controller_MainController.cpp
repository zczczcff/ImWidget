#include "Controller/Controller_MainController.h"
#include "Model/Model_MainModel.h"
#include "UI/MainUI.h"

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
			 ImGuiWidget::ImWidget* widget = m_MainModel->BeginEditFile(FileFullPath);
			 m_MainUI->CreateUIEditorPage(widget, FileName, FileFullPath);
		 });

	 m_MainUI->OnEditorPageClosed.Add([this](const std::string& FileFullPath)
		 {
			 m_MainModel->FinishEditFile(FileFullPath);
		 });
}
