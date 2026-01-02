#pragma once

#include "Model/Projectconfig.h"
#include "ImTools/ImDelegate.h"
#include "Tools/ProjectFileManager.h"
#include <map>
namespace ImGuiWidget
{
	class ImWidget;
}
class Model_Editor;
class Model_MainModel
{
public:
	struct EditedUIFile
	{
		std::string FileFullPath;
		ImGuiWidget::ImWidget* rootwidget;
		Model_Editor* model_editor;
		EditedUIFile(std::string FileFullPath,ImGuiWidget::ImWidget* rootwidget,Model_Editor* model_editor)
			:FileFullPath(FileFullPath),
			rootwidget(rootwidget),
			model_editor(model_editor)
		{ }
	};
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