#pragma once
#include "ImUserWidget.h"

#include "ImBasicWidgetDeclaration.h"
#include "ImTools/ImDelegate.h"

namespace ImGuiWidget
{
    class ImPageManager;
    class ImWindow;
}


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
    ImGuiWidget::ImVerticalBox* ImVerticalBox_WidgetTree;
    //---------------------弹出菜单------------------
    ImGuiWidget::ImWindow* m_RightKeyFunMenuWindow;
    ImGuiWidget::ImVerticalBox* ImVerticalBox_FolderOperatorMenu;

public:
    ImMulticastDelegate<const std::string&, const std::string&> OnUIFileSelected;
    ImMulticastDelegate<const std::string&> OnEditorPageClosed;
private:
    void SetProjectViewVBoxContent(class ProjectFileManager* projectmananger, ImGuiWidget::ImVerticalBox* Vbox,const std::string& CurrentPath);
    void SetupFileButton(ImGuiWidget::ImButton* filebutton);
    void InitDirButton(ImGuiWidget::ImButton* Dirbutton,const std::string& dir);
    void PopupDirRightKeyWindow(const std::string& dir);
    void PopupRightKeyWindow(ImWidget* rootwidget);
public:
    void UpdateProjectView(class ProjectFileManager* projectmananger);
    void On_UIFileButtonClicked(const std::string& FileName, const std::string& FileFullPath);
    void CreateUIEditorPage(ImGuiWidget::ImWidget* FileRootWidget, const std::string& FileName, const std::string& FileFullPath);
    void On_EditorPageClosed(const std::string& FilePath);
};
