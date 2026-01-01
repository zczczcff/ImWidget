#pragma once

#include "ImWidget/ImUserWidget.h"
#include "ImWidget/ImBasicWidgetDeclaration.h"
#include "ImTools/ImDelegate.h"
#include <string>
#include <map>


class UI_WidgetTreeView :public ImGuiWidget::ImUserWidget
{
private:
	struct FileStruct
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
	std::map<std::string, FileStruct*> AllFileView;
	std::string ActiveView;
	std::function<void(ImWidget*)> OnSelectionChanged; // 选中回调
	std::function<void(ImWidget*)> OnWidgetDeleted; //删除控件回调
public:
	ImMulticastDelegate<ImGuiWidget::ImWidget*> OnWidgetDeletedButtonClicked;
private:
	// 存储节点展开状态 (目标控件名 -> 是否展开)
	// 递归构建树节点
	ImGuiWidget::ImWidget* BuildTreeNode(ImGuiWidget::ImWidget* widget, std::unordered_set<ImGuiWidget::ImWidget*>& m_ExpandedNode, FileStruct* TargetStruct, int depth = 0);

	FileStruct* GetActiveFileStruct();

public:
	UI_WidgetTreeView(const std::string& WidgetName)
		: ImUserWidget(WidgetName)
		//m_SelectedWidget(nullptr)
	{
		SetAllowDragOn(true);
	}
		//外部设置选中控件
	void SetSelectedWidget(ImGuiWidget::ImWidget* widget);

	void CreateNewTreeView(const std::string& Name, ImGuiWidget::ImWidget* Target);

	void SetActiveTreeView(const std::string& Name);

	// 获取当前选中的控件
	ImGuiWidget::ImWidget* GetSelectedWidget();
	// 刷新树视图
	void Refresh();

};