#pragma once

#include "Model/Projectconfig.h"
#include "ImTools/ImDelegate.h"
#include "Tools/ProjectFileManager.h"
#include <map>
#include "EditorEventObject.h"
namespace ImGuiWidget
{
	class ImWidget;
	class ImUserWidgetClass;
}
class Model_ImUserWidgetClassEditor;
class JAsyncLog;
class Model_MainModel:public EditorEventObject
{
public:
	struct EditedUIFile
	{
		std::string FileFullPath;
		ImGuiWidget::ImUserWidgetClass* EditedFile;
		Model_ImUserWidgetClassEditor* model_editor;
		EditedUIFile(std::string FileFullPath, ImGuiWidget::ImUserWidgetClass* EditedFile,Model_ImUserWidgetClassEditor* model_editor)
			:FileFullPath(FileFullPath),
			EditedFile(EditedFile),
			model_editor(model_editor)
		{ }
		EditedUIFile():EditedFile(nullptr), model_editor(nullptr){}

		operator bool()
		{
			return (!FileFullPath.empty()) && EditedFile && model_editor;
		}
	};
private:
	ProjectConfig m_ProjectConfig;
	ProjectFileManager m_ProjectFileManager;
	std::map<std::string, EditedUIFile> EditedFiles;
	JAsyncLog* m_Log;
public:
	//ImMulticastDelegate<ProjectFileManager*> OnProjectConfigChanged;
	std::function<void(std::vector<std::string>&&)> OnLogUpdate;
private:
	void InitAction();
public:
	Model_MainModel()
	{
		InitAction();
		Init();
	}
	void Tick();
	void Init();
	EditedUIFile BeginEditFile(const std::string& FileFullPath);
	bool FinishEditFile(const std::string& FileFullPath);
	std::string CteateNewUIFileInDir(const std::string& Dir);
	bool RenameFile(const std::string& OldFullPath, const std::string& NewFullPath);
	~Model_MainModel();
private:
	void LoadConfig(const std::string& ConfigPath);
};