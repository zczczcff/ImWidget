#pragma once
#include <map>
#include <string>
#include "ImWidget/ImBasicWidgetDeclaration.h"
#include "ImTools/ImDelegate.h"
#include "ImWidget/ImObject.h"
#include "EditorGlobalInterface.h"
class Model_WidgetEditor:public EditorGlobalInterface
{
private:
	std::string EditedFileFullPath;
	ImGuiWidget::ImWidget* RootWidget;
	std::unordered_set<std::string> ExistedWidgetName;
	class EditCommandManager* m_EditCommandManager;
	std::vector<ActionID> FileActions;//和文件相关的Action
public:
	//ImMulticastDelegate<> OnWidgetTreeChanged;
	//ImMulticastDelegate<ImGuiWidget::ImObject*, const std::string&> OnPropertyEditUnDoRedo;
	//ImMulticastDelegate<> OnChildEditUndoRedo;
	//ImMulticastDelegate<bool, bool> OnUndoRedoStateChanged;

	void CollectWidgetNames(ImGuiWidget::ImWidget* widget);
private:
	void ActionInit();
	void ResetFileAction();
public:
	Model_WidgetEditor(ImGuiWidget::ImWidget* rootwidget,const std::string& EditedFileFullPath);
	ImGuiWidget::ImWidget* GetRootWidget() { return RootWidget; }
	bool RemoveChildWidget(ImGuiWidget::ImWidget* WidgetToRemove);
	bool InsertChildTo(ImGuiWidget::ImWidget* child, ImGuiWidget::ImWidget* Target, int InsertIndex);
	bool InsertChildTo(const std::string& WidgetRegisterName, ImGuiWidget::ImWidget* Target, int InsertIndex);
	bool EditProperty(const ImGuiWidget::PropertyInfo& propInfo, const void* NewValue, ImGuiWidget::ImObject* Target);
	bool Undo();
	bool Redo();
	bool CanUndo();
	bool CanRedo();
private:
	void UpdateUndoRedoState();
public:
};