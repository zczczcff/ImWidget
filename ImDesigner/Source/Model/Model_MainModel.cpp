#include "Model/Model_MainModel.h"
#include "ImWidget/ImWidgetCodeGenerator.h"
#include "ImWidget/ImWidgetSerializer.h"
#include "Model/Model_WidgetEditor.h"
#include "Model/FileUtil.h"
#include "Tools/JAsyncLog.h"
#include "Tools/JLog.h"
#include "EditorAction.h"
void Model_MainModel::InitAction()
{
	AddValidator(Action::ProjectView::RENAME_FILE, [this](std::string OldFullPath, std::string NewFullPath)
		{
			return RenameFile(OldFullPath, NewFullPath);
		});
}
void Model_MainModel::Tick()
{
	std::vector<std::string> RecentLogs = m_Log->GetRecentLogs();
	if (RecentLogs.size() > 0&&OnLogUpdate)
	{
		OnLogUpdate(std::move(RecentLogs));
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
	ImGuiWidget::ImWidget* NewEditedWidget = ImGuiWidget::LoadWidgetTreeFromFile(FileFullPath);
	if (NewEditedWidget)
	{
		EditedUIFile* NewEditedFile = new EditedUIFile(FileFullPath, NewEditedWidget, new Model_WidgetEditor(NewEditedWidget));
		EditedFiles.insert(std::make_pair(FileFullPath, NewEditedFile));
		return NewEditedFile;
	}
	return nullptr;
}

void Model_MainModel::FinishEditFile(const std::string& FileFullPath)
{
	auto it = EditedFiles.find(FileFullPath);
	if (it != EditedFiles.end())
	{
		delete it->second;
		EditedFiles.erase(it);
	}
}

std::string Model_MainModel::CteateNewUIFileInDir(const std::string& Dir)
{
	std::string NewFile = FileUtil::createUniqueFile(Dir, "NewUI", ".imui");
	if (NewFile.empty()) return "";
	m_ProjectFileManager.rescan();
	OnProjectConfigChanged.Broadcast(&m_ProjectFileManager);
	return Dir + "/" + NewFile;
}

bool Model_MainModel::RenameFile(const std::string& OldFullPath, const std::string& NewFullPath)
{
	if (FileUtil::renameFile(OldFullPath, NewFullPath))
	{
		m_ProjectFileManager.rescan();
		OnProjectConfigChanged.Broadcast(&m_ProjectFileManager);
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
	OnProjectConfigChanged.Broadcast(&m_ProjectFileManager);
}
