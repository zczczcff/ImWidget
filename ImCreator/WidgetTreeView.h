#pragma once
#include "ImWidget/ImUserWidget.h"
#include "ImWidget/ImExpandableBox.h"
#include "ImWidget/ImVerticalBox.h"
#include "ImWidget/ImTextBlock.h"
#include "ImWidget/ImButton.h"
#include "ImWidget/ImPanelWidget.h"
#include <functional>
#include <unordered_set>

class WidgetTreeView : public ImGuiWidget::ImUserWidget
{
private:
	struct FileStruct
	{
		ImWidget* TreeViewRoot = nullptr;//视图根控件
		ImWidget* TargetWidget;          // 目标控件
		ImWidget* SelectedWidget;        // 当前选中的控件
		std::unordered_set<ImWidget*> m_ExpandedNode;

	};
private:
	//ImWidget* m_TargetWidget;          // 目标控件
	//ImWidget* m_SelectedWidget;        // 当前选中的控件

	std::map<std::string, FileStruct*> AllFileView;
	std::string ActiveView;
	//std::function<void(ImWidget*)> OnSelectionChanged; // 选中回调
	// 存储节点展开状态 (目标控件名 -> 是否展开)
	//std::unordered_set<ImWidget*> m_ExpandedNode;
	// 递归构建树节点
	ImWidget* BuildTreeNode(ImWidget* widget, std::unordered_set<ImWidget*>& m_ExpandedNode, int depth = 0)
	{
		// 检查是否为PanelWidget
		ImGuiWidget::ImPanelWidget* panelWidget = dynamic_cast<ImGuiWidget::ImPanelWidget*>(widget);

		if (panelWidget)
		{
			// 创建可展开盒子
			ImGuiWidget::ImExpandableBox* expandableBox = new ImGuiWidget::ImExpandableBox(widget->GetWidgetName() + "_TreeNode");

			// 创建头部按钮（可选中）
			ImGuiWidget::ImButton* headerButton = new ImGuiWidget::ImButton(widget->GetWidgetName() + "_HeaderBtn");
			headerButton->SetSize(ImVec2(200, 20));

			// 设置按钮文本和样式
			ImGuiWidget::ImTextBlock* buttonText = new ImGuiWidget::ImTextBlock(widget->GetWidgetName() + "_HeaderTxt");
			buttonText->SetText(widget->GetWidgetName());
			headerButton->SetContent(buttonText);

			// 点击头部按钮选中控件
			headerButton->SetOnPressed([this, widget]() {
				SetSelectedWidget(widget);
				});

			expandableBox->SetHead(headerButton);

			// 创建垂直容器存放子节点
			ImGuiWidget::ImVerticalBox* childContainer = new ImGuiWidget::ImVerticalBox(widget->GetWidgetName() + "_ChildContainer");
			expandableBox->SetBody(childContainer);

			// 递归添加子节点
			int slotCount = panelWidget->GetSlotNum();
			for (int i = 0; i < slotCount; ++i)
			{
				ImGuiWidget::ImSlot* slot = panelWidget->GetSlotAt(i);
				if (slot && slot->GetContent())
				{
					ImWidget* childNode = BuildTreeNode(slot->GetContent(), m_ExpandedNode,depth + 1);
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
			ImGuiWidget::ImButton* nodeButton = new ImGuiWidget::ImButton(widget->GetWidgetName() + "_NodeBtn");


			ImGuiWidget::ImTextBlock* buttonText = new ImGuiWidget::ImTextBlock(widget->GetWidgetName() + "_NodeTxt");
			buttonText->SetHorizontalAlignment(ImGuiWidget::ImTextBlock::TextAlignment_Horizontal::Left);
			buttonText->SetText(widget->GetWidgetName());
			nodeButton->SetContent(buttonText);

			// 点击按钮选中控件
			nodeButton->SetOnPressed([this, widget]() {
				SetSelectedWidget(widget);
				});

			return nodeButton;
		}
	}

	void SetSelectedWidget(ImWidget* widget)
	{
		if (auto view = GetActiveFileStruct())
		{
			view->SelectedWidget = widget;
		}
		//m_SelectedWidget = widget;
		//if (OnSelectionChanged)
		//{
		//	OnSelectionChanged(widget);
		//}
	}

	void AddSingleNodeState(ImWidget* TargetWidget)
	{
		if (!TargetWidget)return;
		if (ImGuiWidget::ImPanelWidget* panel = dynamic_cast<ImGuiWidget::ImPanelWidget*>(TargetWidget))
		{

		}
	}

	FileStruct* GetActiveFileStruct()
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
public:
	WidgetTreeView(const std::string& WidgetName)
		: ImUserWidget(WidgetName)
		//m_SelectedWidget(nullptr)
	{
	}

	//void SetTargetWidget(ImWidget* targetWidget)
	//{
	//	m_TargetWidget = targetWidget;
	//	if (m_TargetWidget)
	//	{
	//		// 构建树形结构
	//		ImWidget* treeRoot = BuildTreeNode(m_TargetWidget);
	//		SetRootWidget(treeRoot);
	//	}
	//	else
	//	{
	//		SetRootWidget(nullptr);
	//	}
	//}

	void CreateNewTreeView(const std::string& Name,ImGuiWidget::ImWidget* Target)
	{
		if (!Target)return;
		FileStruct* NewView = new FileStruct;
		NewView->TreeViewRoot = BuildTreeNode(Target, NewView->m_ExpandedNode);
		NewView->TargetWidget = Target;
		AllFileView.emplace(std::make_pair(Name, NewView));
	}

	void SetActiveTreeView(const std::string& Name)
	{
		auto it = AllFileView.find(Name);
		if (it != AllFileView.end())
		{
			ActiveView = Name;
			SetRootWidget(it->second->TreeViewRoot, false);
		}
	}
	// 设置选中回调
	//void SetOnSelectionChanged(std::function<void(ImWidget*)> callback)
	//{
	//	OnSelectionChanged = callback;
	//}

	// 获取当前选中的控件
	ImWidget* GetSelectedWidget()
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

	//ImWidget* GetTargetWidget() { return m_TargetWidget; }
	// 刷新树视图
	void Refresh()
	{

		if (auto viewstruct = GetActiveFileStruct())
		{
			viewstruct->TreeViewRoot= BuildTreeNode(viewstruct->TargetWidget,viewstruct->m_ExpandedNode);
			SetRootWidget(viewstruct->TreeViewRoot, true);//重建了根，要删除旧的
		}

		//if (m_TargetWidget)
		//{
		//	ImWidget* newTree = BuildTreeNode(m_TargetWidget);
		//	SetRootWidget(newTree);
		//}
	}
};
