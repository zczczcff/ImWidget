//----Gen Include Begin----
// Auto-generated includes
#include "Widget_PageTag.h"
#include "ImBasicWidgetList.h"
//----Gen Include End----
//----Gen Code Begin----
// Auto-generated initialization code
void Widget_PageTag::Init()
{
    ImButton_PageButton = new ImGuiWidget::ImButton("ImButton_PageButton");
    ImHorizontalBox_2 = new ImGuiWidget::ImHorizontalBox("ImHorizontalBox_2");
    ImTextBlock_PageName = new ImGuiWidget::ImTextBlock("ImTextBlock_PageName");
    ImButton_Close = new ImGuiWidget::ImButton("ImButton_Close");
    ImTextBlock_6 = new ImGuiWidget::ImTextBlock("ImTextBlock_6");

    ImGuiWidget::PropertyStruct* ImButton_PageButton_PressedStyle = ImButton_PageButton->GetPropertyPtr<ImGuiWidget::PropertyStruct>("PressedStyle");
    ImButton_PageButton_PressedStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(200, 220, 255, 255));
    ImButton_PageButton_PressedStyle->SetPropertyValue<bool>("HasBorder", false);
    ImButton_PageButton_PressedStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    ImButton_PageButton_PressedStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    ImButton_PageButton_PressedStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));
    ImButton_PageButton->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    ImButton_PageButton->SetPropertyValue<std::string>("Name", "ImButton_PageButton");
    ImButton_PageButton->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    ImButton_PageButton->SetPropertyValue<bool>("HaveBorder", true);
    ImGuiWidget::PropertyStruct* ImButton_PageButton_NormalStyle = ImButton_PageButton->GetPropertyPtr<ImGuiWidget::PropertyStruct>("NormalStyle");
    ImButton_PageButton_NormalStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(240, 245, 255, 255));
    ImButton_PageButton_NormalStyle->SetPropertyValue<bool>("HasBorder", false);
    ImButton_PageButton_NormalStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    ImButton_PageButton_NormalStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    ImButton_PageButton_NormalStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));
    ImButton_PageButton->SetPropertyValue<std::string>("TooltipText", "");
    ImGuiWidget::PropertyStruct* ImButton_PageButton_HoveredStyle = ImButton_PageButton->GetPropertyPtr<ImGuiWidget::PropertyStruct>("HoveredStyle");
    ImButton_PageButton_HoveredStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(225, 235, 255, 255));
    ImButton_PageButton_HoveredStyle->SetPropertyValue<bool>("HasBorder", false);
    ImButton_PageButton_HoveredStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    ImButton_PageButton_HoveredStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    ImButton_PageButton_HoveredStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));

    ImHorizontalBox_2->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    ImHorizontalBox_2->SetPropertyValue<std::string>("Name", "ImHorizontalBox_2");
    ImHorizontalBox_2->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 0));
    ImHorizontalBox_2->SetPropertyValue<bool>("HaveBorder", true);

    ImTextBlock_PageName->SetPropertyValue<int>("TextFontSize", 14);
    ImTextBlock_PageName->SetPropertyValue<std::string>("Text", "Page");
    ImTextBlock_PageName->SetPropertyValue<std::string>("Name", "ImTextBlock_PageName");
    ImTextBlock_PageName->SetPropertyValue<ImU32>("TextColor", IM_COL32(0, 0, 0, 255));
    ImTextBlock_PageName->SetPropertyValue<std::string>("HorizontalAlignment", "Center");
    ImTextBlock_PageName->SetPropertyValue<std::string>("VerticalAlignment", "Center");

    ImGuiWidget::PropertyStruct* ImButton_Close_PressedStyle = ImButton_Close->GetPropertyPtr<ImGuiWidget::PropertyStruct>("PressedStyle");
    ImButton_Close_PressedStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(224, 90, 90, 255));
    ImButton_Close_PressedStyle->SetPropertyValue<bool>("HasBorder", false);
    ImButton_Close_PressedStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    ImButton_Close_PressedStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    ImButton_Close_PressedStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));
    ImButton_Close->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    ImButton_Close->SetPropertyValue<std::string>("Name", "ImButton_Close");
    ImButton_Close->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    ImButton_Close->SetPropertyValue<bool>("HaveBorder", false);
    ImGuiWidget::PropertyStruct* ImButton_Close_NormalStyle = ImButton_Close->GetPropertyPtr<ImGuiWidget::PropertyStruct>("NormalStyle");
    ImButton_Close_NormalStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(136, 136, 136, 255));
    ImButton_Close_NormalStyle->SetPropertyValue<bool>("HasBorder", false);
    ImButton_Close_NormalStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    ImButton_Close_NormalStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    ImButton_Close_NormalStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));
    ImButton_Close->SetPropertyValue<std::string>("TooltipText", "");
    ImGuiWidget::PropertyStruct* ImButton_Close_HoveredStyle = ImButton_Close->GetPropertyPtr<ImGuiWidget::PropertyStruct>("HoveredStyle");
    ImButton_Close_HoveredStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(255, 107, 107, 255));
    ImButton_Close_HoveredStyle->SetPropertyValue<bool>("HasBorder", false);
    ImButton_Close_HoveredStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    ImButton_Close_HoveredStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    ImButton_Close_HoveredStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));

    ImTextBlock_6->SetPropertyValue<int>("TextFontSize", 14);
    ImTextBlock_6->SetPropertyValue<std::string>("Text", u8"×");
    ImTextBlock_6->SetPropertyValue<std::string>("Name", "ImTextBlock_6");
    ImTextBlock_6->SetPropertyValue<ImU32>("TextColor", IM_COL32(0, 0, 0, 255));
    ImTextBlock_6->SetPropertyValue<std::string>("HorizontalAlignment", "Center");
    ImTextBlock_6->SetPropertyValue<std::string>("VerticalAlignment", "Center");

    ImGuiWidget::ImSlot* ImButton_PageButton_slot0 = ImButton_PageButton->AddChild(ImHorizontalBox_2);
    ImButton_PageButton_slot0->SetPropertyValue<bool>("bAutoSize", true);

    ImGuiWidget::ImSlot* ImHorizontalBox_2_slot0 = ImHorizontalBox_2->AddChild(ImTextBlock_PageName);
    ImHorizontalBox_2_slot0->SetPropertyValue<float>("SizeRatio", 1.000000f);
    ImHorizontalBox_2_slot0->SetPropertyValue<bool>("bAutoSize", true);
    ImHorizontalBox_2_slot0->SetPropertyValue<float>("PaddingTop", 5.000000f);
    ImHorizontalBox_2_slot0->SetPropertyValue<float>("PaddingBottom", 5.000000f);
    ImHorizontalBox_2_slot0->SetPropertyValue<float>("PaddingLeft", 1.000000f);
    ImHorizontalBox_2_slot0->SetPropertyValue<float>("PaddingRight", 1.000000f);

    ImGuiWidget::ImSlot* ImHorizontalBox_2_slot1 = ImHorizontalBox_2->AddChild(ImButton_Close);
    ImHorizontalBox_2_slot1->SetPropertyValue<float>("SizeRatio", 1.000000f);
    ImHorizontalBox_2_slot1->SetPropertyValue<bool>("bAutoSize", false);
    ImHorizontalBox_2_slot1->SetPropertyValue<float>("PaddingTop", 4.000000f);
    ImHorizontalBox_2_slot1->SetPropertyValue<float>("PaddingBottom", 4.000000f);
    ImHorizontalBox_2_slot1->SetPropertyValue<float>("PaddingLeft", 0.000000f);
    ImHorizontalBox_2_slot1->SetPropertyValue<float>("PaddingRight", 4.000000f);

    ImGuiWidget::ImSlot* ImButton_Close_slot0 = ImButton_Close->AddChild(ImTextBlock_6);
    ImButton_Close_slot0->SetPropertyValue<bool>("bAutoSize", true);


    SetRootWidget(ImButton_PageButton);
}
//----Gen Code End----

void Widget_PageTag::Init2()
{
    ImButton_PageButton->SetFocusable(false);
    ImButton_Close->SetFocusable(false);
}