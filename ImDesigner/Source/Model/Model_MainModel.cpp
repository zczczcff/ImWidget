#include "Model/Model_MainModel.h"
#include "ImWidget/ImWidgetCodeGenerator.h"
#include "ImWidget/ImWidgetSerializer.h"
#include "Model/Model_WidgetEditor.h"
#include "Model/FileUtil.h"
#include "Tools/JAsyncLog.h"
#include "Tools/JLog.h"
#include "EditorAction.h"
#include "EditorEvents.h"
#include "ImWidget/ImUserWidgetClass.h"

void Model_MainModel::InitAction()
{
	AddValidator(Action::ProjectView::RENAME_FILE, [this](const std::string& OldFullPath, const std::string& NewFullPath)
		{
			return RenameFile(OldFullPath, NewFullPath);
		});

	AddValidator(Action::ProjectView::UI_FILE_SELECTED, [this](const std::string& FileName, const std::string& FileFullPath) 
		{
			if (EditedUIFile* file = BeginEditFile(FileFullPath))
			{
				Publish(Events::MainUI::UI_FILE_OPENED, file->EditedFile, FileName, FileFullPath);
				return true;
			}
			else
			{
				return false;
			}
		});

	AddValidator(Action::MainUI::EDITOR_PAGE_CLOSED, [this](const std::string& FileFullPath) 
		{
			return FinishEditFile(FileFullPath);
		});

	AddValidator(Action::ProjectView::CREATE_NEW_FILE, [this](const std::string& Dir) 
		{
			std::string NewFilePath = CteateNewUIFileInDir(Dir);
			if (NewFilePath.empty())
			{
				return false;
			}
			else
			{
				Publish(Events::ProjectView::ACTIVATE_FILE_RENAME, NewFilePath, true);
				return true;
			}
		});

}
void Model_MainModel::Tick()
{
	std::vector<std::string> RecentLogs = m_Log->GetRecentLogs();
	if (RecentLogs.size() > 0)
	{
		if (OnLogUpdate)
		{
			OnLogUpdate(std::move(RecentLogs));
		}
		else
		{
			Publish(Events::REGISTER_LOG_UPDATE_FUN, OnLogUpdate);

			if (OnLogUpdate)
			{
				OnLogUpdate(std::move(RecentLogs));
			}
		}
	}
}

void Model_MainModel::Init()
{
	LoadConfig("");
	m_Log = new JAsyncLog("./ImDesignerLog.txt");
	m_Log->SetFlushInterval(std::chrono::milliseconds(200));
	SetLogFun([log = this->m_Log](const std::string& msg){log->Log(msg); });
}

Model_MainModel::EditedUIFile* Model_MainModel::BeginEditFile(const std::string& FileFullPath)
{
	if (EditedFiles.find(FileFullPath) != EditedFiles.end()) return nullptr;
	//ImGuiWidget::ImWidget* NewEditedWidget = ImGuiWidget::LoadWidgetTreeFromFile(FileFullPath);
	ImGuiWidget::ImUserWidgetClass* NewEditedUserWidgetCLassFile = new ImGuiWidget::ImUserWidgetClass("");
	if (NewEditedUserWidgetCLassFile->InitFromFile(FileFullPath))
	{
		EditedUIFile* NewEditedFile = new EditedUIFile(FileFullPath, NewEditedUserWidgetCLassFile, new Model_WidgetEditor(NewEditedUserWidgetCLassFile, FileFullPath));
		EditedFiles.insert(std::make_pair(FileFullPath, NewEditedFile));
		return NewEditedFile;
	}
	else
	{
		delete NewEditedUserWidgetCLassFile;
		return nullptr;
	}
}

bool Model_MainModel::FinishEditFile(const std::string& FileFullPath)
{
	auto it = EditedFiles.find(FileFullPath);
	if (it != EditedFiles.end())
	{
		delete it->second;
		EditedFiles.erase(it);
		return true;
	}
	return false;
}

std::string Model_MainModel::CteateNewUIFileInDir(const std::string& Dir)
{
	std::string NewFile = FileUtil::createUniqueFile(Dir, "NewUI", ".imui");
	if (NewFile.empty()) return "";
	m_ProjectFileManager.rescan();
	Publish(Events::ProjectView::UPDATE_PROJECT_VIEW, &m_ProjectFileManager);
	//OnProjectConfigChanged.Broadcast(&m_ProjectFileManager);
	return Dir + "/" + NewFile;
}

bool Model_MainModel::RenameFile(const std::string& OldFullPath, const std::string& NewFullPath)
{
	if (FileUtil::renameFile(OldFullPath, NewFullPath))
	{
		m_ProjectFileManager.rescan();
		Publish(Events::ProjectView::UPDATE_PROJECT_VIEW, &m_ProjectFileManager);
		//OnProjectConfigChanged.Broadcast(&m_ProjectFileManager);
		return true;
	}
	return false;
}

Model_MainModel::~Model_MainModel()
{
	delete m_Log;
}

void Model_MainModel::LoadConfig(const std::string& ConfigPath)
{
	if (m_ProjectConfig.GetCurrentConfigFilePath() == ConfigPath) return;
	if (ConfigPath.empty())
	{
		m_ProjectConfig = ProjectConfig();
	}
	else
	{
		m_ProjectConfig = ProjectConfig(ConfigPath);
	}
	m_ProjectFileManager = ProjectFileManager(m_ProjectConfig.GetUIFolder(), { ".imui" });
	//OnProjectConfigChanged.Broadcast(&m_ProjectFileManager);
	Publish(Events::ProjectView::UPDATE_PROJECT_VIEW, &m_ProjectFileManager);
}
