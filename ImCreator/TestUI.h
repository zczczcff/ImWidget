#pragma once
#include "ImUserWidget.h"

    class TestUI : public ImGuiWidget::ImUserWidget {
    public:
        TestUI(const std::string& name);
        virtual void Init() override;

    protected:
        //----Gen Members Begin----
        // Auto-generated widget pointers
        ImGuiWidget::ImCanvasPanel* CanvasPanel_05851;
        ImGuiWidget::ImVerticalBox* VerticalBox_17652;
        ImGuiWidget::ImButton* Button_321953;
        ImGuiWidget::ImTextBlock* TextBlock_426154;
        ImGuiWidget::ImButton* Button_218555;
        //----Gen Members End----
    };
