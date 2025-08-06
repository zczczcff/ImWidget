#pragma once
#include "ImUserWidget.h"

    class Test1 : public ImGuiWidget::ImUserWidget {
    public:
        Test1(const std::string& name);
        virtual void Init() override;

    protected:
                                                                                                                                                                                                                                                                                                                //----Gen Members Begin----
        // Auto-generated widget pointers
        ImGuiWidget::ImVerticalBox* MainBox;
        ImGuiWidget::ImHorizontalBox* m_MenuList;
        ImGuiWidget::ImButton* MenuButton_Project;
        ImGuiWidget::ImTextBlock* Button_Project_Text;
        ImGuiWidget::ImButton* Button_NewUI;
        ImGuiWidget::ImTextBlock* NewUI_MenuText;
        ImGuiWidget::ImButton* Button_SaveUI;
        ImGuiWidget::ImTextBlock* SaveUI_MenuText;
        ImGuiWidget::ImButton* Button_GenCode;
        ImGuiWidget::ImTextBlock* GenCode_MenuText;
        ImGuiWidget::ImHorizontalSplitter* VSplitter0;
        ImGuiWidget::ImVerticalBox* MiddleBox;
        ImGuiWidget::ImHorizontalSplitter* Splitter0;
        ImGuiWidget::ImHorizontalSplitter* WidgetList_WidgetTreeSplitter;
        ImGuiWidget::ImVerticalBox* WidgetList;
        ImGuiWidget::ImWidget* Example_Button;
        ImGuiWidget::ImWidget* Example_TextBlock;
        ImGuiWidget::ImWidget* Example_Image;
        ImGuiWidget::ImWidget* Example_CanvasPanel;
        ImGuiWidget::ImWidget* Example_HorizontalBox;
        ImGuiWidget::ImWidget* Example_VerticalBox;
        ImGuiWidget::ImWidget* Example_ComboBox;
        ImGuiWidget::ImWidget* InputTextTest;
        ImGuiWidget::ImCheckBox* CheckBoxTest;
        ImGuiWidget::ImWidget* WidgetTreeView;
        ImGuiWidget::ImWidget* ImCreatorUIPageManager;
        ImGuiWidget::ImWidget* DetailList;
        ImGuiWidget::ImVerticalBox* BottomBox;
        ImGuiWidget::ImScrollingTextList* LogList;
        //----Gen Members End----
    };
