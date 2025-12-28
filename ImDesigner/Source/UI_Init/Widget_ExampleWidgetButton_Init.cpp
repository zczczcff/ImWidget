//----Gen Include Begin----
// Auto-generated includes
#include "Widget_ExampleWidgetButton.h"
#include "ImBasicWidgetList.h"
//----Gen Include End----
//----Gen Code Begin----
// Auto-generated initialization code
void Widget_ExampleWidgetButton::Init()
{
    ImButton_ExampleButton = new ImGuiWidget::ImButton("ImButton_ExampleButton");
    ImTextBlock_WidgetTypeName = new ImGuiWidget::ImTextBlock("ImTextBlock_WidgetTypeName");

    ImGuiWidget::PropertyStruct* ImButton_ExampleButton_PressedStyle = ImButton_ExampleButton->GetPropertyPtr<ImGuiWidget::PropertyStruct>("PressedStyle");
    ImButton_ExampleButton_PressedStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(136, 255, 206, 255));
    ImButton_ExampleButton_PressedStyle->SetPropertyValue<bool>("HasBorder", false);
    ImButton_ExampleButton_PressedStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    ImButton_ExampleButton_PressedStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    ImButton_ExampleButton_PressedStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));
    ImButton_ExampleButton->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    ImButton_ExampleButton->SetPropertyValue<std::string>("Name", "ImButton_ExampleButton");
    ImButton_ExampleButton->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    ImButton_ExampleButton->SetPropertyValue<bool>("HaveBorder", true);
    ImGuiWidget::PropertyStruct* ImButton_ExampleButton_NormalStyle = ImButton_ExampleButton->GetPropertyPtr<ImGuiWidget::PropertyStruct>("NormalStyle");
    ImButton_ExampleButton_NormalStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(255, 255, 255, 255));
    ImButton_ExampleButton_NormalStyle->SetPropertyValue<bool>("HasBorder", false);
    ImButton_ExampleButton_NormalStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    ImButton_ExampleButton_NormalStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    ImButton_ExampleButton_NormalStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));
    ImButton_ExampleButton->SetPropertyValue<std::string>("TooltipText", "");
    ImGuiWidget::PropertyStruct* ImButton_ExampleButton_HoveredStyle = ImButton_ExampleButton->GetPropertyPtr<ImGuiWidget::PropertyStruct>("HoveredStyle");
    ImButton_ExampleButton_HoveredStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(224, 253, 221, 255));
    ImButton_ExampleButton_HoveredStyle->SetPropertyValue<bool>("HasBorder", false);
    ImButton_ExampleButton_HoveredStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    ImButton_ExampleButton_HoveredStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    ImButton_ExampleButton_HoveredStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));

    ImTextBlock_WidgetTypeName->SetPropertyValue<int>("TextFontSize", 14);
    ImTextBlock_WidgetTypeName->SetPropertyValue<std::string>("Text", u8"控件");
    ImTextBlock_WidgetTypeName->SetPropertyValue<std::string>("Name", "ImTextBlock_WidgetTypeName");
    ImTextBlock_WidgetTypeName->SetPropertyValue<ImU32>("TextColor", IM_COL32(0, 0, 0, 255));
    ImTextBlock_WidgetTypeName->SetPropertyValue<std::string>("HorizontalAlignment", "Center");
    ImTextBlock_WidgetTypeName->SetPropertyValue<std::string>("VerticalAlignment", "Center");

    ImGuiWidget::ImSlot* ImButton_ExampleButton_slot0 = ImButton_ExampleButton->AddChild(ImTextBlock_WidgetTypeName);
    ImButton_ExampleButton_slot0->SetPropertyValue<bool>("bAutoSize", true);


    SetRootWidget(ImButton_ExampleButton);
}
//----Gen Code End----
