//----Gen Include Begin----
// Auto-generated includes
#include "MainUI.h"
#include "ImBasicWidgetList.h"
//----Gen Include End----
//----Gen Code Begin----
// Auto-generated initialization code
void MainUI::Init()
{
    ImVerticalBox_3 = new ImGuiWidget::ImVerticalBox("ImVerticalBox_3");
    ImHorizontalBox_4 = new ImGuiWidget::ImHorizontalBox("ImHorizontalBox_4");
    ImButton_Project = new ImGuiWidget::ImButton("ImButton_Project");
    ImTextBlock_9 = new ImGuiWidget::ImTextBlock("ImTextBlock_9");
    ImButton_Save = new ImGuiWidget::ImButton("ImButton_Save");
    ImTextBlock_10 = new ImGuiWidget::ImTextBlock("ImTextBlock_10");
    ImButton_Generate = new ImGuiWidget::ImButton("ImButton_Generate");
    ImTextBlock_11 = new ImGuiWidget::ImTextBlock("ImTextBlock_11");
    ImVerticalSplitter_6 = new ImGuiWidget::ImVerticalSplitter("ImVerticalSplitter_6");
    ImHorizontalSplitter_1 = new ImGuiWidget::ImHorizontalSplitter("ImHorizontalSplitter_1");
    ImBorder_Left = new ImGuiWidget::ImBorder("ImBorder_Left");
    ImVerticalSplitter_0 = new ImGuiWidget::ImVerticalSplitter("ImVerticalSplitter_0");
    ImScrollBox_WidgetList = new ImGuiWidget::ImScrollBox("ImScrollBox_WidgetList");
    ImVerticalBox_WidgetList = new ImGuiWidget::ImVerticalBox("ImVerticalBox_WidgetList");
    ImScrollBox_WidgetTreeView = new ImGuiWidget::ImScrollBox("ImScrollBox_WidgetTreeView");
    ImBorder_LeftTab = new ImGuiWidget::ImBorder("ImBorder_LeftTab");
    ImBorder_MainWorkSpace = new ImGuiWidget::ImBorder("ImBorder_MainWorkSpace");
    ImVerticalBox_7 = new ImGuiWidget::ImVerticalBox("ImVerticalBox_7");
    ImHorizontalBox_PageTag = new ImGuiWidget::ImHorizontalBox("ImHorizontalBox_PageTag");
    ImBorder_CurrentPage = new ImGuiWidget::ImBorder("ImBorder_CurrentPage");
    ImBorder_Right = new ImGuiWidget::ImBorder("ImBorder_Right");
    ImScrollingTextList_LogList = new ImGuiWidget::ImScrollingTextList("ImScrollingTextList_LogList");

    ImVerticalBox_3->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    ImVerticalBox_3->SetPropertyValue<std::string>("Name", "ImVerticalBox_3");
    ImVerticalBox_3->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    ImVerticalBox_3->SetPropertyValue<bool>("HaveBorder", true);

    ImHorizontalBox_4->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    ImHorizontalBox_4->SetPropertyValue<std::string>("Name", "ImHorizontalBox_4");
    ImHorizontalBox_4->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(247, 247, 247, 255));
    ImHorizontalBox_4->SetPropertyValue<bool>("HaveBorder", true);

    ImGuiWidget::PropertyStruct* ImButton_Project_PressedStyle = ImButton_Project->GetPropertyPtr<ImGuiWidget::PropertyStruct>("PressedStyle");
    ImButton_Project_PressedStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(255, 255, 255, 255));
    ImButton_Project_PressedStyle->SetPropertyValue<bool>("HasBorder", false);
    ImButton_Project_PressedStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    ImButton_Project_PressedStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    ImButton_Project_PressedStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));
    ImButton_Project->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    ImButton_Project->SetPropertyValue<std::string>("Name", "ImButton_Project");
    ImButton_Project->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    ImButton_Project->SetPropertyValue<bool>("HaveBorder", false);
    ImGuiWidget::PropertyStruct* ImButton_Project_NormalStyle = ImButton_Project->GetPropertyPtr<ImGuiWidget::PropertyStruct>("NormalStyle");
    ImButton_Project_NormalStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(247, 247, 247, 255));
    ImButton_Project_NormalStyle->SetPropertyValue<bool>("HasBorder", false);
    ImButton_Project_NormalStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    ImButton_Project_NormalStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    ImButton_Project_NormalStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));
    ImButton_Project->SetPropertyValue<std::string>("TooltipText", "");
    ImGuiWidget::PropertyStruct* ImButton_Project_HoveredStyle = ImButton_Project->GetPropertyPtr<ImGuiWidget::PropertyStruct>("HoveredStyle");
    ImButton_Project_HoveredStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(198, 215, 255, 255));
    ImButton_Project_HoveredStyle->SetPropertyValue<bool>("HasBorder", false);
    ImButton_Project_HoveredStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    ImButton_Project_HoveredStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    ImButton_Project_HoveredStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));

    ImTextBlock_9->SetPropertyValue<int>("TextFontSize", 14);
    ImTextBlock_9->SetPropertyValue<std::string>("Text", u8"项目");
    ImTextBlock_9->SetPropertyValue<std::string>("Name", "ImTextBlock_9");
    ImTextBlock_9->SetPropertyValue<ImU32>("TextColor", IM_COL32(0, 0, 0, 255));
    ImTextBlock_9->SetPropertyValue<std::string>("HorizontalAlignment", "Center");
    ImTextBlock_9->SetPropertyValue<std::string>("VerticalAlignment", "Center");

    ImGuiWidget::PropertyStruct* ImButton_Save_PressedStyle = ImButton_Save->GetPropertyPtr<ImGuiWidget::PropertyStruct>("PressedStyle");
    ImButton_Save_PressedStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(255, 255, 255, 255));
    ImButton_Save_PressedStyle->SetPropertyValue<bool>("HasBorder", false);
    ImButton_Save_PressedStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    ImButton_Save_PressedStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    ImButton_Save_PressedStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));
    ImButton_Save->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    ImButton_Save->SetPropertyValue<std::string>("Name", "ImButton_Save");
    ImButton_Save->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    ImButton_Save->SetPropertyValue<bool>("HaveBorder", false);
    ImGuiWidget::PropertyStruct* ImButton_Save_NormalStyle = ImButton_Save->GetPropertyPtr<ImGuiWidget::PropertyStruct>("NormalStyle");
    ImButton_Save_NormalStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(247, 247, 247, 255));
    ImButton_Save_NormalStyle->SetPropertyValue<bool>("HasBorder", false);
    ImButton_Save_NormalStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    ImButton_Save_NormalStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    ImButton_Save_NormalStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));
    ImButton_Save->SetPropertyValue<std::string>("TooltipText", "");
    ImGuiWidget::PropertyStruct* ImButton_Save_HoveredStyle = ImButton_Save->GetPropertyPtr<ImGuiWidget::PropertyStruct>("HoveredStyle");
    ImButton_Save_HoveredStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(198, 215, 255, 255));
    ImButton_Save_HoveredStyle->SetPropertyValue<bool>("HasBorder", false);
    ImButton_Save_HoveredStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    ImButton_Save_HoveredStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    ImButton_Save_HoveredStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));

    ImTextBlock_10->SetPropertyValue<int>("TextFontSize", 14);
    ImTextBlock_10->SetPropertyValue<std::string>("Text", u8"保存");
    ImTextBlock_10->SetPropertyValue<std::string>("Name", "ImTextBlock_10");
    ImTextBlock_10->SetPropertyValue<ImU32>("TextColor", IM_COL32(0, 0, 0, 255));
    ImTextBlock_10->SetPropertyValue<std::string>("HorizontalAlignment", "Center");
    ImTextBlock_10->SetPropertyValue<std::string>("VerticalAlignment", "Center");

    ImGuiWidget::PropertyStruct* ImButton_Generate_PressedStyle = ImButton_Generate->GetPropertyPtr<ImGuiWidget::PropertyStruct>("PressedStyle");
    ImButton_Generate_PressedStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(255, 255, 255, 255));
    ImButton_Generate_PressedStyle->SetPropertyValue<bool>("HasBorder", false);
    ImButton_Generate_PressedStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    ImButton_Generate_PressedStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    ImButton_Generate_PressedStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));
    ImButton_Generate->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    ImButton_Generate->SetPropertyValue<std::string>("Name", "ImButton_Generate");
    ImButton_Generate->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    ImButton_Generate->SetPropertyValue<bool>("HaveBorder", false);
    ImGuiWidget::PropertyStruct* ImButton_Generate_NormalStyle = ImButton_Generate->GetPropertyPtr<ImGuiWidget::PropertyStruct>("NormalStyle");
    ImButton_Generate_NormalStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(247, 247, 247, 255));
    ImButton_Generate_NormalStyle->SetPropertyValue<bool>("HasBorder", false);
    ImButton_Generate_NormalStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    ImButton_Generate_NormalStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    ImButton_Generate_NormalStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));
    ImButton_Generate->SetPropertyValue<std::string>("TooltipText", "");
    ImGuiWidget::PropertyStruct* ImButton_Generate_HoveredStyle = ImButton_Generate->GetPropertyPtr<ImGuiWidget::PropertyStruct>("HoveredStyle");
    ImButton_Generate_HoveredStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(198, 215, 255, 255));
    ImButton_Generate_HoveredStyle->SetPropertyValue<bool>("HasBorder", false);
    ImButton_Generate_HoveredStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    ImButton_Generate_HoveredStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    ImButton_Generate_HoveredStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));

    ImTextBlock_11->SetPropertyValue<int>("TextFontSize", 14);
    ImTextBlock_11->SetPropertyValue<std::string>("Text", u8"生成");
    ImTextBlock_11->SetPropertyValue<std::string>("Name", "ImTextBlock_11");
    ImTextBlock_11->SetPropertyValue<ImU32>("TextColor", IM_COL32(0, 0, 0, 255));
    ImTextBlock_11->SetPropertyValue<std::string>("HorizontalAlignment", "Center");
    ImTextBlock_11->SetPropertyValue<std::string>("VerticalAlignment", "Center");

    ImVerticalSplitter_6->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    ImVerticalSplitter_6->SetPropertyValue<std::string>("Name", "ImVerticalSplitter_6");
    ImVerticalSplitter_6->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    ImVerticalSplitter_6->SetPropertyValue<bool>("HaveBorder", true);
    ImGuiWidget::PropertyStruct* ImVerticalSplitter_6_SplitterStyle = ImVerticalSplitter_6->GetPropertyPtr<ImGuiWidget::PropertyStruct>("SplitterStyle");
    ImVerticalSplitter_6_SplitterStyle->SetPropertyValue<float>("BarHeight", 4.000000f);
    ImVerticalSplitter_6_SplitterStyle->SetPropertyValue<ImU32>("Color", IM_COL32(100, 100, 100, 255));
    ImVerticalSplitter_6_SplitterStyle->SetPropertyValue<ImU32>("HoveredColor", IM_COL32(120, 120, 120, 255));
    ImVerticalSplitter_6_SplitterStyle->SetPropertyValue<ImU32>("ActiveColor", IM_COL32(150, 150, 150, 255));
    ImVerticalSplitter_6_SplitterStyle->SetPropertyValue<float>("Rounding", 0.000000f);

    ImHorizontalSplitter_1->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    ImHorizontalSplitter_1->SetPropertyValue<std::string>("Name", "ImHorizontalSplitter_1");
    ImHorizontalSplitter_1->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    ImHorizontalSplitter_1->SetPropertyValue<bool>("HaveBorder", true);
    ImGuiWidget::PropertyStruct* ImHorizontalSplitter_1_SplitterStyle = ImHorizontalSplitter_1->GetPropertyPtr<ImGuiWidget::PropertyStruct>("SplitterStyle");
    ImHorizontalSplitter_1_SplitterStyle->SetPropertyValue<ImU32>("ActiveColor", IM_COL32(150, 150, 150, 255));
    ImHorizontalSplitter_1_SplitterStyle->SetPropertyValue<float>("BarWidth", 4.000000f);
    ImHorizontalSplitter_1_SplitterStyle->SetPropertyValue<ImU32>("Color", IM_COL32(100, 100, 100, 255));
    ImHorizontalSplitter_1_SplitterStyle->SetPropertyValue<ImU32>("HoveredColor", IM_COL32(120, 120, 120, 255));
    ImHorizontalSplitter_1_SplitterStyle->SetPropertyValue<float>("Rounding", 0.000000f);

    ImBorder_Left->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    ImBorder_Left->SetPropertyValue<std::string>("Name", "ImBorder_Left");
    ImBorder_Left->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    ImBorder_Left->SetPropertyValue<bool>("HaveBorder", true);

    ImVerticalSplitter_0->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    ImVerticalSplitter_0->SetPropertyValue<std::string>("Name", "ImVerticalSplitter_0");
    ImVerticalSplitter_0->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    ImVerticalSplitter_0->SetPropertyValue<bool>("HaveBorder", true);
    ImGuiWidget::PropertyStruct* ImVerticalSplitter_0_SplitterStyle = ImVerticalSplitter_0->GetPropertyPtr<ImGuiWidget::PropertyStruct>("SplitterStyle");
    ImVerticalSplitter_0_SplitterStyle->SetPropertyValue<float>("BarHeight", 4.000000f);
    ImVerticalSplitter_0_SplitterStyle->SetPropertyValue<ImU32>("Color", IM_COL32(100, 100, 100, 255));
    ImVerticalSplitter_0_SplitterStyle->SetPropertyValue<ImU32>("HoveredColor", IM_COL32(120, 120, 120, 255));
    ImVerticalSplitter_0_SplitterStyle->SetPropertyValue<ImU32>("ActiveColor", IM_COL32(150, 150, 150, 255));
    ImVerticalSplitter_0_SplitterStyle->SetPropertyValue<float>("Rounding", 0.000000f);

    ImScrollBox_WidgetList->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    ImScrollBox_WidgetList->SetPropertyValue<std::string>("Name", "ImScrollBox_WidgetList");
    ImScrollBox_WidgetList->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    ImScrollBox_WidgetList->SetPropertyValue<ImU32>("ScrollbarGrabHoveredColor", IM_COL32(120, 120, 120, 255));
    ImScrollBox_WidgetList->SetPropertyValue<ImU32>("ScrollbarBgColor", IM_COL32(40, 40, 40, 255));
    ImScrollBox_WidgetList->SetPropertyValue<bool>("HaveBorder", true);
    ImScrollBox_WidgetList->SetPropertyValue<float>("ScrollbarThickness", 10.000000f);
    ImScrollBox_WidgetList->SetPropertyValue<ImU32>("ScrollbarGrabColor", IM_COL32(100, 100, 100, 255));
    ImScrollBox_WidgetList->SetPropertyValue<ImU32>("ScrollbarGrabActiveColor", IM_COL32(150, 150, 150, 255));
    ImScrollBox_WidgetList->SetPropertyValue<bool>("HorizontalScrollEnabled", true);
    ImScrollBox_WidgetList->SetPropertyValue<bool>("VerticalScrollEnabled", true);
    ImScrollBox_WidgetList->SetPropertyValue<bool>("ShowHorizontalScrollbar", true);
    ImScrollBox_WidgetList->SetPropertyValue<bool>("ShowVerticalScrollbar", true);

    ImVerticalBox_WidgetList->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    ImVerticalBox_WidgetList->SetPropertyValue<std::string>("Name", "ImVerticalBox_WidgetList");
    ImVerticalBox_WidgetList->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    ImVerticalBox_WidgetList->SetPropertyValue<bool>("HaveBorder", true);

    ImScrollBox_WidgetTreeView->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    ImScrollBox_WidgetTreeView->SetPropertyValue<std::string>("Name", "ImScrollBox_WidgetTreeView");
    ImScrollBox_WidgetTreeView->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    ImScrollBox_WidgetTreeView->SetPropertyValue<ImU32>("ScrollbarGrabHoveredColor", IM_COL32(120, 120, 120, 255));
    ImScrollBox_WidgetTreeView->SetPropertyValue<ImU32>("ScrollbarBgColor", IM_COL32(40, 40, 40, 255));
    ImScrollBox_WidgetTreeView->SetPropertyValue<bool>("HaveBorder", true);
    ImScrollBox_WidgetTreeView->SetPropertyValue<float>("ScrollbarThickness", 10.000000f);
    ImScrollBox_WidgetTreeView->SetPropertyValue<ImU32>("ScrollbarGrabColor", IM_COL32(100, 100, 100, 255));
    ImScrollBox_WidgetTreeView->SetPropertyValue<ImU32>("ScrollbarGrabActiveColor", IM_COL32(150, 150, 150, 255));
    ImScrollBox_WidgetTreeView->SetPropertyValue<bool>("HorizontalScrollEnabled", true);
    ImScrollBox_WidgetTreeView->SetPropertyValue<bool>("VerticalScrollEnabled", true);
    ImScrollBox_WidgetTreeView->SetPropertyValue<bool>("ShowHorizontalScrollbar", true);
    ImScrollBox_WidgetTreeView->SetPropertyValue<bool>("ShowVerticalScrollbar", true);

    ImBorder_LeftTab->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    ImBorder_LeftTab->SetPropertyValue<std::string>("Name", "ImBorder_LeftTab");
    ImBorder_LeftTab->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    ImBorder_LeftTab->SetPropertyValue<bool>("HaveBorder", true);

    ImBorder_MainWorkSpace->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    ImBorder_MainWorkSpace->SetPropertyValue<std::string>("Name", "ImBorder_MainWorkSpace");
    ImBorder_MainWorkSpace->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    ImBorder_MainWorkSpace->SetPropertyValue<bool>("HaveBorder", true);

    ImVerticalBox_7->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    ImVerticalBox_7->SetPropertyValue<std::string>("Name", "ImVerticalBox_7");
    ImVerticalBox_7->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    ImVerticalBox_7->SetPropertyValue<bool>("HaveBorder", true);

    ImHorizontalBox_PageTag->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    ImHorizontalBox_PageTag->SetPropertyValue<std::string>("Name", "ImHorizontalBox_PageTag");
    ImHorizontalBox_PageTag->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    ImHorizontalBox_PageTag->SetPropertyValue<bool>("HaveBorder", true);

    ImBorder_CurrentPage->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    ImBorder_CurrentPage->SetPropertyValue<std::string>("Name", "ImBorder_CurrentPage");
    ImBorder_CurrentPage->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    ImBorder_CurrentPage->SetPropertyValue<bool>("HaveBorder", true);

    ImBorder_Right->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    ImBorder_Right->SetPropertyValue<std::string>("Name", "ImBorder_Right");
    ImBorder_Right->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    ImBorder_Right->SetPropertyValue<bool>("HaveBorder", true);

    ImScrollingTextList_LogList->SetPropertyValue<std::string>("Name", "ImScrollingTextList_LogList");
    ImScrollingTextList_LogList->SetPropertyValue<float>("ScrollbarRounding", 0.000000f);
    ImScrollingTextList_LogList->SetPropertyValue<ImU32>("TextColor", IM_COL32(0, 0, 0, 255));
    ImScrollingTextList_LogList->SetPropertyValue<ImU32>("ScrollbarBackgroundColor", IM_COL32(200, 200, 200, 255));
    ImScrollingTextList_LogList->SetPropertyValue<float>("LineSpacing", -1.000000f);
    ImScrollingTextList_LogList->SetPropertyValue<float>("ScrollbarWidth", 14.000000f);
    ImScrollingTextList_LogList->SetPropertyValue<ImU32>("ScrollbarGrabColor", IM_COL32(120, 120, 120, 255));
    ImScrollingTextList_LogList->SetPropertyValue<float>("ScrollbarGrabMinHeight", 20.000000f);

    ImGuiWidget::ImSlot* ImVerticalBox_3_slot0 = ImVerticalBox_3->AddChild(ImHorizontalBox_4);
    ImVerticalBox_3_slot0->SetPropertyValue<float>("SizeRatio", 0.500000f);
    ImVerticalBox_3_slot0->SetPropertyValue<bool>("bAutoSize", false);
    ImVerticalBox_3_slot0->SetPropertyValue<float>("PaddingTop", 0.000000f);
    ImVerticalBox_3_slot0->SetPropertyValue<float>("PaddingBottom", 0.000000f);
    ImVerticalBox_3_slot0->SetPropertyValue<float>("PaddingLeft", 0.000000f);
    ImVerticalBox_3_slot0->SetPropertyValue<float>("PaddingRight", 0.000000f);

    ImGuiWidget::ImSlot* ImVerticalBox_3_slot1 = ImVerticalBox_3->AddChild(ImVerticalSplitter_6);
    ImVerticalBox_3_slot1->SetPropertyValue<float>("SizeRatio", 1.000000f);
    ImVerticalBox_3_slot1->SetPropertyValue<bool>("bAutoSize", true);
    ImVerticalBox_3_slot1->SetPropertyValue<float>("PaddingTop", 0.000000f);
    ImVerticalBox_3_slot1->SetPropertyValue<float>("PaddingBottom", 0.000000f);
    ImVerticalBox_3_slot1->SetPropertyValue<float>("PaddingLeft", 0.000000f);
    ImVerticalBox_3_slot1->SetPropertyValue<float>("PaddingRight", 0.000000f);

    ImGuiWidget::ImSlot* ImHorizontalBox_4_slot0 = ImHorizontalBox_4->AddChild(ImButton_Project);
    ImHorizontalBox_4_slot0->SetPropertyValue<float>("SizeRatio", 1.000000f);
    ImHorizontalBox_4_slot0->SetPropertyValue<bool>("bAutoSize", false);
    ImHorizontalBox_4_slot0->SetPropertyValue<float>("PaddingTop", 1.000000f);
    ImHorizontalBox_4_slot0->SetPropertyValue<float>("PaddingBottom", 1.000000f);
    ImHorizontalBox_4_slot0->SetPropertyValue<float>("PaddingLeft", 1.000000f);
    ImHorizontalBox_4_slot0->SetPropertyValue<float>("PaddingRight", 1.000000f);

    ImGuiWidget::ImSlot* ImHorizontalBox_4_slot1 = ImHorizontalBox_4->AddChild(ImButton_Save);
    ImHorizontalBox_4_slot1->SetPropertyValue<float>("SizeRatio", 1.000000f);
    ImHorizontalBox_4_slot1->SetPropertyValue<bool>("bAutoSize", false);
    ImHorizontalBox_4_slot1->SetPropertyValue<float>("PaddingTop", 1.000000f);
    ImHorizontalBox_4_slot1->SetPropertyValue<float>("PaddingBottom", 1.000000f);
    ImHorizontalBox_4_slot1->SetPropertyValue<float>("PaddingLeft", 1.000000f);
    ImHorizontalBox_4_slot1->SetPropertyValue<float>("PaddingRight", 1.000000f);

    ImGuiWidget::ImSlot* ImHorizontalBox_4_slot2 = ImHorizontalBox_4->AddChild(ImButton_Generate);
    ImHorizontalBox_4_slot2->SetPropertyValue<float>("SizeRatio", 1.000000f);
    ImHorizontalBox_4_slot2->SetPropertyValue<bool>("bAutoSize", false);
    ImHorizontalBox_4_slot2->SetPropertyValue<float>("PaddingTop", 1.000000f);
    ImHorizontalBox_4_slot2->SetPropertyValue<float>("PaddingBottom", 1.000000f);
    ImHorizontalBox_4_slot2->SetPropertyValue<float>("PaddingLeft", 1.000000f);
    ImHorizontalBox_4_slot2->SetPropertyValue<float>("PaddingRight", 1.000000f);

    ImGuiWidget::ImSlot* ImButton_Project_slot0 = ImButton_Project->AddChild(ImTextBlock_9);
    ImButton_Project_slot0->SetPropertyValue<bool>("bAutoSize", true);

    ImGuiWidget::ImSlot* ImButton_Save_slot0 = ImButton_Save->AddChild(ImTextBlock_10);
    ImButton_Save_slot0->SetPropertyValue<bool>("bAutoSize", true);

    ImGuiWidget::ImSlot* ImButton_Generate_slot0 = ImButton_Generate->AddChild(ImTextBlock_11);
    ImButton_Generate_slot0->SetPropertyValue<bool>("bAutoSize", true);

    ImGuiWidget::ImSlot* ImVerticalSplitter_6_slot0 = ImVerticalSplitter_6->AddChild(ImHorizontalSplitter_1);
    ImVerticalSplitter_6_slot0->SetPropertyValue<bool>("bAutoSize", true);
    ImVerticalSplitter_6_slot0->SetPropertyValue<float>("PaddingTop", 0.000000f);
    ImVerticalSplitter_6_slot0->SetPropertyValue<float>("PaddingBottom", 0.000000f);
    ImVerticalSplitter_6_slot0->SetPropertyValue<float>("PaddingLeft", 0.000000f);
    ImVerticalSplitter_6_slot0->SetPropertyValue<float>("Ratio", 1.000000f);
    ImVerticalSplitter_6_slot0->SetPropertyValue<float>("PaddingRight", 0.000000f);
    ImVerticalSplitter_6_slot0->SetPropertyValue<float>("MinSize", 30.000000f);

    ImGuiWidget::ImSlot* ImVerticalSplitter_6_slot1 = ImVerticalSplitter_6->AddChild(ImScrollingTextList_LogList);
    ImVerticalSplitter_6_slot1->SetPropertyValue<bool>("bAutoSize", true);
    ImVerticalSplitter_6_slot1->SetPropertyValue<float>("PaddingTop", 0.000000f);
    ImVerticalSplitter_6_slot1->SetPropertyValue<float>("PaddingBottom", 0.000000f);
    ImVerticalSplitter_6_slot1->SetPropertyValue<float>("PaddingLeft", 0.000000f);
    ImVerticalSplitter_6_slot1->SetPropertyValue<float>("Ratio", 0.200000f);
    ImVerticalSplitter_6_slot1->SetPropertyValue<float>("PaddingRight", 0.000000f);
    ImVerticalSplitter_6_slot1->SetPropertyValue<float>("MinSize", 30.000000f);

    ImGuiWidget::ImSlot* ImHorizontalSplitter_1_slot0 = ImHorizontalSplitter_1->AddChild(ImBorder_Left);
    ImHorizontalSplitter_1_slot0->SetPropertyValue<bool>("bAutoSize", true);
    ImHorizontalSplitter_1_slot0->SetPropertyValue<float>("PaddingTop", 0.000000f);
    ImHorizontalSplitter_1_slot0->SetPropertyValue<float>("PaddingBottom", 0.000000f);
    ImHorizontalSplitter_1_slot0->SetPropertyValue<float>("PaddingLeft", 0.000000f);
    ImHorizontalSplitter_1_slot0->SetPropertyValue<float>("Ratio", 0.300000f);
    ImHorizontalSplitter_1_slot0->SetPropertyValue<float>("PaddingRight", 0.000000f);
    ImHorizontalSplitter_1_slot0->SetPropertyValue<float>("MinSize", 30.000000f);

    ImGuiWidget::ImSlot* ImHorizontalSplitter_1_slot1 = ImHorizontalSplitter_1->AddChild(ImBorder_MainWorkSpace);
    ImHorizontalSplitter_1_slot1->SetPropertyValue<bool>("bAutoSize", true);
    ImHorizontalSplitter_1_slot1->SetPropertyValue<float>("PaddingTop", 0.000000f);
    ImHorizontalSplitter_1_slot1->SetPropertyValue<float>("PaddingBottom", 0.000000f);
    ImHorizontalSplitter_1_slot1->SetPropertyValue<float>("PaddingLeft", 0.000000f);
    ImHorizontalSplitter_1_slot1->SetPropertyValue<float>("Ratio", 1.000000f);
    ImHorizontalSplitter_1_slot1->SetPropertyValue<float>("PaddingRight", 0.000000f);
    ImHorizontalSplitter_1_slot1->SetPropertyValue<float>("MinSize", 30.000000f);

    ImGuiWidget::ImSlot* ImHorizontalSplitter_1_slot2 = ImHorizontalSplitter_1->AddChild(ImBorder_Right);
    ImHorizontalSplitter_1_slot2->SetPropertyValue<bool>("bAutoSize", true);
    ImHorizontalSplitter_1_slot2->SetPropertyValue<float>("PaddingTop", 0.000000f);
    ImHorizontalSplitter_1_slot2->SetPropertyValue<float>("PaddingBottom", 0.000000f);
    ImHorizontalSplitter_1_slot2->SetPropertyValue<float>("PaddingLeft", 0.000000f);
    ImHorizontalSplitter_1_slot2->SetPropertyValue<float>("Ratio", 0.300000f);
    ImHorizontalSplitter_1_slot2->SetPropertyValue<float>("PaddingRight", 0.000000f);
    ImHorizontalSplitter_1_slot2->SetPropertyValue<float>("MinSize", 30.000000f);

    ImGuiWidget::ImSlot* ImBorder_Left_slot0 = ImBorder_Left->AddChild(ImVerticalSplitter_0);
    ImBorder_Left_slot0->SetPropertyValue<bool>("bAutoSize", true);
    ImBorder_Left_slot0->SetPropertyValue<float>("PaddingTop", 0.000000f);
    ImBorder_Left_slot0->SetPropertyValue<float>("PaddingBottom", 0.000000f);
    ImBorder_Left_slot0->SetPropertyValue<float>("PaddingLeft", 0.000000f);
    ImBorder_Left_slot0->SetPropertyValue<float>("PaddingRight", 0.000000f);

    ImGuiWidget::ImSlot* ImVerticalSplitter_0_slot0 = ImVerticalSplitter_0->AddChild(ImScrollBox_WidgetList);
    ImVerticalSplitter_0_slot0->SetPropertyValue<bool>("bAutoSize", true);
    ImVerticalSplitter_0_slot0->SetPropertyValue<float>("PaddingTop", 0.000000f);
    ImVerticalSplitter_0_slot0->SetPropertyValue<float>("PaddingBottom", 0.000000f);
    ImVerticalSplitter_0_slot0->SetPropertyValue<float>("PaddingLeft", 0.000000f);
    ImVerticalSplitter_0_slot0->SetPropertyValue<float>("Ratio", 1.000000f);
    ImVerticalSplitter_0_slot0->SetPropertyValue<float>("PaddingRight", 0.000000f);
    ImVerticalSplitter_0_slot0->SetPropertyValue<float>("MinSize", 30.000000f);

    ImGuiWidget::ImSlot* ImVerticalSplitter_0_slot1 = ImVerticalSplitter_0->AddChild(ImScrollBox_WidgetTreeView);
    ImVerticalSplitter_0_slot1->SetPropertyValue<bool>("bAutoSize", true);
    ImVerticalSplitter_0_slot1->SetPropertyValue<float>("PaddingTop", 0.000000f);
    ImVerticalSplitter_0_slot1->SetPropertyValue<float>("PaddingBottom", 0.000000f);
    ImVerticalSplitter_0_slot1->SetPropertyValue<float>("PaddingLeft", 0.000000f);
    ImVerticalSplitter_0_slot1->SetPropertyValue<float>("Ratio", 1.000000f);
    ImVerticalSplitter_0_slot1->SetPropertyValue<float>("PaddingRight", 0.000000f);
    ImVerticalSplitter_0_slot1->SetPropertyValue<float>("MinSize", 30.000000f);

    ImGuiWidget::ImSlot* ImVerticalSplitter_0_slot2 = ImVerticalSplitter_0->AddChild(ImBorder_LeftTab);
    ImVerticalSplitter_0_slot2->SetPropertyValue<bool>("bAutoSize", true);
    ImVerticalSplitter_0_slot2->SetPropertyValue<float>("PaddingTop", 0.000000f);
    ImVerticalSplitter_0_slot2->SetPropertyValue<float>("PaddingBottom", 0.000000f);
    ImVerticalSplitter_0_slot2->SetPropertyValue<float>("PaddingLeft", 0.000000f);
    ImVerticalSplitter_0_slot2->SetPropertyValue<float>("Ratio", 1.000000f);
    ImVerticalSplitter_0_slot2->SetPropertyValue<float>("PaddingRight", 0.000000f);
    ImVerticalSplitter_0_slot2->SetPropertyValue<float>("MinSize", 30.000000f);

    ImGuiWidget::ImSlot* ImScrollBox_WidgetList_slot0 = ImScrollBox_WidgetList->AddChild(ImVerticalBox_WidgetList);
    ImScrollBox_WidgetList_slot0->SetPropertyValue<bool>("bAutoSize", true);

    ImGuiWidget::ImSlot* ImBorder_MainWorkSpace_slot0 = ImBorder_MainWorkSpace->AddChild(ImVerticalBox_7);
    ImBorder_MainWorkSpace_slot0->SetPropertyValue<bool>("bAutoSize", true);
    ImBorder_MainWorkSpace_slot0->SetPropertyValue<float>("PaddingTop", 0.000000f);
    ImBorder_MainWorkSpace_slot0->SetPropertyValue<float>("PaddingBottom", 0.000000f);
    ImBorder_MainWorkSpace_slot0->SetPropertyValue<float>("PaddingLeft", 0.000000f);
    ImBorder_MainWorkSpace_slot0->SetPropertyValue<float>("PaddingRight", 0.000000f);

    ImGuiWidget::ImSlot* ImVerticalBox_7_slot0 = ImVerticalBox_7->AddChild(ImHorizontalBox_PageTag);
    ImVerticalBox_7_slot0->SetPropertyValue<float>("SizeRatio", 0.100000f);
    ImVerticalBox_7_slot0->SetPropertyValue<bool>("bAutoSize", false);
    ImVerticalBox_7_slot0->SetPropertyValue<float>("PaddingTop", 0.000000f);
    ImVerticalBox_7_slot0->SetPropertyValue<float>("PaddingBottom", 0.000000f);
    ImVerticalBox_7_slot0->SetPropertyValue<float>("PaddingLeft", 0.000000f);
    ImVerticalBox_7_slot0->SetPropertyValue<float>("PaddingRight", 0.000000f);

    ImGuiWidget::ImSlot* ImVerticalBox_7_slot1 = ImVerticalBox_7->AddChild(ImBorder_CurrentPage);
    ImVerticalBox_7_slot1->SetPropertyValue<float>("SizeRatio", 1.000000f);
    ImVerticalBox_7_slot1->SetPropertyValue<bool>("bAutoSize", true);
    ImVerticalBox_7_slot1->SetPropertyValue<float>("PaddingTop", 0.000000f);
    ImVerticalBox_7_slot1->SetPropertyValue<float>("PaddingBottom", 0.000000f);
    ImVerticalBox_7_slot1->SetPropertyValue<float>("PaddingLeft", 0.000000f);
    ImVerticalBox_7_slot1->SetPropertyValue<float>("PaddingRight", 0.000000f);


    SetRootWidget(ImVerticalBox_3);
}
//----Gen Code End----
