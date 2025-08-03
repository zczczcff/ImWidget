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
	ImWidget* m_TargetWidget;          // 目标控件
	ImWidget* m_SelectedWidget;        // 当前选中的控件
	std::function<void(ImWidget*)> OnSelectionChanged; // 选中回调
	// 存储节点展开状态 (目标控件名 -> 是否展开)
	std::unordered_set<ImWidget*> m_ExpandedNode;
	// 递归构建树节点
	ImWidget* BuildTreeNode(ImWidget* widget, int depth = 0)
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
					ImWidget* childNode = BuildTreeNode(slot->GetContent(), depth + 1);
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

			expandableBox->SetOnExpandedStateChanged([this, panelWidget](bool newstate)
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
		m_SelectedWidget = widget;
		if (OnSelectionChanged)
		{
			OnSelectionChanged(widget);
		}
	}

	void AddSingleNodeState(ImWidget* TargetWidget)
	{
		if (!TargetWidget)return;
		if (ImGuiWidget::ImPanelWidget* panel = dynamic_cast<ImGuiWidget::ImPanelWidget*>(TargetWidget))
		{

		}
	}

public:
	WidgetTreeView(const std::string& WidgetName)
		: ImUserWidget(WidgetName),
		m_SelectedWidget(nullptr)
	{
	}

	void SetTargetWidget(ImWidget* targetWidget)
	{
		m_TargetWidget = targetWidget;
		if (m_TargetWidget)
		{
			// 构建树形结构
			ImWidget* treeRoot = BuildTreeNode(m_TargetWidget);
			SetRootWidget(treeRoot);
		}
		else
		{
			SetRootWidget(nullptr);
		}
	}
	// 设置选中回调
	void SetOnSelectionChanged(std::function<void(ImWidget*)> callback)
	{
		OnSelectionChanged = callback;
	}

	// 获取当前选中的控件
	ImWidget* GetSelectedWidget() const
	{
		return m_SelectedWidget;
	}

	ImWidget* GetTargetWidget() { return m_TargetWidget; }
	// 刷新树视图
	void Refresh()
	{

		if (m_TargetWidget)
		{
			ImWidget* newTree = BuildTreeNode(m_TargetWidget);
			SetRootWidget(newTree);
		}
	}
};
