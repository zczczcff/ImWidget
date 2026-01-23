#include "UI/UI_WidgetTreeView.h"
#include "ImWidget/ImBasicWidgetList.h"
#include "ImGlobal.h"
#include "Public/WidgetInfor.h"
#include "Tools/JLog.h"
#include "EditorAction.h"
#include "EditorEvents.h"

void UI_WidgetTreeView::ActionInit()
{
    ResetAction();
    ResetEvent();
    AddSequentialProcessor(Action::ProjectView::RENAME_FILE, [this](const std::string& OldFullPath, const std::string& NewFullPath)
        {
            if (EditedFileFullPath == OldFullPath)
            {
                EditedFileFullPath = NewFullPath;
                ResetAction();
                ResetEvent();
            }
        });
}

void UI_WidgetTreeView::ResetEvent()
{
    for (auto& id : FileEvents)
    {
        Unsubscribe(id);
    }
    FileEvents.clear();

    FileEvents.push_back(Subscribe(EditedFileFullPath + Events::UIFileView::UPDATE_WIDGETTREE_VIEW, [this]()
        {
            Refresh();
        }));
}

void UI_WidgetTreeView::ResetAction()
{
    for (auto& id : FileActions)
    {
        RemoveProcessor(id);
    }
    FileActions.clear();

    FileActions.push_back(AddSequentialProcessor(EditedFileFullPath + Action::WIDGET_SELECTED, [this](ImGuiWidget::ImWidget* SelectedWidget)
        {
            SetSelectedWidget(SelectedWidget);
        }));

    //FileActions.push_back(AddSequentialProcessor(EditedFileFullPath + Events::UIFileView::UPDATE_WIDGETTREE_VIEW, [this]() 
    //    {
    //        Refresh();
    //    }));

}

void UI_WidgetTreeView::InitPopUpMenu()
{
    auto windowmanager = ImGuiWidget::GetGlobalApp()->GetWindowManager();
    ImVerticalBox_WidgetMenu = new ImGuiWidget::ImVerticalBox("ImVerticalBox_WidgetMenu");
    ImGuiWidget::ImButton* CopyButton = CreateWidgetMenuButton(u8"复制");
    ImGuiWidget::ImButton* DeleteButton = CreateWidgetMenuButton(u8"删除");
    ImGuiWidget::ImButton* InsertPreviousButton = CreateWidgetMenuButton(u8"在前一个位置插入", true);
    ImGuiWidget::ImButton* InsertToButton = CreateWidgetMenuButton(u8"插入控件", true);
    ImGuiWidget::ImButton* InsertAfterButton = CreateWidgetMenuButton(u8"在后一个位置插入", true);
    DeleteButton->OnLeftClicked.Add([this]() { On_WidgetDeleteButtonClicked(PopupMenuTargetWidget); });
    ImVerticalBox_WidgetMenu->AddChildToVerticalBox(CopyButton)->SetIfAutoSize(false);
    ImVerticalBox_WidgetMenu->AddChildToVerticalBox(DeleteButton)->SetIfAutoSize(false);
    ImVerticalBox_WidgetMenu->AddChildToVerticalBox(InsertPreviousButton)->SetIfAutoSize(false);
    ImVerticalBox_WidgetMenu->AddChildToVerticalBox(InsertToButton)->SetIfAutoSize(false);
    ImVerticalBox_WidgetMenu->AddChildToVerticalBox(InsertAfterButton)->SetIfAutoSize(false);

    WidgetMenu = windowmanager->CreatePopupWindow(ImVerticalBox_WidgetMenu->GetMinSize(), ImVec2(0, 0), ImVerticalBox_WidgetMenu, false);
    WidgetMenu->Close();

    ImVerticalBox_WidgetMenu_InsertNew=new ImGuiWidget::ImVerticalBox("ImVerticalBox_WidgetMenu_InsertNew");
    for (auto& SingleWidgetInfor : BasicWidgetList::GetBasicWidgetList())
    {
        ImGuiWidget::ImButton* Example_Button = CreateWidgetInsertButton
        (
            SingleWidgetInfor.CN_DisplayName,
            SingleWidgetInfor.RegisterName,
            IconManager::GetInstance()->GetIcon(SingleWidgetInfor.IconID)
        );
        ImVerticalBox_WidgetMenu_InsertNew->AddChildToVerticalBox(Example_Button)->SetIfAutoSize(false);
    }
    WidgetMenu_InsertNew = windowmanager->CreatePopupWindow(ImVerticalBox_WidgetMenu_InsertNew->GetMinSize(), ImVec2(0, 0), ImVerticalBox_WidgetMenu_InsertNew, false, WidgetMenu);
    WidgetMenu_InsertNew->Close();

    CopyButton->OnMouseHover.Add([this]() 
        {
            WidgetMenu_InsertNew->Close();
        });
    DeleteButton->OnMouseHover.Add([this]() 
        {
            WidgetMenu_InsertNew->Close();
        });

    InsertPreviousButton->OnMouseHover.Add([this, InsertPreviousButton]()
        {
            m_InsertMode = InsertChildMode::InsertPrevious;
            ImVec2 PopupPos = InsertPreviousButton->GetPosition() + ImVec2(InsertPreviousButton->GetSize().x, 0);
            WidgetMenu_InsertNew->SetPopupRect(PopupPos);
            WidgetMenu_InsertNew->SetActive();
        });
    InsertPreviousButton->OnMouseHoverOut.Add([this]() 
        {
            //WidgetMenu_InsertNew->Close(); 
        });

    InsertToButton->OnMouseHover.Add([this, InsertToButton]()
        {
            m_InsertMode = InsertChildMode::InsertToThis;
            ImVec2 PopupPos = InsertToButton->GetPosition() + ImVec2(InsertToButton->GetSize().x, 0);
            WidgetMenu_InsertNew->SetPopupRect(PopupPos);
            WidgetMenu_InsertNew->SetActive();
        });
    InsertToButton->OnMouseHoverOut.Add([this]() 
        {
            //WidgetMenu_InsertNew->Close(); 
        });

    InsertAfterButton->OnMouseHover.Add([this, InsertAfterButton]()
        {
            m_InsertMode = InsertChildMode::InsertNext;
            ImVec2 PopupPos = InsertAfterButton->GetPosition() + ImVec2(InsertAfterButton->GetSize().x, 0);
            WidgetMenu_InsertNew->SetPopupRect(PopupPos);
            WidgetMenu_InsertNew->SetActive();
        });
    InsertAfterButton->OnMouseHoverOut.Add([this]() 
        {
            //WidgetMenu_InsertNew->Close(); 
        });
}

void UI_WidgetTreeView::InitButtonStyle()
{
    ImGuiWidget::ImButton* tempbutton = new ImGuiWidget::ImButton("tempbutton");
    Highlight_Style = new ImGuiWidget::ButtonStateStyle();
    Normal_Style=new ImGuiWidget::ButtonStateStyle();
    (*Normal_Style) = tempbutton->GetNormalStyle();
    (*Highlight_Style) = tempbutton->GetFocusedStyle();
    delete tempbutton;
}

ImGuiWidget::ImButton* UI_WidgetTreeView::CreateWidgetMenuButton(const std::string& Text, bool bHaveSubMenu)
{
    ImGuiWidget::ImButton* button = new ImGuiWidget::ImButton("UI_WidgetTreeView_WidgetMenuButton");
    button->bHaveBorder = false;
    ImGuiWidget::ImTextBlock* text = new ImGuiWidget::ImTextBlock("UI_WidgetTreeView_WidgetMenuButton_Text");
    text->SetText(Text);
    text->SetHorizontalAlignment(ImGuiWidget::ImTextBlock::TextAlignment_Horizontal::Left);


    ImGuiWidget::ImHorizontalBox* HBox = new ImGuiWidget::ImHorizontalBox("UI_WidgetTreeView_HBox");
    auto hboxslot = HBox->AddChildToHorizontalBox(text);
    hboxslot->PaddingLeft = 10;
    hboxslot->PaddingRight = 16;
    hboxslot->PaddingBottom = 2;
    hboxslot->PaddingTop = 2;

    if (bHaveSubMenu)
    {
        ImGuiWidget::ImTextBlock* text_arrow = new ImGuiWidget::ImTextBlock("UI_WidgetTreeView_WidgetMenuButton_Text");
        text_arrow->SetText(u8">");
        hboxslot = HBox->AddChildToHorizontalBox(text_arrow);
        hboxslot->SetIfAutoSize(false);
        hboxslot->PaddingRight = 2;
    }

    HBox->bHaveBackGround = false;
    HBox->bHaveBorder = false;
    button->SetContent(HBox);
    //button->GetContentSlot()->SetPadding(2, 2, 10, 10);

    button->bHaveBorder = false;
    button->OnLeftClicked.Add([this]() { WidgetMenu->Close(); });
    return button;
}

ImGuiWidget::ImButton* UI_WidgetTreeView::CreateWidgetInsertButton(const std::string& CN_Name, const std::string& RegisterName, ImTextureID icon)
{
    ImGuiWidget::ImButton* button = new ImGuiWidget::ImButton("UI_WidgetTreeView_WidgetMenuButton");
    button->bHaveBorder = false;
    ImGuiWidget::ImTextBlock* text = new ImGuiWidget::ImTextBlock("UI_WidgetTreeView_WidgetMenuButton_Text");
    text->SetText(CN_Name);
    text->SetHorizontalAlignment(ImGuiWidget::ImTextBlock::TextAlignment_Horizontal::Left);
    ImGuiWidget::ImHorizontalBox* HBox = new ImGuiWidget::ImHorizontalBox("UI_WidgetTreeView_WidgetInsertHBox");
    ImGuiWidget::ImImage* Icon = new ImGuiWidget::ImImage("UI_WidgetTreeView_WidgetInsert_Icon", icon, 24, 24);
    HBox->AddChildToHorizontalBox(Icon)->SetIfAutoSize(false);
    HBox->AddChildToHorizontalBox(text);
    HBox->bHaveBackGround = false;
    //HBox->SetBackGroundColor(IM_COL32(255, 255, 255, 255));
    button->SetContent(HBox);
    button->OnLeftClicked.Add([this, RegisterName]() 
        {
            WidgetMenu_InsertNew->Close();
            WidgetMenu->Close();
            On_InsertWidgetButtonClicked(RegisterName); 
        });
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
        buttonText->SetHorizontalAlignment(ImGuiWidget::ImTextBlock::ImTextBlock::TextAlignment_Horizontal::Left);
        deletebuttonText->SetText("X");
        headerButton->SetContent(buttonText);
        deletebutton->SetContent(deletebuttonText);
        //headerButton->GetNormalStyle().BackgroundColor = DEFAULT_COLOR;
        headerButton->SetNormalStyle(*Normal_Style);
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
        headerbox->bHaveBorder = false;
        headerbox->bHaveBackGround = false;
        // 创建垂直容器存放子节点
        ImGuiWidget::ImVerticalBox* childContainer = new ImGuiWidget::ImVerticalBox(nodewidget->GetWidgetName() + "_ChildContainer");
        expandableBox->SetBody(childContainer);
        childContainer->bHaveBorder = false;
        childContainer->bHaveBackGround = false;
        // 递归添加子节点
        int slotCount = panelWidget->GetChildNum();
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
        //nodeButton->GetNormalStyle().BackgroundColor = DEFAULT_COLOR;
        nodeButton->SetNormalStyle(*Normal_Style);
        // 点击按钮选中控件
        nodeButton->SetOnPressed([this, nodewidget, nodeButton]() { On_WidgetSelectedButtonClicked(nodewidget, nodeButton); });
        nodeButton->OnRightClicked.Add([this, nodewidget]() { On_WidgetSelectedButtonRightClicked(nodewidget); });
        deletebutton->OnLeftClicked.Add([this, nodewidget]() { On_WidgetDeleteButtonClicked(nodewidget); });
        m_TreeView.WidgetToHeaderButton[nodewidget] = nodeButton;
        m_TreeView.HeaderButtonToWidget[nodeButton] = nodewidget;

        ImGuiWidget::ImHorizontalBox* NodeBox = new ImGuiWidget::ImHorizontalBox(nodewidget->GetWidgetName() + "_NodeBox");
        NodeBox->AddChildToHorizontalBox(nodeButton)->SetIfAutoSize(true);
        NodeBox->AddChildToHorizontalBox(deletebutton)->SetIfAutoSize(false);
        NodeBox->bHaveBorder = false;
        NodeBox->bHaveBackGround = false;
        return NodeBox;
    }
}

ImGuiWidget::ImWidget* UI_WidgetTreeView::BuildRootNode(ImGuiWidget::ImWidget* widget, std::unordered_set<ImGuiWidget::ImWidget*>& m_ExpandedNode)
{
    ImGuiWidget::ImButton* HeadButton = new ImGuiWidget::ImButton(widget->GetWidgetName() + "_HeadButton");
    ImGuiWidget::ImHorizontalBox* HBox = new ImGuiWidget::ImHorizontalBox(widget->GetWidgetName() + "_HBox");
    HBox->bHaveBackGround = false;
    ImGuiWidget::ImTextBlock* text = new ImGuiWidget::ImTextBlock(widget->GetWidgetName() + "_text");
    text->SetText(widget->GetWidgetName());
    text->SetHorizontalAlignment(ImGuiWidget::ImTextBlock::TextAlignment_Horizontal::Center);
    ImGuiWidget::ImImage* icon = nullptr;
    HeadButton->SetOnPressed([this, widget, HeadButton]() { On_WidgetSelectedButtonClicked(widget, HeadButton); });
    HeadButton->OnRightClicked.Add([this, widget]() { On_WidgetSelectedButtonRightClicked(widget); });
    m_TreeView.WidgetToHeaderButton[widget] = HeadButton;
    m_TreeView.HeaderButtonToWidget[HeadButton] = widget;
    HeadButton->SetNormalStyle(*Normal_Style);
    if (widget->GetChildNum() > 0)
    {
        icon = new ImGuiWidget::ImImage(widget->GetWidgetName() + "_icon", IconManager::GetInstance()->GetIcon(ImDesignerIcon::WidgetTree), 24, 24);
        icon->SetTintcolor(IM_COL32(20, 180, 20, 255));
        ImGuiWidget::ImExpandableBox* expandableBox = new ImGuiWidget::ImExpandableBox(widget->GetWidgetName() + "_TreeNode");
        HBox->AddChildToHorizontalBox(icon)->SetIfAutoSize(false);
        HBox->AddChildToHorizontalBox(text)->SetIfAutoSize(true);
        HeadButton->SetContent(HBox);

        expandableBox->bHaveBorder = false;
        ImGuiWidget::ImVerticalBox* childContainer = new ImGuiWidget::ImVerticalBox(widget->GetWidgetName() + "_ChildContainer");
        expandableBox->SetHead(HeadButton);
        expandableBox->SetBody(childContainer);
        //childContainer->bHaveBorder = false;
        childContainer->bHaveBackGround = false;

        for (int i = 0; i < widget->GetChildNum(); ++i)
        {
                ImWidget* childNode = BuildTreeNode(widget->GetChildAt(i), m_ExpandedNode);
                if (childNode)
                {
                    childContainer->AddChildToVerticalBox(childNode)->SetIfAutoSize(false);
                }
        }

        if (m_ExpandedNode.find(widget) != m_ExpandedNode.end())
        {
            expandableBox->SetExpandedState(true);
        }

        expandableBox->SetOnExpandedStateChanged([this, widget, &m_ExpandedNode](bool newstate) mutable
            {
                if (newstate)
                {
                    m_ExpandedNode.insert(widget);
                }
                else
                {
                    m_ExpandedNode.erase(widget);
                }
            });
        return expandableBox;
    }
    else
    {
        icon = new ImGuiWidget::ImImage(widget->GetWidgetName() + "_icon", IconManager::GetInstance()->GetIcon(ImDesignerIcon::SingleWidget), 24, 24);
        icon->SetTintcolor(IM_COL32(20, 20, 180, 255));
        HBox->AddChildToHorizontalBox(icon)->SetIfAutoSize(false);
        HBox->AddChildToHorizontalBox(text)->SetIfAutoSize(true);
        HeadButton->SetContent(HBox);
        return HeadButton;
    }


}

void UI_WidgetTreeView::On_WidgetDeleteButtonClicked(ImGuiWidget::ImWidget* widget)
{
    if (ExecuteAction(EditedFileFullPath + Action::UIFileView::REQUEST_DELETE_WIDGET, widget).success)
    {
        Refresh();
    }
    //OnRequestWidgetDeleted.Broadcast(widget);
}

void UI_WidgetTreeView::On_WidgetSelectedButtonClicked(ImGuiWidget::ImWidget* widget, ImGuiWidget::ImButton* nodeButton)
{
    if (m_TreeView.SelectedHeaderButton)
    {
        //m_TreeView.SelectedHeaderButton->GetNormalStyle().BackgroundColor = DEFAULT_COLOR;
        m_TreeView.SelectedHeaderButton->SetNormalStyle(*Normal_Style);
    }
    m_TreeView.SelectedHeaderButton = nodeButton;
    m_TreeView.SelectedWidget = widget;
    //nodeButton->GetNormalStyle().BackgroundColor = HIGHLIGHT_COLOR;
    nodeButton->SetNormalStyle(*Highlight_Style);
    ExecuteAction(EditedFileFullPath + Action::WIDGET_SELECTED, widget);
    //OnWidgetSelectedButtonClicked.Broadcast(widget);
}

void UI_WidgetTreeView::On_WidgetSelectedButtonRightClicked(ImGuiWidget::ImWidget* widget)
{
    PopupMenuTargetWidget = widget;
    WidgetMenu->SetPopupRect(ImGuiWidget::GetMousePos());
    WidgetMenu->SetActive();
}

void UI_WidgetTreeView::On_InsertWidgetButtonClicked(const std::string& InsertWidgetRegisterName)
{
	if (!PopupMenuTargetWidget) return;

	if (m_InsertMode == InsertChildMode::InsertToThis)
	{
        ExecuteAction(Action::UIFileView::REQUEST_INSERT_WIDGET, PopupMenuTargetWidget, PopupMenuTargetWidget->GetChildNum(), InsertWidgetRegisterName);
		//OnRequestInsertWidget.Broadcast(PopupMenuTargetWidget, PopupMenuTargetWidget->GetChildNum(), InsertWidgetRegisterName);
	}
	else if (PopupMenuTargetWidget != m_TreeView.TargetWidget)
	{
		int InsertIndex = -1;
        ImGuiWidget::ImWidget* Target = PopupMenuTargetWidget->GetParents();
		for (int i = 0; i < Target->GetChildNum(); i++)
		{
			if (Target->GetChildAt(i) == PopupMenuTargetWidget)
			{
				InsertIndex = i;
				break;
			}
		}
        if (InsertIndex < 0) return;
		if (m_InsertMode == InsertChildMode::InsertNext)
		{
			InsertIndex++;
		}
        ExecuteAction(Action::UIFileView::REQUEST_INSERT_WIDGET, Target, InsertIndex, InsertWidgetRegisterName);
		//OnRequestInsertWidget.Broadcast(Target, InsertIndex, InsertWidgetRegisterName);
	}
}

// 设置目标控件树
void UI_WidgetTreeView::SetTargetWidget(ImWidget* Target)
{
    if (!Target) return;

    // 清空现有树视图
    Clear();

    m_TreeView.TargetWidget = Target;
    m_TreeView.TreeViewRoot = BuildRootNode(Target, m_TreeView.m_ExpandedNode);
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
        //m_TreeView.SelectedHeaderButton->GetNormalStyle().BackgroundColor = DEFAULT_COLOR;
        m_TreeView.SelectedHeaderButton->SetNormalStyle(*Normal_Style);
    }

    auto it = m_TreeView.WidgetToHeaderButton.find(widget);
    if (it != m_TreeView.WidgetToHeaderButton.end())
    {
        //it->second->GetNormalStyle().BackgroundColor = HIGHLIGHT_COLOR;
        it->second->SetNormalStyle(*Highlight_Style);
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