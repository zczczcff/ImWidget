#pragma once

#include "Model/Projectconfig.h"
#include "ImTools/ImDelegate.h"
#include "Tools/ProjectFileManager.h"

class Model_MainModel
{
private:
	ProjectConfig m_ProjectConfig;
	ProjectFileManager m_ProjectFileManager;
public:
	ImMulticastDelegate<ProjectFileManager*> OnProjectConfigChanged;
public:
	void Tick();
	void Init();
private:
	void LoadConfig(const std::string& ConfigPath);
};