#pragma once
#include "ImUserWidget.h"

    class ./TestUI.imui : public ImGuiWidget::ImUserWidget {
    public:
        ./TestUI.imui(const std::string& name);
        virtual void Init() override;

    protected:
        //----Gen Members Begin----
        // Auto-generated widget pointers
        ImGuiWidget::ImCanvasPanel* CanvasPanel_0585153;
        ImGuiWidget::ImVerticalBox* VerticalBox_1765254;
        ImGuiWidget::ImButton* Button_32195355;
        ImGuiWidget::ImTextBlock* TextBlock_42615456;
        ImGuiWidget::ImButton* Button_21855557;
        //----Gen Members End----
    };
