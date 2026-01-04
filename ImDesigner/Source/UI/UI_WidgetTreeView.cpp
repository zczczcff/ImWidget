#include "UI/UI_WidgetTreeView.h"
#include "ImWidget/ImBasicWidgetList.h"
#include "ImGlobal.h"

void UI_WidgetTreeView::InitPopUpMenu()
{
    auto windowmanager = ImGuiWidget::GetGlobalApp()->GetWindowManager();
    ImVerticalBox_WidgetMenu = new ImGuiWidget::ImVerticalBox("ImVerticalBox_WidgetMenu");
    ImGuiWidget::ImButton* CopyButton = CreateWidgetMenuButton(u8"复制");
    ImGuiWidget::ImButton* DeleteButton = CreateWidgetMenuButton(u8"删除");
    DeleteButton->OnLeftClicked.Add([this]() { On_WidgetDeleteButtonClicked(PopupMenuTargetWidget); });
    ImVerticalBox_WidgetMenu->AddChildToVerticalBox(CopyButton)->SetIfAutoSize(false);
    ImVerticalBox_WidgetMenu->AddChildToVerticalBox(DeleteButton)->SetIfAutoSize(false);

    WidgetMenu = windowmanager->CreatePopupWindow(ImVerticalBox_WidgetMenu->GetMinSize(), ImVec2(0, 0), ImVerticalBox_WidgetMenu, false);
    WidgetMenu->Close();
}

ImGuiWidget::ImButton* UI_WidgetTreeView::CreateWidgetMenuButton(const std::string& Text)
{
    ImGuiWidget::ImButton* button = new ImGuiWidget::ImButton("UI_WidgetTreeView_WidgetMenuButton");
    ImGuiWidget::ImTextBlock* text = new ImGuiWidget::ImTextBlock("UI_WidgetTreeView_WidgetMenuButton_Text");
    text->SetText(Text);
    text->SetHorizontalAlignment(ImGuiWidget::ImTextBlock::TextAlignment_Horizontal::Left);
    button->SetContent(text);
    button->GetContentSlot()->SetPadding(2, 2, 10, 10);

    button->bHaveBorder = false;
    button->OnLeftClicked.Add([this]() { WidgetMenu->Close(); });
    return button;
}

ImGuiWidget::ImWidget* UI_WidgetTreeView::BuildTreeNode(ImWidget* nodewidget, std::unordered_set<ImWidget*>& m_ExpandedNode, int depth)
{
    // 检查是否为PanelWidget
    ImGuiWidget::ImPanelWidget* panelWidget = dynamic_cast<ImGuiWidget::ImPanelWidget*>(nodewidget);

    if (panelWidget)
    {
        // 创建可展开盒子
        ImGuiWidget::ImExpandableBox* expandableBox = new ImGuiWidget::ImExpandableBox(nodewidget->GetWidgetName() + "_TreeNode");
        expandableBox->bHaveBorder = false;
        // 创建头部按钮（可选中）
        ImGuiWidget::ImButton* headerButton = new ImGuiWidget::ImButton(nodewidget->GetWidgetName() + "_HeaderBtn");
        ImGuiWidget::ImButton* deletebutton = new ImGuiWidget::ImButton(nodewidget->GetWidgetName() + "_deleteBtn");

        // 设置按钮文本和样式
        ImGuiWidget::ImTextBlock* buttonText = new ImGuiWidget::ImTextBlock(nodewidget->GetWidgetName() + "_HeaderTxt");
        ImGuiWidget::ImTextBlock* deletebuttonText = new ImGuiWidget::ImTextBlock(nodewidget->GetWidgetName() + "_deleteTxt");
        buttonText->SetText(nodewidget->GetWidgetName());
        deletebuttonText->SetText("X");
        headerButton->SetContent(buttonText);
        deletebutton->SetContent(deletebuttonText);
        headerButton->GetNormalStyle().BackgroundColor = DEFAULT_COLOR;
        // 点击头部按钮选中控件
        headerButton->SetOnPressed([this, nodewidget, headerButton]() { On_WidgetSelectedButtonClicked(nodewidget, headerButton); });
        headerButton->OnRightClicked.Add([this, nodewidget]() { On_WidgetSelectedButtonRightClicked(nodewidget); });
        deletebutton->SetOnPressed([this, nodewidget]() { On_WidgetDeleteButtonClicked(nodewidget); });
        m_TreeView.WidgetToHeaderButton[nodewidget] = headerButton;
        m_TreeView.HeaderButtonToWidget[headerButton] = nodewidget;
        ImGuiWidget::ImHorizontalBox* headerbox = new ImGuiWidget::ImHorizontalBox(nodewidget->GetWidgetName() + "_HeaderBox");
        headerbox->AddChildToHorizontalBox(headerButton)->SetIfAutoSize(true);
        headerbox->AddChildToHorizontalBox(deletebutton)->SetIfAutoSize(false);
        expandableBox->SetHead(headerbox);

        // 创建垂直容器存放子节点
        ImGuiWidget::ImVerticalBox* childContainer = new ImGuiWidget::ImVerticalBox(nodewidget->GetWidgetName() + "_ChildContainer");
        expandableBox->SetBody(childContainer);

        // 递归添加子节点
        int slotCount = panelWidget->GetSlotNum();
        for (int i = 0; i < slotCount; ++i)
        {
            ImGuiWidget::ImSlot* slot = panelWidget->GetSlotAt(i);
            if (slot && slot->GetContent())
            {
                ImWidget* childNode = BuildTreeNode(slot->GetContent(), m_ExpandedNode, depth + 1);
                if (childNode)
                {
                    childContainer->AddChildToVerticalBox(childNode)->SetIfAutoSize(false);
                }
            }
        }

        if (m_ExpandedNode.find(panelWidget) != m_ExpandedNode.end())
        {
            expandableBox->SetExpandedState(true);
        }

        expandableBox->SetOnExpandedStateChanged([this, panelWidget, &m_ExpandedNode](bool newstate) mutable
            {
                if (newstate)
                {
                    m_ExpandedNode.insert(panelWidget);
                }
                else
                {
                    m_ExpandedNode.erase(panelWidget);
                }
            });
        return expandableBox;
    }
    else
    {
        // 普通控件 - 创建选择按钮
        ImGuiWidget::ImButton* nodeButton = new ImGuiWidget::ImButton(nodewidget->GetWidgetName() + "_NodeBtn");
        ImGuiWidget::ImButton* deletebutton = new ImGuiWidget::ImButton(nodewidget->GetWidgetName() + "_deleteBtn");
        ImGuiWidget::ImTextBlock* deletebuttonText = new ImGuiWidget::ImTextBlock(nodewidget->GetWidgetName() + "_deleteTxt");
        deletebuttonText->SetText("X");
        deletebutton->SetContent(deletebuttonText);

        ImGuiWidget::ImTextBlock* buttonText = new ImGuiWidget::ImTextBlock(nodewidget->GetWidgetName() + "_NodeTxt");
        buttonText->SetHorizontalAlignment(ImGuiWidget::ImTextBlock::TextAlignment_Horizontal::Left);
        buttonText->SetText(nodewidget->GetWidgetName());
        nodeButton->SetContent(buttonText);
        nodeButton->GetNormalStyle().BackgroundColor = DEFAULT_COLOR;
        // 点击按钮选中控件
        nodeButton->SetOnPressed([this, nodewidget, nodeButton]() { On_WidgetSelectedButtonClicked(nodewidget, nodeButton); });
        nodeButton->OnRightClicked.Add([this, nodewidget]() { On_WidgetSelectedButtonRightClicked(nodewidget); });
        deletebutton->OnLeftClicked.Add([this, nodewidget]() { On_WidgetDeleteButtonClicked(nodewidget); });
        m_TreeView.WidgetToHeaderButton[nodewidget] = nodeButton;
        m_TreeView.HeaderButtonToWidget[nodeButton] = nodewidget;

        ImGuiWidget::ImHorizontalBox* NodeBox = new ImGuiWidget::ImHorizontalBox(nodewidget->GetWidgetName() + "_NodeBox");
        NodeBox->AddChildToHorizontalBox(nodeButton)->SetIfAutoSize(true);
        NodeBox->AddChildToHorizontalBox(deletebutton)->SetIfAutoSize(false);
        return NodeBox;
    }
}

void UI_WidgetTreeView::On_WidgetDeleteButtonClicked(ImGuiWidget::ImWidget* widget)
{
    OnWidgetDeleted.Broadcast(widget);
}

void UI_WidgetTreeView::On_WidgetSelectedButtonClicked(ImGuiWidget::ImWidget* widget, ImGuiWidget::ImButton* nodeButton)
{
    if (m_TreeView.SelectedHeaderButton)
    {
        m_TreeView.SelectedHeaderButton->GetNormalStyle().BackgroundColor = DEFAULT_COLOR;
    }
    m_TreeView.SelectedHeaderButton = nodeButton;
    m_TreeView.SelectedWidget = widget;
    nodeButton->GetNormalStyle().BackgroundColor = HIGHLIGHT_COLOR;
    OnWidgetSelectedButtonClicked.Broadcast(widget);
}

void UI_WidgetTreeView::On_WidgetSelectedButtonRightClicked(ImGuiWidget::ImWidget* widget)
{
    PopupMenuTargetWidget = widget;
    WidgetMenu->SetPosition(ImGuiWidget::GetMousePos());
    WidgetMenu->SetActive();
}

// 设置目标控件树
void UI_WidgetTreeView::SetTargetWidget(ImWidget* Target)
{
    if (!Target) return;

    // 清空现有树视图
    Clear();

    m_TreeView.TargetWidget = Target;
    m_TreeView.TreeViewRoot = BuildTreeNode(Target, m_TreeView.m_ExpandedNode);
    SetRootWidget(m_TreeView.TreeViewRoot, false);
}

//外部设置选中控件
void UI_WidgetTreeView::SetSelectedWidget(ImWidget* widget)
{
    if (!widget) return;
    if (widget == m_TreeView.SelectedWidget) return;
    if (!m_TreeView.TargetWidget) return;

    if (!widget->IsInTree(m_TreeView.TargetWidget)) return; //目标不在当前树中

    m_TreeView.SelectedWidget = widget;
    if (m_TreeView.SelectedHeaderButton)
    {
        m_TreeView.SelectedHeaderButton->GetNormalStyle().BackgroundColor = DEFAULT_COLOR;
    }

    auto it = m_TreeView.WidgetToHeaderButton.find(widget);
    if (it != m_TreeView.WidgetToHeaderButton.end())
    {
        it->second->GetNormalStyle().BackgroundColor = HIGHLIGHT_COLOR;
        m_TreeView.SelectedHeaderButton = it->second;

        ImGuiWidget::ImButton* HeaderButton = it->second;
        ImWidget* CurrentNode = HeaderButton;
        while (CurrentNode != this && CurrentNode != nullptr)
        {
            if (ImGuiWidget::ImButton* NextHeadButton = dynamic_cast<ImGuiWidget::ImButton*>(CurrentNode))
            {
                HeaderButton = NextHeadButton;
            }
            if (ImGuiWidget::ImExpandableBox* expandablebox = dynamic_cast<ImGuiWidget::ImExpandableBox*>(CurrentNode))
            {
                expandablebox->SetExpandedState(true);
                auto widgetIt = m_TreeView.HeaderButtonToWidget.find(HeaderButton);
                if (widgetIt != m_TreeView.HeaderButtonToWidget.end())
                {
                    m_TreeView.m_ExpandedNode.insert(widgetIt->second);
                }
            }
            CurrentNode = CurrentNode->GetParents();
        }
    }
}

// 获取当前选中的控件
ImGuiWidget::ImWidget* UI_WidgetTreeView::GetSelectedWidget()
{
    return m_TreeView.SelectedWidget;
}

// 刷新树视图
void UI_WidgetTreeView::Refresh()
{
    if (m_TreeView.TargetWidget)
    {
        m_TreeView.HeaderButtonToWidget.clear();
        m_TreeView.WidgetToHeaderButton.clear();
        m_TreeView.TreeViewRoot = BuildTreeNode(m_TreeView.TargetWidget, m_TreeView.m_ExpandedNode);
        SetRootWidget(m_TreeView.TreeViewRoot, true); //重建了根，要删除旧的
        SetSelectedWidget(m_TreeView.SelectedWidget);
    }
}

// 清空树视图
void UI_WidgetTreeView::Clear()
{
    // 清空映射关系
    m_TreeView.HeaderButtonToWidget.clear();
    m_TreeView.WidgetToHeaderButton.clear();
    m_TreeView.m_ExpandedNode.clear();

    // 重置状态
    m_TreeView.SelectedWidget = nullptr;
    m_TreeView.SelectedHeaderButton = nullptr;
    m_TreeView.TargetWidget = nullptr;

    // 设置根控件为空
    SetRootWidget(nullptr, false);
}