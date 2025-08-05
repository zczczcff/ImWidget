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
        ImGuiWidget::ImHorizontalBox* m_MenuList30;
        ImGuiWidget::ImButton* MenuButton_Project31;
        ImGuiWidget::ImTextBlock* Button_Project_Text33;
        ImGuiWidget::ImButton* Button_NewUI34;
        ImGuiWidget::ImTextBlock* NewUI_MenuText36;
        ImGuiWidget::ImButton* Button_SaveUI41;
        ImGuiWidget::ImTextBlock* SaveUI_MenuText42;
        ImGuiWidget::ImHorizontalSplitter* VSplitter02;
        ImGuiWidget::ImVerticalBox* MiddleBox4;
        ImGuiWidget::ImHorizontalSplitter* Splitter01;
        ImGuiWidget::ImHorizontalSplitter* WidgetList_WidgetTreeSplitter5;
        ImGuiWidget::ImVerticalBox* WidgetList6;
        ImGuiWidget::ImWidget* Example_Button12;
        ImGuiWidget::ImWidget* Example_TextBlock14;
        ImGuiWidget::ImWidget* Example_Image16;
        ImGuiWidget::ImWidget* Example_CanvasPanel18;
        ImGuiWidget::ImWidget* Example_HorizontalBox20;
        ImGuiWidget::ImWidget* Example_VerticalBox22;
        ImGuiWidget::ImWidget* Example_ComboBox24;
        ImGuiWidget::ImWidget* InputTextTest26;
        ImGuiWidget::ImCheckBox* CheckBoxTest27;
        ImGuiWidget::ImWidget* WidgetTreeView7;
        ImGuiWidget::ImWidget* ImCreatorUIPageManager9;
        ImGuiWidget::ImWidget* DetailList8;
        ImGuiWidget::ImVerticalBox* BottomBox28;
        ImGuiWidget::ImScrollingTextList* LogList29;
        //----Gen Members End----
    };
