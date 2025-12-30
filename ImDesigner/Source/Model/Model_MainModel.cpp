#include "Model/Model_MainModel.h"

void Model_MainModel::Tick() 
{

}

void Model_MainModel::Init()
{
	LoadConfig("");
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
