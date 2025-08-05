#pragma once
#include "ImUserWidget.h"

    class Test1 : public ImGuiWidget::ImUserWidget {
    public:
        Test1(const std::string& name);
        virtual void Init() override;

    protected:
                                                        //----Gen Members Begin----
        // Auto-generated widget pointers
        ImGuiWidget::ImVerticalBox* MainBox3;
        ImGuiWidget::ImHorizontalBox* m_MenuList36;
        ImGuiWidget::ImButton* MenuButton_Project37;
        ImGuiWidget::ImTextBlock* Button_Project_Text43;
        ImGuiWidget::ImHorizontalSplitter* VSplitter02;
        ImGuiWidget::ImVerticalBox* MiddleBox4;
        ImGuiWidget::ImHorizontalSplitter* Splitter01;
        ImGuiWidget::ImHorizontalSplitter* WidgetList_WidgetTreeSplitter5;
        ImGuiWidget::ImVerticalBox* WidgetList6;
        ImGuiWidget::ImWidget* Example_Button18;
        ImGuiWidget::ImWidget* Example_TextBlock20;
        ImGuiWidget::ImWidget* Example_Image22;
        ImGuiWidget::ImWidget* Example_CanvasPanel24;
        ImGuiWidget::ImWidget* Example_HorizontalBox26;
        ImGuiWidget::ImWidget* Example_VerticalBox28;
        ImGuiWidget::ImWidget* Example_ComboBox30;
        ImGuiWidget::ImWidget* InputTextTest32;
        ImGuiWidget::ImCheckBox* CheckBoxTest33;
        ImGuiWidget::ImWidget* WidgetTreeView7;
        ImGuiWidget::ImPanelWidget* CenterPageManager12;
        ImGuiWidget::ImHorizontalBox* TabBar13;
        ImGuiWidget::ImButton* TabButton_test17;
        ImGuiWidget::ImWidget* TabItem_test14;
        ImGuiWidget::ImWidget* DetailList10;
        ImGuiWidget::ImVerticalBox* BottomBox34;
        ImGuiWidget::ImScrollingTextList* LogList35;
        //----Gen Members End----
    };
