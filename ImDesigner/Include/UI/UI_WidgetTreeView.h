#pragma once

#include "ImWidget/ImUserWidget.h"
#include "ImWidget/ImBasicWidgetDeclaration.h"
#include "ImTools/ImDelegate.h"
#include <string>
#include <map>
#include "EditorEventObject.h"

namespace ImGuiWidget
{
    class ImWindow;
    class ButtonStateStyle;
}

class UI_WidgetTreeView :public ImGuiWidget::ImUserWidget,public EditorEventObject
{
private:
    struct TreeViewStruct
    {
        ImGuiWidget::ImWidget* TreeViewRoot = nullptr;//视图根控件
        ImGuiWidget::ImWidget* TargetWidget = nullptr;      // 目标控件
        ImGuiWidget::ImWidget* SelectedWidget = nullptr;       // 当前选中的控件
        std::unordered_set<ImGuiWidget::ImWidget*> m_ExpandedNode;
        std::unordered_map<ImGuiWidget::ImWidget*, ImGuiWidget::ImButton*> WidgetToHeaderButton;
        std::unordered_map<ImGuiWidget::ImButton*, ImGuiWidget::ImWidget*> HeaderButtonToWidget;
        ImGuiWidget::ImButton* SelectedHeaderButton = nullptr;
    };
    enum class InsertChildMode
    {
        InsertPrevious,
        InsertToThis,
        InsertNext
    };
private:
    //const ImU32 HIGHLIGHT_COLOR = IM_COL32(20, 200, 20, 255); // 高亮颜色
    //ImU32 DEFAULT_COLOR = IM_COL32(250, 250, 250, 255);   // 默认颜色
    std::string EditedFileFullPath;
    ImGuiWidget::ButtonStateStyle* Highlight_Style = nullptr;
    ImGuiWidget::ButtonStateStyle* Normal_Style = nullptr;
    TreeViewStruct m_TreeView;  // 单个树视图结构

    ImGuiWidget::ImWindow* WidgetMenu = nullptr;//右键菜单弹出窗口
    ImGuiWidget::ImWidget* PopupMenuTargetWidget = nullptr;//右键弹出菜单目标控件
    InsertChildMode m_InsertMode;
    ImGuiWidget::ImVerticalBox* ImVerticalBox_WidgetMenu = nullptr;//右键菜单

    ImGuiWidget::ImWindow* WidgetMenu_InsertNew = nullptr;//二级子菜单窗口
    ImGuiWidget::ImVerticalBox* ImVerticalBox_WidgetMenu_InsertNew = nullptr;//二级子菜单：插入控件

    std::vector<EditorActionID> FileActions;
    std::vector<EditorEventID> FileEvents;
public:
    //ImMulticastDelegate<ImGuiWidget::ImWidget*> OnRequestWidgetDeleted;
    //ImMulticastDelegate<ImGuiWidget::ImWidget*> OnWidgetSelectedButtonClicked;
    //ImMulticastDelegate<ImGuiWidget::ImWidget*, int, const std::string&> OnRequestInsertWidget;
private:
    void ActionInit();
    void ResetAction();
    void ResetEvent();
    void InitPopUpMenu();
    void InitButtonStyle();
    ImGuiWidget::ImButton* CreateWidgetMenuButton(const std::string& Text,bool bHaveSubMenu=false);
    ImGuiWidget::ImButton* CreateWidgetInsertButton(const std::string& CN_Name, const std::string& RegisterName, ImTextureID icon);
    // 递归构建树节点
    ImGuiWidget::ImWidget* BuildTreeNode(ImGuiWidget::ImWidget* widget, std::unordered_set<ImGuiWidget::ImWidget*>& m_ExpandedNode, int depth = 0);
    ImGuiWidget::ImWidget* BuildRootNode(ImGuiWidget::ImWidget* widget, std::unordered_set<ImGuiWidget::ImWidget*>& m_ExpandedNode);
    void On_WidgetDeleteButtonClicked(ImGuiWidget::ImWidget* widget);
    void On_WidgetSelectedButtonClicked(ImGuiWidget::ImWidget* widget, ImGuiWidget::ImButton* nodeButton);
    void On_WidgetSelectedButtonRightClicked(ImGuiWidget::ImWidget* widget);
    void On_InsertWidgetButtonClicked(const std::string& InsertWidgetRegisterName);
public:
    UI_WidgetTreeView(const std::string& WidgetName, const std::string& EditedFileFullPath)
        : ImUserWidget(WidgetName),
        EditedFileFullPath(EditedFileFullPath)
    {
        SetAllowDragOn(true);
        InitPopUpMenu();
        InitButtonStyle();
        ActionInit();
    }

    // 设置目标控件树
    void SetTargetWidget(ImGuiWidget::ImWidget* Target);

    //外部设置选中控件
    void SetSelectedWidget(ImGuiWidget::ImWidget* widget);

    // 获取当前选中的控件
    ImGuiWidget::ImWidget* GetSelectedWidget();

    // 获取目标控件
    ImGuiWidget::ImWidget* GetTargetWidget() { return m_TreeView.TargetWidget; }

    // 刷新树视图
    void Refresh();

    // 清空树视图
    void Clear();
};