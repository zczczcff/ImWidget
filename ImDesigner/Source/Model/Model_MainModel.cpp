#include "Model/Model_MainModel.h"
#include "ImWidget/ImWidgetCodeGenerator.h"
#include "ImWidget/ImWidgetSerializer.h"

void Model_MainModel::Tick() 
{

}

void Model_MainModel::Init()
{
	LoadConfig("");
}

ImGuiWidget::ImWidget* Model_MainModel::BeginEditFile(const std::string& FileFullPath)
{
	if (EditedWidgets.find(FileFullPath) != EditedWidgets.end()) return nullptr;
	ImGuiWidget::ImWidget* NewEditedWidget = ImGuiWidget::LoadWidgetTreeFromFile(FileFullPath);
	if (NewEditedWidget)
	{
		EditedWidgets.insert(std::make_pair(FileFullPath, NewEditedWidget));
	}
	return NewEditedWidget;
}

void Model_MainModel::FinishEditFile(const std::string& FileFullPath)
{
	auto it = EditedWidgets.find(FileFullPath);
	if (it != EditedWidgets.end())
	{
		delete it->second;
		EditedWidgets.erase(it);
	}
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
