//----Gen Include Begin----
// Auto-generated includes
#include "TestUI.h"
#include "ImUserWidget.h"
#include "ImButton.h"
#include "ImCanvasPanel.h"
#include "ImTextBlock.h"
#include "ImVerticalBox.h"
//----Gen Include End----

TestUI::TestUI(const std::string& name)
    : ImGuiWidget::ImUserWidget(name)
{
}

//----Gen Code Begin----
// Auto-generated initialization code
void TestUI::Init()
{
    CanvasPanel_05851 = new ImGuiWidget::ImCanvasPanel("CanvasPanel_05851");
    VerticalBox_17652 = new ImGuiWidget::ImVerticalBox("VerticalBox_17652");
    Button_321953 = new ImGuiWidget::ImButton("Button_321953");
    TextBlock_426154 = new ImGuiWidget::ImTextBlock("TextBlock_426154");
    Button_218555 = new ImGuiWidget::ImButton("Button_218555");

    CanvasPanel_05851->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    CanvasPanel_05851->SetPropertyValue<std::string>("Name", "CanvasPanel_05851");
    CanvasPanel_05851->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    CanvasPanel_05851->SetPropertyValue<bool>("HaveBorder", true);

    VerticalBox_17652->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    VerticalBox_17652->SetPropertyValue<std::string>("Name", "VerticalBox_17652");
    VerticalBox_17652->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    VerticalBox_17652->SetPropertyValue<bool>("HaveBorder", true);

    ImGuiWidget::PropertyStruct* Button_321953_PressedStyle = Button_321953->GetPropertyPtr<ImGuiWidget::PropertyStruct>("PressedStyle");
    Button_321953_PressedStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(0, 51, 153, 255));
    Button_321953_PressedStyle->SetPropertyValue<bool>("HasBorder", false);
    Button_321953_PressedStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    Button_321953_PressedStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    Button_321953_PressedStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));
    Button_321953->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    Button_321953->SetPropertyValue<std::string>("Name", "Button_321953");
    Button_321953->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    Button_321953->SetPropertyValue<bool>("HaveBorder", true);
    ImGuiWidget::PropertyStruct* Button_321953_NormalStyle = Button_321953->GetPropertyPtr<ImGuiWidget::PropertyStruct>("NormalStyle");
    Button_321953_NormalStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(0, 102, 204, 255));
    Button_321953_NormalStyle->SetPropertyValue<bool>("HasBorder", false);
    Button_321953_NormalStyle->SetPropertyValue<float>("Rounding", 10.000000f);
    Button_321953_NormalStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    Button_321953_NormalStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));
    Button_321953->SetPropertyValue<std::string>("TooltipText", "");
    ImGuiWidget::PropertyStruct* Button_321953_HoveredStyle = Button_321953->GetPropertyPtr<ImGuiWidget::PropertyStruct>("HoveredStyle");
    Button_321953_HoveredStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(51, 153, 255, 255));
    Button_321953_HoveredStyle->SetPropertyValue<bool>("HasBorder", false);
    Button_321953_HoveredStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    Button_321953_HoveredStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    Button_321953_HoveredStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));

    TextBlock_426154->SetPropertyValue<std::string>("Text", "test1");
    TextBlock_426154->SetPropertyValue<std::string>("Name", "TextBlock_426154");
    TextBlock_426154->SetPropertyValue<ImU32>("TextColor", IM_COL32(0, 0, 0, 255));
    TextBlock_426154->SetPropertyValue<UnknownType>("HorizontalAlignment", "Center");
    TextBlock_426154->SetPropertyValue<UnknownType>("VerticalAlignment", "Center");

    ImGuiWidget::PropertyStruct* Button_218555_PressedStyle = Button_218555->GetPropertyPtr<ImGuiWidget::PropertyStruct>("PressedStyle");
    Button_218555_PressedStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(0, 51, 153, 255));
    Button_218555_PressedStyle->SetPropertyValue<bool>("HasBorder", false);
    Button_218555_PressedStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    Button_218555_PressedStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    Button_218555_PressedStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));
    Button_218555->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    Button_218555->SetPropertyValue<std::string>("Name", "Button_218555");
    Button_218555->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    Button_218555->SetPropertyValue<bool>("HaveBorder", true);
    ImGuiWidget::PropertyStruct* Button_218555_NormalStyle = Button_218555->GetPropertyPtr<ImGuiWidget::PropertyStruct>("NormalStyle");
    Button_218555_NormalStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(0, 102, 204, 255));
    Button_218555_NormalStyle->SetPropertyValue<bool>("HasBorder", false);
    Button_218555_NormalStyle->SetPropertyValue<float>("Rounding", 10.000000f);
    Button_218555_NormalStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    Button_218555_NormalStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));
    Button_218555->SetPropertyValue<std::string>("TooltipText", "");
    ImGuiWidget::PropertyStruct* Button_218555_HoveredStyle = Button_218555->GetPropertyPtr<ImGuiWidget::PropertyStruct>("HoveredStyle");
    Button_218555_HoveredStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(51, 153, 255, 255));
    Button_218555_HoveredStyle->SetPropertyValue<bool>("HasBorder", false);
    Button_218555_HoveredStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    Button_218555_HoveredStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    Button_218555_HoveredStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));

    CanvasPanel_05851->AddChild(VerticalBox_17652);
    VerticalBox_17652->AddChild(Button_321953);
    VerticalBox_17652->AddChild(Button_218555);
    Button_321953->AddChild(TextBlock_426154);

    SetRootWidget(CanvasPanel_05851);
}
//----Gen Code End----
