//----Gen Include Begin----
// Auto-generated includes
#include "test2.imui.h"
#include "ImUserWidget.h"
#include "ImButton.h"
#include "ImCanvasPanel.h"
#include "ImHorizontalBox.h"
#include "ImTextBlock.h"
//----Gen Include End----

test2.imui::test2.imui(const std::string& name)
    : ImGuiWidget::ImUserWidget(name)
{
}

//----Gen Code Begin----
// Auto-generated initialization code
void test2.imui::Init()
{
    MainCanvas = new ImGuiWidget::ImCanvasPanel("MainCanvas");
    HorizontalBox_1 = new ImGuiWidget::ImHorizontalBox("HorizontalBox_1");
    Button_2 = new ImGuiWidget::ImButton("Button_2");
    TextBlockbutton = new ImGuiWidget::ImTextBlock("TextBlockbutton");

    MainCanvas->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    MainCanvas->SetPropertyValue<std::string>("Name", "MainCanvas");
    MainCanvas->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    MainCanvas->SetPropertyValue<bool>("HaveBorder", true);

    HorizontalBox_1->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    HorizontalBox_1->SetPropertyValue<std::string>("Name", "HorizontalBox_1");
    HorizontalBox_1->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    HorizontalBox_1->SetPropertyValue<bool>("HaveBorder", true);

    ImGuiWidget::PropertyStruct* Button_2_PressedStyle = Button_2->GetPropertyPtr<ImGuiWidget::PropertyStruct>("PressedStyle");
    Button_2_PressedStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(0, 51, 153, 255));
    Button_2_PressedStyle->SetPropertyValue<bool>("HasBorder", false);
    Button_2_PressedStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    Button_2_PressedStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    Button_2_PressedStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));
    Button_2->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    Button_2->SetPropertyValue<std::string>("Name", "Button_2");
    Button_2->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    Button_2->SetPropertyValue<bool>("HaveBorder", true);
    ImGuiWidget::PropertyStruct* Button_2_NormalStyle = Button_2->GetPropertyPtr<ImGuiWidget::PropertyStruct>("NormalStyle");
    Button_2_NormalStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(0, 102, 204, 255));
    Button_2_NormalStyle->SetPropertyValue<bool>("HasBorder", false);
    Button_2_NormalStyle->SetPropertyValue<float>("Rounding", 10.000000f);
    Button_2_NormalStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    Button_2_NormalStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));
    Button_2->SetPropertyValue<std::string>("TooltipText", "");
    ImGuiWidget::PropertyStruct* Button_2_HoveredStyle = Button_2->GetPropertyPtr<ImGuiWidget::PropertyStruct>("HoveredStyle");
    Button_2_HoveredStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(51, 153, 255, 255));
    Button_2_HoveredStyle->SetPropertyValue<bool>("HasBorder", false);
    Button_2_HoveredStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    Button_2_HoveredStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    Button_2_HoveredStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));

    TextBlockbutton->SetPropertyValue<std::string>("Text", "");
    TextBlockbutton->SetPropertyValue<std::string>("Name", "TextBlockbutton");
    TextBlockbutton->SetPropertyValue<ImU32>("TextColor", IM_COL32(0, 0, 0, 255));
    TextBlockbutton->SetPropertyValue<UnknownType>("HorizontalAlignment", "Center");
    TextBlockbutton->SetPropertyValue<UnknownType>("VerticalAlignment", "Center");

    MainCanvas->AddChild(HorizontalBox_1);
    HorizontalBox_1->AddChild(Button_2);
    Button_2->AddChild(TextBlockbutton);

    SetRootWidget(MainCanvas);
}
//----Gen Code End----
