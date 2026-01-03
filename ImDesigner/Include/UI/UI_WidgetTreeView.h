#pragma once

#include "ImWidget/ImUserWidget.h"
#include "ImWidget/ImBasicWidgetDeclaration.h"
#include "ImTools/ImDelegate.h"
#include <string>
#include <map>


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

private:
    const ImU32 HIGHLIGHT_COLOR = IM_COL32(20, 200, 20, 255); // 高亮颜色
    const ImU32 DEFAULT_COLOR = IM_COL32(0, 102, 204, 255);   // 默认颜色

    TreeViewStruct m_TreeView;  // 单个树视图结构

public:
    ImMulticastDelegate<ImGuiWidget::ImWidget*> OnWidgetDeleteButtonClicked;
    ImMulticastDelegate<ImGuiWidget::ImWidget*> OnWidgetSelectedButtonClicked;

private:
    // 递归构建树节点
    ImGuiWidget::ImWidget* BuildTreeNode(ImGuiWidget::ImWidget* widget, std::unordered_set<ImGuiWidget::ImWidget*>& m_ExpandedNode, int depth = 0);

    void On_WidgetDeleteButtonClicked(ImGuiWidget::ImWidget* widget);
    void On_WidgetSelectedButtonClicked(ImGuiWidget::ImWidget* widget, ImGuiWidget::ImButton* nodeButton);

public:
    UI_WidgetTreeView(const std::string& WidgetName)
        : ImUserWidget(WidgetName)
    {
        SetAllowDragOn(true);
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