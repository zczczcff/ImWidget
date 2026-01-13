#pragma once
#include <vector>
#include <string>
#include "ImWidget/ImUserWidgetClass.h"
#include "Model_WidgetEditor.h"
class Model_UIFile
{
private:
	ImGuiWidget::ImUserWidgetClass* Target = nullptr;
	std::string Path;
	class EditCommandManager* m_EditCommandManager;
private:
	std::unordered_set<std::string> ExistedVarName;
	std::map<std::string, Model_WidgetEditor*> AllWidgetEditors;
public:
	ImMulticastDelegate<ImGuiWidget::ImObject*, const std::string&> OnPropertyEditUnDoRedo;
	ImMulticastDelegate<> OnWidgetTreeChanged;
	ImMulticastDelegate<const std::string&, const std::string&> OnVarRename;
	ImMulticastDelegate<bool, bool> OnUndoRedoStateChanged;
public:
	Model_UIFile(const std::string& Path);
	void Undo();
	void Redo();
	void UpdateUndoRedoState();
	bool RemoveChildWidget(const std::string& varName,ImGuiWidget::ImWidget* WidgetToRemove);
	bool InsertChildTo(const std::string& varName,ImGuiWidget::ImWidget* child, ImGuiWidget::ImWidget* Target, int InsertIndex);
	bool InsertChildTo(const std::string& varName,const std::string& WidgetRegisterName, ImGuiWidget::ImWidget* Target, int InsertIndex);
	bool EditProperty(const std::string& varName,const ImGuiWidget::PropertyInfo& propInfo, const void* NewValue, ImGuiWidget::ImObject* Target);
	bool RenameVar(const std::string& OldName, const std::string& NewName);
	bool AddWidgetVar(const std::string& varName, const std::string& RootWidgetRegisterName);
private:
	bool CreateWidgetVar(const std::string& RootWidgetRegisterType);
};