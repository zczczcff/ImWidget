#pragma once
#include "ImUserWidget.h"

#include "ImBasicWidgetDeclaration.h"

class Widget_PageTag : public ImGuiWidget::ImUserWidget
{
public:
    void Init();

    Widget_PageTag(const std::string& name): ImGuiWidget::ImUserWidget(name)
{
    Init();
}

protected:
//----Gen Members Begin----
    // Auto-generated widget pointers
    ImGuiWidget::ImButton* ImButton_PageButton;
    ImGuiWidget::ImHorizontalBox* ImHorizontalBox_2;
    ImGuiWidget::ImTextBlock* ImTextBlock_PageName;
    ImGuiWidget::ImButton* ImButton_Close;
    ImGuiWidget::ImTextBlock* ImTextBlock_6;
//----Gen Members End----
};
