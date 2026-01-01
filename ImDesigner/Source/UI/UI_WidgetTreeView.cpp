#include "UI/UI_WidgetTreeView.h"
#include "ImWidget/ImBasicWidgetList.h"
// 存储节点展开状态 (目标控件名 -> 是否展开)
// 递归构建树节点

ImGuiWidget::ImWidget* UI_WidgetTreeView::BuildTreeNode(ImWidget* nodewidget, std::unordered_set<ImWidget*>& m_ExpandedNode, FileStruct* TargetStruct, int depth)
{
	// 检查是否为PanelWidget
	ImGuiWidget::ImPanelWidget* panelWidget = dynamic_cast<ImGuiWidget::ImPanelWidget*>(nodewidget);

	if (panelWidget)
	{
		// 创建可展开盒子
		ImGuiWidget::ImExpandableBox* expandableBox = new ImGuiWidget::ImExpandableBox(nodewidget->GetWidgetName() + "_TreeNode");

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
		headerButton->SetOnPressed([this, nodewidget, headerButton, TargetStruct]() { On_WidgetSelectedButtonClicked(nodewidget, TargetStruct, headerButton); });

		deletebutton->SetOnPressed([this, nodewidget]() { On_WidgetDeleteButtonClicked(nodewidget); });
		TargetStruct->WidgetToHeaderButton[nodewidget] = headerButton;
		TargetStruct->HeaderButtonToWidget[headerButton] = nodewidget;
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
				ImWidget* childNode = BuildTreeNode(slot->GetContent(), m_ExpandedNode, TargetStruct, depth + 1);
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
		nodeButton->SetOnPressed([this, nodewidget, TargetStruct, nodeButton]() { On_WidgetSelectedButtonClicked(nodewidget, TargetStruct, nodeButton); });
		deletebutton->OnLeftClicked.Add([this, nodewidget]() { On_WidgetDeleteButtonClicked(nodewidget); });
		TargetStruct->WidgetToHeaderButton[nodewidget] = nodeButton;
		TargetStruct->HeaderButtonToWidget[nodeButton] = nodewidget;

		ImGuiWidget::ImHorizontalBox* NodeBox = new ImGuiWidget::ImHorizontalBox(nodewidget->GetWidgetName() + "_NodeBox");
		NodeBox->AddChildToHorizontalBox(nodeButton)->SetIfAutoSize(true);
		NodeBox->AddChildToHorizontalBox(deletebutton)->SetIfAutoSize(false);
		return NodeBox;
	}
}

UI_WidgetTreeView::FileStruct* UI_WidgetTreeView::GetActiveFileStruct()
{
	auto it = AllFileView.find(ActiveView);
	if (it != AllFileView.end())
	{
		return it->second;
	}
	else
	{
		return nullptr;
	}
}

void UI_WidgetTreeView::On_WidgetDeleteButtonClicked(ImGuiWidget::ImWidget* widget)
{
	OnWidgetDeleteButtonClicked.Broadcast(widget);
}

void UI_WidgetTreeView::On_WidgetSelectedButtonClicked(ImGuiWidget::ImWidget* widget, FileStruct* TargetStruct, ImGuiWidget::ImButton* nodeButton)
{
	if (TargetStruct->SelectedHeaderButton)
	{
		TargetStruct->SelectedHeaderButton->GetNormalStyle().BackgroundColor = DEFAULT_COLOR;
	}
	TargetStruct->SelectedHeaderButton = nodeButton;
	TargetStruct->SelectedWidget = widget;
	nodeButton->GetNormalStyle().BackgroundColor = HIGHLIGHT_COLOR;
	OnWidgetSelectedButtonClicked.Broadcast(widget);
}


//外部设置选中控件
void UI_WidgetTreeView::SetSelectedWidget(ImWidget* widget)
{
	if (!widget) return;
	if (auto view = GetActiveFileStruct())
	{
		if (!widget->IsInTree(view->TargetWidget))return;//目标不在当前活跃的树中
		view->SelectedWidget = widget;
		if (view->SelectedHeaderButton)
		{
			view->SelectedHeaderButton->GetNormalStyle().BackgroundColor = DEFAULT_COLOR;
		}
		auto it = view->WidgetToHeaderButton.find(widget);
		it->second->GetNormalStyle().BackgroundColor = HIGHLIGHT_COLOR;
		view->SelectedHeaderButton = it->second;

		ImGuiWidget::ImButton* HeaderButton = view->WidgetToHeaderButton.find(widget)->second;
		ImWidget* CurrentNode = HeaderButton;
		while (CurrentNode != view->TreeViewRoot)
		{
			if (ImGuiWidget::ImButton* NextHeadButton = dynamic_cast<ImGuiWidget::ImButton*>(CurrentNode))
			{
				HeaderButton = NextHeadButton;
			}
			if (ImGuiWidget::ImExpandableBox* expandablebox = dynamic_cast<ImGuiWidget::ImExpandableBox*>(CurrentNode))
			{
				expandablebox->SetExpandedState(true);
				view->m_ExpandedNode.insert(view->HeaderButtonToWidget.find(HeaderButton)->second);
			}
			CurrentNode = CurrentNode->GetParents();
		}
	}
}

void UI_WidgetTreeView::CreateNewTreeView(const std::string& Name, ImGuiWidget::ImWidget* Target)
{
	if (!Target)return;
	FileStruct* NewView = new FileStruct;
	NewView->TreeViewRoot = BuildTreeNode(Target, NewView->m_ExpandedNode, NewView);
	NewView->TargetWidget = Target;
	AllFileView.emplace(std::make_pair(Name, NewView));
}

void UI_WidgetTreeView::SetActiveTreeView(const std::string& Name)
{
	auto it = AllFileView.find(Name);
	if (it != AllFileView.end())
	{
		ActiveView = Name;
		SetRootWidget(it->second->TreeViewRoot, false);
	}
}

// 刷新树视图


// 获取当前选中的控件

ImGuiWidget::ImWidget* UI_WidgetTreeView::GetSelectedWidget()
{
	if (auto viewstruct = GetActiveFileStruct())
	{
		return viewstruct->SelectedWidget;
	}
	else
	{
		return nullptr;
	}
}

void UI_WidgetTreeView::Refresh()
{
	if (auto viewstruct = GetActiveFileStruct())
	{
		viewstruct->HeaderButtonToWidget.clear();
		viewstruct->WidgetToHeaderButton.clear();
		viewstruct->TreeViewRoot = BuildTreeNode(viewstruct->TargetWidget, viewstruct->m_ExpandedNode, viewstruct);
		SetRootWidget(viewstruct->TreeViewRoot, true);//重建了根，要删除旧的
		SetSelectedWidget(viewstruct->SelectedWidget);
	}

}

// 设置选中回调

