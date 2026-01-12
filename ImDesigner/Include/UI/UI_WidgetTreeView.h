#pragma once

#include "ImWidget/ImUserWidget.h"
#include "ImWidget/ImBasicWidgetDeclaration.h"
#include "ImTools/ImDelegate.h"
#include "ImWidget/ImUserWidgetClass.h"
#include <string>
#include <map>

namespace ImGuiWidget
{
    class ImWindow;
    class ButtonStateStyle;
}

class UI_WidgetTreeView :public ImGuiWidget::ImUserWidget
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

    union VarView_union
    {
        void* Void_p;
        TreeViewStruct* widgettreeview;
        ImGuiWidget::ImWidget* objectview;
        ImGuiWidget::ImWidget* propertyview;
    };
    ImGuiWidget::ImUserWidgetClass* TargetClass = nullptr;
    ImGuiWidget::ImVerticalBox* RootVBox = nullptr;
private:
    //const ImU32 HIGHLIGHT_COLOR = IM_COL32(20, 200, 20, 255); // 高亮颜色
    //ImU32 DEFAULT_COLOR = IM_COL32(250, 250, 250, 255);   // 默认颜色
    ImGuiWidget::ButtonStateStyle* Highlight_Style = nullptr;
    ImGuiWidget::ButtonStateStyle* Normal_Style = nullptr;
    //TreeViewStruct m_TreeView;  // 单个树视图结构
    TreeViewStruct* CurrentSelectedView = nullptr;
    std::map<std::string, VarView_union> AllViewMap;

    ImGuiWidget::ImWindow* WidgetMenu = nullptr;//右键菜单弹出窗口
    ImGuiWidget::ImWidget* PopupMenuTargetWidget = nullptr;//右键弹出菜单目标控件
    InsertChildMode m_InsertMode;
    ImGuiWidget::ImVerticalBox* ImVerticalBox_WidgetMenu = nullptr;//右键菜单

    ImGuiWidget::ImWindow* WidgetMenu_InsertNew = nullptr;//二级子菜单窗口
    ImGuiWidget::ImVerticalBox* ImVerticalBox_WidgetMenu_InsertNew = nullptr;//二级子菜单：插入控件
public:
    ImMulticastDelegate<ImGuiWidget::ImWidget*> OnRequestWidgetDeleted;
    ImMulticastDelegate<ImGuiWidget::ImWidget*> OnWidgetSelectedButtonClicked;
    ImMulticastDelegate<ImGuiWidget::ImWidget*, int, const std::string&> OnRequestInsertWidget;
private:
    void InitPopUpMenu();
    void InitButtonStyle();
    ImGuiWidget::ImButton* CreateWidgetMenuButton(const std::string& Text,bool bHaveSubMenu=false);
    ImGuiWidget::ImButton* CreateWidgetInsertButton(const std::string& CN_Name, const std::string& RegisterName, ImTextureID icon);
    ImGuiWidget::ImButton* CreateClassHeaderButon(const std::string& ClassName);
    //
    ImGuiWidget::ImWidget* ReBuildClassView(ImGuiWidget::ImUserWidgetClass* userWidgetClass);
    // 递归构建树节点
    ImGuiWidget::ImWidget* BuildTreeNode(ImGuiWidget::ImWidget* widget, std::unordered_set<ImGuiWidget::ImWidget*>& m_ExpandedNode, TreeViewStruct* m_TreeView, int depth = 0);
    ImGuiWidget::ImWidget* BuildWidgetTreeView(ImGuiWidget::ImWidget* widget, std::unordered_set<ImGuiWidget::ImWidget*>& m_ExpandedNode, TreeViewStruct* m_TreeView);

    ImGuiWidget::ImWidget* BuildStructView(const std::string& varName, ImGuiWidget::ImObject* object);


    void On_WidgetDeleteButtonClicked(ImGuiWidget::ImWidget* widget);
    void On_WidgetSelectedButtonClicked(ImGuiWidget::ImWidget* widget, ImGuiWidget::ImButton* nodeButton, TreeViewStruct* m_TreeView);
    void On_WidgetSelectedButtonRightClicked(ImGuiWidget::ImWidget* widget);
    void On_InsertWidgetButtonClicked(const std::string& InsertWidgetRegisterName);
public:
    UI_WidgetTreeView(const std::string& WidgetName)
        : ImUserWidget(WidgetName)
    {
        SetAllowDragOn(true);
        InitPopUpMenu();
        InitButtonStyle();
    }

    // 设置目标控件树
    //void SetTargetWidget(ImGuiWidget::ImWidget* Target);

    //获取控件所在的TreeView
    TreeViewStruct* GetWidgetTreeView(ImGuiWidget::ImWidget* widget);

    //外部设置选中控件
    void SetSelectedWidget(ImGuiWidget::ImWidget* widget);

    // 获取当前选中的控件
    ImGuiWidget::ImWidget* GetSelectedWidget();

    // 获取目标控件
    //ImGuiWidget::ImWidget* GetTargetWidget() { return m_TreeView.TargetWidget; }

    // 刷新树视图
    void Refresh(const std::string& varName);

    void RefreshTree(TreeViewStruct& tree);

    // 清空树视图
    //void Clear();
};