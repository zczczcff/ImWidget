#include "Model/Model_MainModel.h"
#include "ImWidget/ImWidgetCodeGenerator.h"
#include "ImWidget/ImWidgetSerializer.h"
#include "Model/Model_WidgetEditor.h"
#include "Model/FileUtil.h"
void Model_MainModel::Tick() 
{

}

void Model_MainModel::Init()
{
	LoadConfig("");
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
	return NewFile;
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
