#pragma once
#include "UI/UI_WidgetTreeView.h"
#include "ImWidget/ImUserWidgetClass.h"

class UI_UserWidgetClassView : public ImGuiWidget::ImUserWidget
{
private:
	struct viewinfor
	{
		const std::string& varName;
		ImGuiWidget::ImWidget* varView;//对应单个变量的view
	};
private:
	ImGuiWidget::ImUserWidgetClass* TargetClass = nullptr;  // 目标文件对象
	std::map<std::string, viewinfor> AllVarViews;
public:
	ImMulticastDelegate<const std::string&> OnRequestAddWidgetTree;//参数：控件类型

public:
	UI_UserWidgetClassView(const std::string& name, ImGuiWidget::ImUserWidgetClass* target);

	void UpdateView();
};