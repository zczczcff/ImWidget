//----Gen Include Begin----
// Auto-generated includes
#include "Test1.h"
#include "ImUserWidget.h"
#include "ImButton.h"
#include "ImCheckBox.h"
#include "ImHorizontalBox.h"
#include "ImHorizontalSplitter.h"
#include "ImPanelWidget.h"
#include "ImScrollingTextList.h"
#include "ImTextBlock.h"
#include "ImVerticalBox.h"
#include "ImWidget.h"
//----Gen Include End----

using namespace ImGuiWidget;

Test1::Test1(const std::string& name)
    : ImUserWidget(name)
{
}

//----Gen Code Begin----
// Auto-generated initialization code
void Test1::Init()
{
    MainBox3 = new ImGuiWidget::ImVerticalBox("MainBox3");
    m_MenuList34 = new ImGuiWidget::ImHorizontalBox("m_MenuList34");
    MenuButton_Project35 = new ImGuiWidget::ImButton("MenuButton_Project35");
    Button_Project_Text41 = new ImGuiWidget::ImTextBlock("Button_Project_Text41");
    VSplitter02 = new ImGuiWidget::ImHorizontalSplitter("VSplitter02");
    MiddleBox4 = new ImGuiWidget::ImVerticalBox("MiddleBox4");
    Splitter01 = new ImGuiWidget::ImHorizontalSplitter("Splitter01");
    WidgetList_WidgetTreeSplitter5 = new ImGuiWidget::ImHorizontalSplitter("WidgetList_WidgetTreeSplitter5");
    WidgetList6 = new ImGuiWidget::ImVerticalBox("WidgetList6");
    Example_Button18 = new ImGuiWidget::ImWidget("Example_Button18");
    Example_TextBlock20 = new ImGuiWidget::ImWidget("Example_TextBlock20");
    Example_Image22 = new ImGuiWidget::ImWidget("Example_Image22");
    Example_CanvasPanel24 = new ImGuiWidget::ImWidget("Example_CanvasPanel24");
    Example_HorizontalBox26 = new ImGuiWidget::ImWidget("Example_HorizontalBox26");
    Example_VerticalBox28 = new ImGuiWidget::ImWidget("Example_VerticalBox28");
    InputTextTest30 = new ImGuiWidget::ImWidget("InputTextTest30");
    CheckBoxTest31 = new ImGuiWidget::ImCheckBox("CheckBoxTest31");
    WidgetTreeView7 = new ImGuiWidget::ImWidget("WidgetTreeView7");
    CenterPageManager12 = new ImGuiWidget::ImPanelWidget("CenterPageManager12");
    TabBar13 = new ImGuiWidget::ImHorizontalBox("TabBar13");
    TabButton_test17 = new ImGuiWidget::ImButton("TabButton_test17");
    TabItem_test14 = new ImGuiWidget::ImWidget("TabItem_test14");
    DetailList10 = new ImGuiWidget::ImWidget("DetailList10");
    BottomBox32 = new ImGuiWidget::ImVerticalBox("BottomBox32");
    LogList33 = new ImGuiWidget::ImScrollingTextList("LogList33");

    MainBox3->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    MainBox3->SetPropertyValue<bool>("HaveBorder", true);
    MainBox3->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));

    m_MenuList34->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(50, 50, 230, 255));
    m_MenuList34->SetPropertyValue<bool>("HaveBorder", true);
    m_MenuList34->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));

    MenuButton_Project35->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    MenuButton_Project35->SetPropertyValue<bool>("HaveBorder", true);
    ImGuiWidget::PropertyStruct* MenuButton_Project35_PressedStyle = MenuButton_Project35->GetPropertyPtr<ImGuiWidget::PropertyStruct>("PressedStyle");
    MenuButton_Project35_PressedStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(250, 220, 190, 120));
    MenuButton_Project35_PressedStyle->SetPropertyValue<bool>("HasBorder", false);
    MenuButton_Project35_PressedStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    MenuButton_Project35_PressedStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    MenuButton_Project35_PressedStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));
    MenuButton_Project35->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    ImGuiWidget::PropertyStruct* MenuButton_Project35_NormalStyle = MenuButton_Project35->GetPropertyPtr<ImGuiWidget::PropertyStruct>("NormalStyle");
    MenuButton_Project35_NormalStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(210, 180, 150, 120));
    MenuButton_Project35_NormalStyle->SetPropertyValue<bool>("HasBorder", false);
    MenuButton_Project35_NormalStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    MenuButton_Project35_NormalStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    MenuButton_Project35_NormalStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));
    MenuButton_Project35->SetPropertyValue<std::string>("TooltipText", "Project");
    ImGuiWidget::PropertyStruct* MenuButton_Project35_HoveredStyle = MenuButton_Project35->GetPropertyPtr<ImGuiWidget::PropertyStruct>("HoveredStyle");
    MenuButton_Project35_HoveredStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(230, 200, 170, 120));
    MenuButton_Project35_HoveredStyle->SetPropertyValue<bool>("HasBorder", false);
    MenuButton_Project35_HoveredStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    MenuButton_Project35_HoveredStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    MenuButton_Project35_HoveredStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));


    VSplitter02->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    VSplitter02->SetPropertyValue<bool>("HaveBorder", true);
    VSplitter02->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));

    MiddleBox4->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    MiddleBox4->SetPropertyValue<bool>("HaveBorder", true);
    MiddleBox4->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));

    Splitter01->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    Splitter01->SetPropertyValue<bool>("HaveBorder", true);
    Splitter01->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));

    WidgetList_WidgetTreeSplitter5->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    WidgetList_WidgetTreeSplitter5->SetPropertyValue<bool>("HaveBorder", true);
    WidgetList_WidgetTreeSplitter5->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));

    WidgetList6->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    WidgetList6->SetPropertyValue<bool>("HaveBorder", true);
    WidgetList6->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));







    InputTextTest30->SetPropertyValue<ImU32>("BorderColor", IM_COL32(100, 100, 100, 255));
    InputTextTest30->SetPropertyValue<std::string>("Text", "");
    InputTextTest30->SetPropertyValue<ImU32>("TextColor", IM_COL32(0, 0, 0, 255));
    InputTextTest30->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(255, 255, 255, 255));
    InputTextTest30->SetPropertyValue<float>("BorderThickness", 1.000000f);
    InputTextTest30->SetPropertyValue<float>("Rounding", 4.000000f);

    CheckBoxTest31->SetPropertyValue<float>("BoxRounding", 3.000000f);
    CheckBoxTest31->SetPropertyValue<ImU32>("CheckColor", IM_COL32(0, 0, 0, 255));
    CheckBoxTest31->SetPropertyValue<bool>("Checked", false);
    CheckBoxTest31->SetPropertyValue<ImU32>("BoxColor", IM_COL32(100, 100, 100, 255));
    CheckBoxTest31->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(255, 255, 255, 255));
    CheckBoxTest31->SetPropertyValue<float>("BorderThickness", 1.500000f);


    CenterPageManager12->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    CenterPageManager12->SetPropertyValue<bool>("HaveBorder", true);
    CenterPageManager12->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));

    TabBar13->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    TabBar13->SetPropertyValue<bool>("HaveBorder", true);
    TabBar13->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));

    TabButton_test17->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    TabButton_test17->SetPropertyValue<bool>("HaveBorder", true);
    ImGuiWidget::PropertyStruct* TabButton_test17_PressedStyle = TabButton_test17->GetPropertyPtr<ImGuiWidget::PropertyStruct>("PressedStyle");
    TabButton_test17_PressedStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(50, 50, 80, 220));
    TabButton_test17_PressedStyle->SetPropertyValue<bool>("HasBorder", true);
    TabButton_test17_PressedStyle->SetPropertyValue<float>("Rounding", 4.000000f);
    TabButton_test17_PressedStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    TabButton_test17_PressedStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(100, 100, 100, 200));
    TabButton_test17->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    ImGuiWidget::PropertyStruct* TabButton_test17_NormalStyle = TabButton_test17->GetPropertyPtr<ImGuiWidget::PropertyStruct>("NormalStyle");
    TabButton_test17_NormalStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(70, 70, 100, 220));
    TabButton_test17_NormalStyle->SetPropertyValue<bool>("HasBorder", true);
    TabButton_test17_NormalStyle->SetPropertyValue<float>("Rounding", 4.000000f);
    TabButton_test17_NormalStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    TabButton_test17_NormalStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(100, 100, 100, 200));
    TabButton_test17->SetPropertyValue<std::string>("TooltipText", "");
    ImGuiWidget::PropertyStruct* TabButton_test17_HoveredStyle = TabButton_test17->GetPropertyPtr<ImGuiWidget::PropertyStruct>("HoveredStyle");
    TabButton_test17_HoveredStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(90, 90, 120, 220));
    TabButton_test17_HoveredStyle->SetPropertyValue<bool>("HasBorder", true);
    TabButton_test17_HoveredStyle->SetPropertyValue<float>("Rounding", 4.000000f);
    TabButton_test17_HoveredStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    TabButton_test17_HoveredStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(100, 100, 100, 200));



    BottomBox32->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    BottomBox32->SetPropertyValue<bool>("HaveBorder", true);
    BottomBox32->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));


    MainBox3->AddChild(m_MenuList34);
    MainBox3->AddChild(VSplitter02);
    m_MenuList34->AddChild(MenuButton_Project35);
    MenuButton_Project35->AddChild(Button_Project_Text41);
    VSplitter02->AddChild(MiddleBox4);
    VSplitter02->AddChild(BottomBox32);
    MiddleBox4->AddChild(Splitter01);
    Splitter01->AddChild(WidgetList_WidgetTreeSplitter5);
    Splitter01->AddChild(CenterPageManager12);
    Splitter01->AddChild(DetailList10);
    WidgetList_WidgetTreeSplitter5->AddChild(WidgetList6);
    WidgetList_WidgetTreeSplitter5->AddChild(WidgetTreeView7);
    WidgetList6->AddChild(Example_Button18);
    WidgetList6->AddChild(Example_TextBlock20);
    WidgetList6->AddChild(Example_Image22);
    WidgetList6->AddChild(Example_CanvasPanel24);
    WidgetList6->AddChild(Example_HorizontalBox26);
    WidgetList6->AddChild(Example_VerticalBox28);
    WidgetList6->AddChild(InputTextTest30);
    WidgetList6->AddChild(CheckBoxTest31);
    CenterPageManager12->AddChild(TabBar13);
    TabBar13->AddChild(TabButton_test17);
    TabButton_test17->AddChild(TabItem_test14);
    BottomBox32->AddChild(LogList33);

    SetRootWidget(MainBox3);
}
//----Gen Code End----
