#pragma once
#include "ImUserWidget.h"

#include "ImBasicWidgetDeclaration.h"
#include "ImTools/ImDelegate.h"
#include <map>
#include "EditorGlobalInterface.h"


namespace ImGuiWidget
{
    class ImPageManager;
    class ImWindow;
}
class UI_WidgetTreeView;
class UI_WidgetEditor;
class UI_DetailView;
class UI_ProjectView;

class MainUI : public ImGuiWidget::ImUserWidget,public EditorGlobalInterface
{
public:
    void Init();
    void Init2();
    MainUI(const std::string& name): ImGuiWidget::ImUserWidget(name)
{
    Init();
    Init2();
    EventInit();
}

protected:
//----Gen Members Begin----
    // Auto-generated widget pointers
    ImGuiWidget::ImVerticalBox* ImVerticalBox_3;
    ImGuiWidget::ImHorizontalBox* ImHorizontalBox_4;
    ImGuiWidget::ImButton* ImButton_Project;
    ImGuiWidget::ImTextBlock* ImTextBlock_9;
    ImGuiWidget::ImButton* ImButton_Save;
    ImGuiWidget::ImTextBlock* ImTextBlock_10;
    ImGuiWidget::ImButton* ImButton_Generate;
    ImGuiWidget::ImTextBlock* ImTextBlock_11;
    ImGuiWidget::ImButton* ImButton_Undo;
    ImGuiWidget::ImImage* ImImage_Undo;
    ImGuiWidget::ImButton* ImButton_Redo;
    ImGuiWidget::ImImage* ImImage_Redo;
    ImGuiWidget::ImVerticalSplitter* ImVerticalSplitter_6;
    ImGuiWidget::ImHorizontalSplitter* ImHorizontalSplitter_1;
    ImGuiWidget::ImBorder* ImBorder_Left;
    ImGuiWidget::ImVerticalSplitter* ImVerticalSplitter_0;
    ImGuiWidget::ImScrollBox* ImScrollBox_WidgetList;
    ImGuiWidget::ImVerticalBox* ImVerticalBox_WidgetList;
    ImGuiWidget::ImBorder* ImBorder_LeftTab;
    ImGuiWidget::ImBorder* ImBorder_MainWorkSpace;
    ImGuiWidget::ImBorder* ImBorder_Right;
    ImGuiWidget::ImScrollingTextList* ImScrollingTextList_LogList;
//----Gen Members End----
    //---------------------编辑器主工作区-------------------
    ImGuiWidget::ImPageManager* ImPageManager_Main;

    //---------------------项目管理及控件树------------------
    ImGuiWidget::ImPageManager* ImPageManager_LeftPart;

    UI_ProjectView* ProjectView;

    //---------------------控件树视图-----------------------
    ImGuiWidget::ImScrollBox* ImScrollBox_WidgetTree;
    std::map<std::string, UI_WidgetTreeView*> AllTreeViews;
    std::string CurrentTreeView;
    //---------------------细节框-------------------------
    ImGuiWidget::ImScrollBox* ImScrollBox_FileDetail;
    std::map<std::string, UI_DetailView*> AllFileDetails;
    std::string CurrentFileDetail;
    //---------------------弹出菜单------------------
    ImGuiWidget::ImWindow* m_FolderOperatorMenuWindow;
    ImGuiWidget::ImVerticalBox* ImVerticalBox_FolderOperatorMenu;
    ImGuiWidget::ImWindow* m_FileOperatorMenuWindow;
    ImGuiWidget::ImVerticalBox* ImVerticalBox_FileOperatorMenu;
public:
    //ImMulticastDelegate<const std::string&, const std::string&> OnUIFileSelected;
    //ImMulticastDelegate<const std::string&> OnEditorPageClosed;
    //ImMulticastDelegate<const std::string&> OnEditorPageSelected;
    //ImMulticastDelegate<> OnRequestUndo;
    //ImMulticastDelegate<> OnRequestRedo;
protected:
    void EventInit();
    //ProjectView相关操作
public:
    UI_ProjectView* GetProjectView() { return ProjectView; }
    void UpdateProjectView(class ProjectFileManager* projectmananger);
private:
    void On_EditorPageClosed(const std::string& FilePath);
    void On_EditorPageSelected(const std::string& PageID);
    //WidgetEditor相关操作

    void CreateNewWidgetEditorPage(ImGuiWidget::ImWidget* FileRootWidget, const std::string& FileName, const std::string& FileFullPath);
public:
    UI_WidgetEditor* GetWidgetEditorByName(const std::string& Name);
    bool ShowWidgetEditorByName(const std::string& Name);
private:
    bool RenameWidgetEditorPage(const std::string& OldFullPath, const std::string& NewFullPath);

    //WidgetTreeView相关操作
public:
    bool CreateNewWidgetTreeView(const std::string& Name, ImGuiWidget::ImWidget* TargetWidget);
    UI_WidgetTreeView* GetWidgetTreeViewByName(const std::string& Name);
    bool ShowWidgetTreeViewByName(const std::string& Name);
    bool RemoveWidgetTreeViewByName(const std::string& Name);
private:
    bool RenameWidgetTreeView(const std::string& OldName, const std::string& NewName);

    //DetailView相关操作
public:
    bool CreateNewDetailView(const std::string& Name);
    UI_DetailView* GetDetailViewByName(const std::string& Name);
    bool ShowDetailViewByName(const std::string& Name);
    bool RemoveDetailViewByName(const std::string& Name);
private:
    bool RenameDetailView(const std::string& OldName, const std::string& NewName);

public:
    bool HandleRenameFile(const std::string& OldFullPath, const std::string& NewFullPath);
    bool HandleCloseFile(const std::string& FileFullPath);

    //log更新
public:
    void UpdateLog(std::vector<std::string>&& Logs);
    void UpdateUndoRedoState(bool CanUndo, bool CanRedo);
};
