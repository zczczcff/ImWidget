#pragma once
#include "ImUserWidget.h"

#include "ImBasicWidgetDeclaration.h"
#include "ImTools/ImDelegate.h"
#include <map>

namespace ImGuiWidget
{
    class ImPageManager;
    class ImWindow;
}
class UI_WidgetTreeView;
class UI_WidgetEditor;
class UI_DetailView;

class MainUI : public ImGuiWidget::ImUserWidget
{
public:
    void Init();
    void Init2();
    MainUI(const std::string& name): ImGuiWidget::ImUserWidget(name)
{
    Init();
    Init2();
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

    ImGuiWidget::ImScrollBox* ImScrollBox_Folder;
    ImGuiWidget::ImVerticalBox* ImVerticalBox_Folder;

    ImGuiWidget::ImScrollBox* ImScrollBox_WidgetTree;
    std::map<std::string, UI_WidgetTreeView*> AllTreeViews;
    std::string CurrentTreeView;
    //---------------------细节框-------------------------
    ImGuiWidget::ImScrollBox* ImScrollBox_FileDetail;
    std::map<std::string, UI_DetailView*> AllFileDetails;
    std::string CurrentFileDetail;
    //---------------------弹出菜单------------------
    ImGuiWidget::ImWindow* m_RightKeyFunMenuWindow;
    ImGuiWidget::ImVerticalBox* ImVerticalBox_FolderOperatorMenu;

public:
    ImMulticastDelegate<const std::string&, const std::string&> OnUIFileSelected;
    ImMulticastDelegate<const std::string&> OnEditorPageClosed;
    ImMulticastDelegate<const std::string&> OnEditorPageSelected;
private:
    void SetProjectViewVBoxContent(class ProjectFileManager* projectmananger, ImGuiWidget::ImVerticalBox* Vbox,const std::string& CurrentPath);
    void SetupFileButton(ImGuiWidget::ImButton* filebutton);
    void InitDirButton(ImGuiWidget::ImButton* Dirbutton,const std::string& dir);
    void PopupDirRightKeyWindow(const std::string& dir);
    void PopupRightKeyWindow(ImWidget* rootwidget);
public:
    void UpdateProjectView(class ProjectFileManager* projectmananger);
private:
    void On_UIFileButtonClicked(const std::string& FileName, const std::string& FileFullPath);
    void On_EditorPageClosed(const std::string& FilePath);
    void On_EditorPageSelected(const std::string& PageID);
    //WidgetEditor相关操作
public:
    void CreateNewWidgetEditorPage(ImGuiWidget::ImWidget* FileRootWidget, const std::string& FileName, const std::string& FileFullPath);
    UI_WidgetEditor* GetWidgetEditorByName(const std::string& Name);

    //WidgetTreeView相关操作
public:
    bool CreateNewWidgetTreeView(const std::string& Name, ImGuiWidget::ImWidget* TargetWidget);
    UI_WidgetTreeView* GetWidgetTreeViewByName(const std::string& Name);
    bool ShowWidgetTreeViewByName(const std::string& Name);
    bool RemoveWidgetTreeViewByName(const std::string& Name);

    //DetailView相关操作
public:
    bool CreateNewDetailView(const std::string& Name);
    UI_DetailView* GetDetailViewByName(const std::string& Name);
    bool ShowDetailViewByName(const std::string& Name);
    bool RemoveDetailViewByName(const std::string& Name);
};
