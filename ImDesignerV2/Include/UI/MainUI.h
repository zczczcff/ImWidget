#pragma once
#include "ImUserWidget.h"

#include "ImBasicWidgetDeclaration.h"

class MainUI : public ImGuiWidget::ImUserWidget
{
public:
    void Init();

    MainUI(const std::string& name): ImGuiWidget::ImUserWidget(name)
{
    Init();
}

protected:
//----Gen Members Begin----
    // Auto-generated widget pointers
    ImGuiWidget::ImVerticalBox* ImVerticalBox_3;
    ImGuiWidget::ImHorizontalBox* ImHorizontalBox_4;
    ImGuiWidget::ImButton* ImButton_Project;
    ImGuiWidget::ImTextBlock* ImTextBlock_9;
    ImGuiWidget::ImButton* ImButton_Save;
    ImGuiWidget::ImTextBlock* ImTextBlock_10;
    ImGuiWidget::ImButton* ImButton_Generate;
    ImGuiWidget::ImTextBlock* ImTextBlock_11;
    ImGuiWidget::ImButton* ImButton_Undo;
    ImGuiWidget::ImImage* ImImage_Undo;
    ImGuiWidget::ImButton* ImButton_Redo;
    ImGuiWidget::ImImage* ImImage_Redo;
    ImGuiWidget::ImVerticalSplitter* ImVerticalSplitter_6;
    ImGuiWidget::ImHorizontalSplitter* ImHorizontalSplitter_1;
    ImGuiWidget::ImBorder* ImBorder_Left;
    ImGuiWidget::ImVerticalSplitter* ImVerticalSplitter_0;
    ImGuiWidget::ImScrollBox* ImScrollBox_WidgetList;
    ImGuiWidget::ImVerticalBox* ImVerticalBox_WidgetList;
    ImGuiWidget::ImBorder* ImBorder_LeftTab;
    ImGuiWidget::ImBorder* ImBorder_MainWorkSpace;
    ImGuiWidget::ImBorder* ImBorder_Right;
    ImGuiWidget::ImScrollingTextList* ImScrollingTextList_LogList;
//----Gen Members End----
};
