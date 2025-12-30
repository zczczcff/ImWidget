#include "Controller/Controller_MainController.h"
#include "Model/Model_MainModel.h"
#include "UI/MainUI.h"

 Controller_MainController::Controller_MainController(MainUI* MainUI, Model_MainModel* m_MainModel)
	:
	m_MainUI(MainUI),
	m_MainModel(m_MainModel)
{
	 m_MainModel->OnProjectConfigChanged.Add([this](ProjectFileManager* projectmananger) 
		 {
			 m_MainUI->UpdateProjectView(projectmananger);
		 });
	 m_MainModel->Init();
}
