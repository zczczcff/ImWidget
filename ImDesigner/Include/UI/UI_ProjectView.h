#pragma once
#include "ImWidget/ImUserWidget.h"
#include "ImWidget/ImBasicWidgetDeclaration.h"
#include "ImTools/ImDelegate.h"
#include <map>
#include <unordered_map>
#include "EditorEventObject.h"

namespace ImGuiWidget
{
    class ImWindow;
}
class UI_ProjectView : public ImGuiWidget::ImUserWidget,public EditorEventObject
{
private:
    ImGuiWidget::ImScrollBox* ImScrollBox_Folder;
    ImGuiWidget::ImVerticalBox* ImVerticalBox_Folder;

    std::unordered_map<std::string, ImGuiWidget::ImHorizontalBox*> FileFullPathToFileBodyHBox;
    //ImGuiWidget::ImInputText* ImInputText_Rename;
    // 弹出菜单
    ImGuiWidget::ImWindow* m_FolderOperatorMenuWindow;
    ImGuiWidget::ImVerticalBox* ImVerticalBox_FolderOperatorMenu;
    ImGuiWidget::ImWindow* m_FileOperatorMenuWindow;
    ImGuiWidget::ImVerticalBox* ImVerticalBox_FileOperatorMenu;
    std::string CurrentOperatedDirPath;
public:
    //ImMulticastDelegate<const std::string&, const std::string&> OnUIFileSelected;
    //ImMulticastDelegate <const std::string&> OnRequestCreateFileInDir;
    //ImMulticastDelegate<const std::string&, const std::string&> OnFileRenamed;
public:
    UI_ProjectView(const std::string& name);
    

private:
    void Init();
    void InitEvents();
    void InitAction();
    void InitPopUpMenu();
    ImGuiWidget::ImButton* CreateWidgetMenuButton(const std::string& Text, ImGuiWidget::ImWindow* BindPopupWindow);
    void SetProjectViewVBoxContent(class ProjectFileManager* projectmananger, ImGuiWidget::ImVerticalBox* Vbox, const std::string& CurrentPath);
    void SetupButtonStyle(ImGuiWidget::ImButton* filebutton);
    void InitDirButton(ImGuiWidget::ImButton* Dirbutton, const std::string& dir);
    void PopupDirRightKeyWindow();
    //void PopupRightKeyWindow(ImGuiWidget::ImWidget* rootwidget);

    // 事件处理函数
    void On_UIFileButtonClicked(const std::string& FileName, const std::string& FileFullPath);
    void On_CommitFileRename(const std::string& OldFullPath, const std::string& NewFullPath);

    void ExpandToFile(const std::string& FileFullPath);
    void UpdateProjectView(class ProjectFileManager* projectmananger);
	void ActivateFileRename(const std::string& FileFullPath, bool ScrollToTarget = false);
    void ScrollToFileWithDelay(const std::string& FileFullPath);
};