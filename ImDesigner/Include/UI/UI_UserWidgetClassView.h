#pragma once
#include "UI/UI_WidgetTreeView.h"
#include "ImWidget/ImUserWidgetClass.h"

namespace ImGuiWidget
{
	class ImWindow;
	class ButtonStateStyle;
}

class UI_UserWidgetClassView : public ImGuiWidget::ImUserWidget
{
private:
	struct viewinfor
	{
		const std::string& varName;
		ImGuiWidget::ImWidget* varView;//对应单个变量的view
	};
private:
	ImGuiWidget::ImButton* TitleButton = nullptr;
	ImGuiWidget::ImUserWidgetClass* TargetClass = nullptr;  // 目标文件对象
	std::map<std::string, viewinfor> AllVarViews;
	ImGuiWidget::ImVerticalBox* RootVBox = nullptr;

	ImGuiWidget::ImWindow* Titlemenu = nullptr;//右键菜单弹出窗口
	ImGuiWidget::ImVerticalBox* TitleMenuWidget = nullptr;//右键弹出菜单
public:
	ImMulticastDelegate<const std::string&> OnRequestAddWidgetTree;//参数：控件类型
	ImMulticastDelegate<const std::string&> OnWidgetVarSelected;
	ImMulticastDelegate<const std::string&, ImGuiWidget::ImWidget*> OnWidgetInTreeSelected;
public:
	UI_UserWidgetClassView(const std::string& name, ImGuiWidget::ImUserWidgetClass* target);
private:
	void InitView();
	

	ImGuiWidget::ImButton* BuildStructViewButton(const std::string& varName);
	void InitTitleMenu();
	ImGuiWidget::ImButton* CreateTitleMenuButton(const std::string& text);
	void On_TitleButtonRightClicked();
};