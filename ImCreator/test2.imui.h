#pragma once
#include "ImUserWidget.h"

    class test2.imui : public ImGuiWidget::ImUserWidget {
    public:
        test2.imui(const std::string& name);
        virtual void Init() override;

    protected:
        //----Gen Members Begin----
        // Auto-generated widget pointers
        ImGuiWidget::ImCanvasPanel* MainCanvas;
        ImGuiWidget::ImHorizontalBox* HorizontalBox_1;
        ImGuiWidget::ImButton* Button_2;
        ImGuiWidget::ImTextBlock* TextBlockbutton;
        //----Gen Members End----
    };
