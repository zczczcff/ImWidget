#pragma once

#include "Model/Projectconfig.h"
#include "ImTools/ImDelegate.h"
#include "Tools/ProjectFileManager.h"
#include <map>
namespace ImGuiWidget
{
	class ImWidget;
}

class Model_MainModel
{
private:
	ProjectConfig m_ProjectConfig;
	ProjectFileManager m_ProjectFileManager;
	std::map<std::string, ImGuiWidget::ImWidget*> EditedWidgets;
public:
	ImMulticastDelegate<ProjectFileManager*> OnProjectConfigChanged;
public:
	void Tick();
	void Init();
	ImGuiWidget::ImWidget* BeginEditFile(const std::string& FileFullPath);
	void FinishEditFile(const std::string& FileFullPath);
private:
	void LoadConfig(const std::string& ConfigPath);
};