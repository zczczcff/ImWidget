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
		ImWidget* TargetWidget = nullptr;      // 目标控件
		ImWidget* SelectedWidget = nullptr;       // 当前选中的控件
		std::unordered_set<ImWidget*> m_ExpandedNode;
		std::unordered_map<ImWidget*, ImGuiWidget::ImButton*> WidgetToHeaderButton;
		ImGuiWidget::ImButton* SelectedHeaderButton = nullptr;
	};
private:
	const ImU32 HIGHLIGHT_COLOR = IM_COL32(20, 200, 20, 255); // 高亮颜色
	const ImU32 DEFAULT_COLOR = IM_COL32(0, 102, 204, 255);   // 默认颜色
	std::map<std::string, FileStruct*> AllFileView;
	std::string ActiveView;
	std::function<void(ImWidget*)> OnSelectionChanged; // 选中回调
	std::function<void(ImWidget*)> OnWidgetDeleted; //删除控件回调
	// 存储节点展开状态 (目标控件名 -> 是否展开)
	// 递归构建树节点
	ImWidget* BuildTreeNode(ImWidget* widget, std::unordered_set<ImWidget*>& m_ExpandedNode, FileStruct* TargetStruct,int depth = 0)
	{
		// 检查是否为PanelWidget
		ImGuiWidget::ImPanelWidget* panelWidget = dynamic_cast<ImGuiWidget::ImPanelWidget*>(widget);

		if (panelWidget)
		{
			// 创建可展开盒子
			ImGuiWidget::ImExpandableBox* expandableBox = new ImGuiWidget::ImExpandableBox(widget->GetWidgetName() + "_TreeNode");

			// 创建头部按钮（可选中）
			ImGuiWidget::ImButton* headerButton = new ImGuiWidget::ImButton(widget->GetWidgetName() + "_HeaderBtn");
			ImGuiWidget::ImButton* deletebutton= new ImGuiWidget::ImButton(widget->GetWidgetName() + "_deleteBtn");

			// 设置按钮文本和样式
			ImGuiWidget::ImTextBlock* buttonText = new ImGuiWidget::ImTextBlock(widget->GetWidgetName() + "_HeaderTxt");
			ImGuiWidget::ImTextBlock* deletebuttonText = new ImGuiWidget::ImTextBlock(widget->GetWidgetName() + "_deleteTxt");
			buttonText->SetText(widget->GetWidgetName());
			deletebuttonText->SetText("X");
			headerButton->SetContent(buttonText);
			deletebutton->SetContent(deletebuttonText);
			headerButton->GetNormalStyle().BackgroundColor = DEFAULT_COLOR; 
			// 点击头部按钮选中控件
			headerButton->SetOnPressed([this, widget,headerButton, TargetStruct]()
				{
					if (TargetStruct->SelectedHeaderButton)
					{
						TargetStruct->SelectedHeaderButton->GetNormalStyle().BackgroundColor= DEFAULT_COLOR;
					}
					TargetStruct->SelectedHeaderButton = headerButton;
					TargetStruct->SelectedWidget = widget;
					headerButton->GetNormalStyle().BackgroundColor = HIGHLIGHT_COLOR;
					if (OnSelectionChanged)
					{
						OnSelectionChanged(widget);
					}
				});

			deletebutton->SetOnPressed([this,widget]() 
				{
					if (OnWidgetDeleted)
					{
						OnWidgetDeleted(widget);
					}
					DeleteChildWidget(widget);
				});
			TargetStruct->WidgetToHeaderButton[widget] = headerButton;

			ImGuiWidget::ImHorizontalBox* headerbox = new ImGuiWidget::ImHorizontalBox(widget->GetWidgetName() + "_HeaderBox");
			headerbox->AddChildToHorizontalBox(headerButton)->SetIfAutoSize(true);
			headerbox->AddChildToHorizontalBox(deletebutton)->SetIfAutoSize(false);
			expandableBox->SetHead(headerbox);

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
					ImWidget* childNode = BuildTreeNode(slot->GetContent(), m_ExpandedNode,TargetStruct,depth + 1);
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
			ImGuiWidget::ImButton* deletebutton = new ImGuiWidget::ImButton(widget->GetWidgetName() + "_deleteBtn");
			ImGuiWidget::ImTextBlock* deletebuttonText = new ImGuiWidget::ImTextBlock(widget->GetWidgetName() + "_deleteTxt");
			deletebuttonText->SetText("X");
			deletebutton->SetContent(deletebuttonText);

			ImGuiWidget::ImTextBlock* buttonText = new ImGuiWidget::ImTextBlock(widget->GetWidgetName() + "_NodeTxt");
			buttonText->SetHorizontalAlignment(ImGuiWidget::ImTextBlock::TextAlignment_Horizontal::Left);
			buttonText->SetText(widget->GetWidgetName());
			nodeButton->SetContent(buttonText);
			nodeButton->GetNormalStyle().BackgroundColor = DEFAULT_COLOR;
			// 点击按钮选中控件
			nodeButton->SetOnPressed([this, widget, TargetStruct, nodeButton]()
				{
					if (TargetStruct->SelectedHeaderButton)
					{
						TargetStruct->SelectedHeaderButton->GetNormalStyle().BackgroundColor = DEFAULT_COLOR;
					}
					TargetStruct->SelectedHeaderButton = nodeButton;
					TargetStruct->SelectedWidget = widget;
					nodeButton->GetNormalStyle().BackgroundColor = HIGHLIGHT_COLOR;
					if (OnSelectionChanged)
					{
						OnSelectionChanged(widget);
					}
				});
			deletebutton->SetOnPressed([this, widget]()
				{
					if (OnWidgetDeleted)
					{
						OnWidgetDeleted(widget);
					}
					DeleteChildWidget(widget);
				});
			TargetStruct->WidgetToHeaderButton[widget] = nodeButton;

			ImGuiWidget::ImHorizontalBox* NodeBox = new ImGuiWidget::ImHorizontalBox(widget->GetWidgetName() + "_NodeBox");
			NodeBox->AddChildToHorizontalBox(nodeButton)->SetIfAutoSize(true);
			NodeBox->AddChildToHorizontalBox(deletebutton)->SetIfAutoSize(false);
			return NodeBox;
		}
	}
	//外部设置选中控件
	void SetSelectedWidget(ImWidget* widget)
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
	//virtual void HandleInteraction() override
	//{
	//	ImUserWidget::HandleInteraction();

	//	if (ImGui::IsKeyPressed(ImGuiKey_Delete)&&GetIsFocused())
	//	{
	//		printf("delete widget");
	//	}
	//}

	void CreateNewTreeView(const std::string& Name,ImGuiWidget::ImWidget* Target)
	{
		if (!Target)return;
		FileStruct* NewView = new FileStruct;
		NewView->TreeViewRoot = BuildTreeNode(Target, NewView->m_ExpandedNode, NewView);
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
	void SetOnSelectionChanged(std::function<void(ImWidget*)> callback)
	{
		OnSelectionChanged = callback;
	}
	//设置删除回调
	void SetOnWidgetDeleted(std::function<void(ImWidget*)> callback) { OnWidgetDeleted = callback; }
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

	/**
	* 从目标控件树中删除指定子控件
	*
	* @param childToDelete 要删除的子控件指针
	* @param deleteObject 是否真正删除对象 (true=删除内存，false=仅从树中移除)
	* @return 是否成功删除
	*/
	bool DeleteChildWidget(ImWidget* childToDelete)
	{
		if (!childToDelete) return false;

		// 获取当前活动视图
		if (auto view = GetActiveFileStruct())
		{
			// 1. 处理删除根控件的特殊情况
			if (childToDelete == view->TargetWidget)
			{
				// 清除相关状态
				view->m_ExpandedNode.clear();
				view->SelectedWidget = nullptr;
				view->TreeViewRoot = nullptr;
				view->TargetWidget = nullptr;

				// 刷新视图
				SetRootWidget(nullptr);
				return true;
			}

			// 2. 获取直接父控件
			ImGuiWidget::ImPanelWidget* parentPanel =
				dynamic_cast<ImGuiWidget::ImPanelWidget*>(childToDelete->GetParents());

			if (!parentPanel) return false; // 没有父控件或父控件不是容器

			// 3. 从父控件中删除子控件
			parentPanel->RemoveChild(childToDelete); // 完全删除

			// 4. 清理相关状态
			view->m_ExpandedNode.erase(childToDelete);
			if (view->SelectedWidget == childToDelete) {
				view->SelectedWidget = nullptr;
			}

			// 5. 刷新视图
			Refresh();
			return true;
		}
		return false;
	}

	// 刷新树视图
	void Refresh()
	{

		if (auto viewstruct = GetActiveFileStruct())
		{
			viewstruct->TreeViewRoot= BuildTreeNode(viewstruct->TargetWidget,viewstruct->m_ExpandedNode, viewstruct);
			SetRootWidget(viewstruct->TreeViewRoot, true);//重建了根，要删除旧的
			SetSelectedWidget(viewstruct->SelectedWidget);
		}

		//if (m_TargetWidget)
		//{
		//	ImWidget* newTree = BuildTreeNode(m_TargetWidget);
		//	SetRootWidget(newTree);
		//}
	}
};
