//----Gen Include Begin----
// Auto-generated includes
#include "./TestUI.imui.h"
#include "ImUserWidget.h"
#include "ImButton.h"
#include "ImCanvasPanel.h"
#include "ImTextBlock.h"
#include "ImVerticalBox.h"
//----Gen Include End----

./TestUI.imui::./TestUI.imui(const std::string& name)
    : ImGuiWidget::ImUserWidget(name)
{
}

//----Gen Code Begin----
// Auto-generated initialization code
void ./TestUI.imui::Init()
{
    CanvasPanel_0585153 = new ImGuiWidget::ImCanvasPanel("CanvasPanel_0585153");
    VerticalBox_1765254 = new ImGuiWidget::ImVerticalBox("VerticalBox_1765254");
    Button_32195355 = new ImGuiWidget::ImButton("Button_32195355");
    TextBlock_42615456 = new ImGuiWidget::ImTextBlock("TextBlock_42615456");
    Button_21855557 = new ImGuiWidget::ImButton("Button_21855557");

    CanvasPanel_0585153->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    CanvasPanel_0585153->SetPropertyValue<bool>("HaveBorder", true);
    CanvasPanel_0585153->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));

    VerticalBox_1765254->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    VerticalBox_1765254->SetPropertyValue<bool>("HaveBorder", true);
    VerticalBox_1765254->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));

    Button_32195355->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    Button_32195355->SetPropertyValue<bool>("HaveBorder", true);
    ImGuiWidget::PropertyStruct* Button_32195355_PressedStyle = Button_32195355->GetPropertyPtr<ImGuiWidget::PropertyStruct>("PressedStyle");
    Button_32195355_PressedStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(0, 51, 153, 255));
    Button_32195355_PressedStyle->SetPropertyValue<bool>("HasBorder", false);
    Button_32195355_PressedStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    Button_32195355_PressedStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    Button_32195355_PressedStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));
    Button_32195355->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    ImGuiWidget::PropertyStruct* Button_32195355_NormalStyle = Button_32195355->GetPropertyPtr<ImGuiWidget::PropertyStruct>("NormalStyle");
    Button_32195355_NormalStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(0, 102, 204, 255));
    Button_32195355_NormalStyle->SetPropertyValue<bool>("HasBorder", false);
    Button_32195355_NormalStyle->SetPropertyValue<float>("Rounding", 10.000000f);
    Button_32195355_NormalStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    Button_32195355_NormalStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));
    Button_32195355->SetPropertyValue<std::string>("TooltipText", "");
    ImGuiWidget::PropertyStruct* Button_32195355_HoveredStyle = Button_32195355->GetPropertyPtr<ImGuiWidget::PropertyStruct>("HoveredStyle");
    Button_32195355_HoveredStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(51, 153, 255, 255));
    Button_32195355_HoveredStyle->SetPropertyValue<bool>("HasBorder", false);
    Button_32195355_HoveredStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    Button_32195355_HoveredStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    Button_32195355_HoveredStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));

    TextBlock_42615456->SetPropertyValue<std::string>("Text", "test1");
    TextBlock_42615456->SetPropertyValue<ImU32>("TextColor", IM_COL32(0, 0, 0, 255));
    TextBlock_42615456->SetPropertyValue<UnknownType>("HorizontalAlignment", "Center");
    TextBlock_42615456->SetPropertyValue<UnknownType>("VerticalAlignment", "Center");

    Button_21855557->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    Button_21855557->SetPropertyValue<bool>("HaveBorder", true);
    ImGuiWidget::PropertyStruct* Button_21855557_PressedStyle = Button_21855557->GetPropertyPtr<ImGuiWidget::PropertyStruct>("PressedStyle");
    Button_21855557_PressedStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(0, 51, 153, 255));
    Button_21855557_PressedStyle->SetPropertyValue<bool>("HasBorder", false);
    Button_21855557_PressedStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    Button_21855557_PressedStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    Button_21855557_PressedStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));
    Button_21855557->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    ImGuiWidget::PropertyStruct* Button_21855557_NormalStyle = Button_21855557->GetPropertyPtr<ImGuiWidget::PropertyStruct>("NormalStyle");
    Button_21855557_NormalStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(0, 102, 204, 255));
    Button_21855557_NormalStyle->SetPropertyValue<bool>("HasBorder", false);
    Button_21855557_NormalStyle->SetPropertyValue<float>("Rounding", 10.000000f);
    Button_21855557_NormalStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    Button_21855557_NormalStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));
    Button_21855557->SetPropertyValue<std::string>("TooltipText", "");
    ImGuiWidget::PropertyStruct* Button_21855557_HoveredStyle = Button_21855557->GetPropertyPtr<ImGuiWidget::PropertyStruct>("HoveredStyle");
    Button_21855557_HoveredStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(51, 153, 255, 255));
    Button_21855557_HoveredStyle->SetPropertyValue<bool>("HasBorder", false);
    Button_21855557_HoveredStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    Button_21855557_HoveredStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    Button_21855557_HoveredStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));

    CanvasPanel_0585153->AddChild(VerticalBox_1765254);
    VerticalBox_1765254->AddChild(Button_32195355);
    VerticalBox_1765254->AddChild(Button_21855557);
    Button_32195355->AddChild(TextBlock_42615456);

    SetRootWidget(CanvasPanel_0585153);
}
//----Gen Code End----
