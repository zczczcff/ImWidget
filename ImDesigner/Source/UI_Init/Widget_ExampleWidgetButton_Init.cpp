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
    ImHorizontalBox_0 = new ImGuiWidget::ImHorizontalBox("ImHorizontalBox_0");
    ImImage_Icon = new ImGuiWidget::ImImage("ImImage_Icon");
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

    ImHorizontalBox_0->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    ImHorizontalBox_0->SetPropertyValue<std::string>("Name", "ImHorizontalBox_0");
    ImHorizontalBox_0->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 0));
    ImHorizontalBox_0->SetPropertyValue<bool>("HaveBorder", false);

    ImImage_Icon->SetPropertyValue<std::string>("Name", "ImImage_Icon");

    ImTextBlock_WidgetTypeName->SetPropertyValue<int>("TextFontSize", 14);
    ImTextBlock_WidgetTypeName->SetPropertyValue<std::string>("Text", "");
    ImTextBlock_WidgetTypeName->SetPropertyValue<std::string>("Name", "ImTextBlock_WidgetTypeName");
    ImTextBlock_WidgetTypeName->SetPropertyValue<ImU32>("TextColor", IM_COL32(0, 0, 0, 255));
    ImTextBlock_WidgetTypeName->SetPropertyValue<std::string>("HorizontalAlignment", "Left");
    ImTextBlock_WidgetTypeName->SetPropertyValue<std::string>("VerticalAlignment", "Center");

    ImGuiWidget::ImSlot* ImButton_ExampleButton_slot0 = ImButton_ExampleButton->AddChild(ImHorizontalBox_0);
    ImButton_ExampleButton_slot0->SetPropertyValue<bool>("bAutoSize", true);

    ImGuiWidget::ImSlot* ImHorizontalBox_0_slot0 = ImHorizontalBox_0->AddChild(ImImage_Icon);
    ImHorizontalBox_0_slot0->SetPropertyValue<float>("SizeRatio", 1.000000f);
    ImHorizontalBox_0_slot0->SetPropertyValue<bool>("bAutoSize", false);
    ImHorizontalBox_0_slot0->SetPropertyValue<float>("PaddingTop", 0.000000f);
    ImHorizontalBox_0_slot0->SetPropertyValue<float>("PaddingBottom", 0.000000f);
    ImHorizontalBox_0_slot0->SetPropertyValue<float>("PaddingLeft", 4.000000f);
    ImHorizontalBox_0_slot0->SetPropertyValue<float>("PaddingRight", 8.000000f);

    ImGuiWidget::ImSlot* ImHorizontalBox_0_slot1 = ImHorizontalBox_0->AddChild(ImTextBlock_WidgetTypeName);
    ImHorizontalBox_0_slot1->SetPropertyValue<float>("SizeRatio", 1.000000f);
    ImHorizontalBox_0_slot1->SetPropertyValue<bool>("bAutoSize", true);
    ImHorizontalBox_0_slot1->SetPropertyValue<float>("PaddingTop", 0.000000f);
    ImHorizontalBox_0_slot1->SetPropertyValue<float>("PaddingBottom", 0.000000f);
    ImHorizontalBox_0_slot1->SetPropertyValue<float>("PaddingLeft", 0.000000f);
    ImHorizontalBox_0_slot1->SetPropertyValue<float>("PaddingRight", 0.000000f);


    SetRootWidget(ImButton_ExampleButton);
}
//----Gen Code End----
