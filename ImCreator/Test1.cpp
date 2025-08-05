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













Test1::Test1(const std::string& name)
    : ImGuiWidget::ImUserWidget(name)
{
}

//----Gen Code Begin----
// Auto-generated initialization code
void Test1::Init()
{
    MainBox3 = new ImGuiWidget::ImVerticalBox("MainBox3");
    m_MenuList36 = new ImGuiWidget::ImHorizontalBox("m_MenuList36");
    MenuButton_Project37 = new ImGuiWidget::ImButton("MenuButton_Project37");
    Button_Project_Text43 = new ImGuiWidget::ImTextBlock("Button_Project_Text43");
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
    Example_ComboBox30 = new ImGuiWidget::ImWidget("Example_ComboBox30");
    InputTextTest32 = new ImGuiWidget::ImWidget("InputTextTest32");
    CheckBoxTest33 = new ImGuiWidget::ImCheckBox("CheckBoxTest33");
    WidgetTreeView7 = new ImGuiWidget::ImWidget("WidgetTreeView7");
    CenterPageManager12 = new ImGuiWidget::ImPanelWidget("CenterPageManager12");
    TabBar13 = new ImGuiWidget::ImHorizontalBox("TabBar13");
    TabButton_test17 = new ImGuiWidget::ImButton("TabButton_test17");
    TabItem_test14 = new ImGuiWidget::ImWidget("TabItem_test14");
    DetailList10 = new ImGuiWidget::ImWidget("DetailList10");
    BottomBox34 = new ImGuiWidget::ImVerticalBox("BottomBox34");
    LogList35 = new ImGuiWidget::ImScrollingTextList("LogList35");

    MainBox3->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    MainBox3->SetPropertyValue<bool>("HaveBorder", true);
    MainBox3->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));

    m_MenuList36->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(50, 50, 230, 255));
    m_MenuList36->SetPropertyValue<bool>("HaveBorder", true);
    m_MenuList36->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));

    MenuButton_Project37->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    MenuButton_Project37->SetPropertyValue<bool>("HaveBorder", true);
    ImGuiWidget::PropertyStruct* MenuButton_Project37_PressedStyle = MenuButton_Project37->GetPropertyPtr<ImGuiWidget::PropertyStruct>("PressedStyle");
    MenuButton_Project37_PressedStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(250, 220, 190, 120));
    MenuButton_Project37_PressedStyle->SetPropertyValue<bool>("HasBorder", false);
    MenuButton_Project37_PressedStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    MenuButton_Project37_PressedStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    MenuButton_Project37_PressedStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));
    MenuButton_Project37->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    ImGuiWidget::PropertyStruct* MenuButton_Project37_NormalStyle = MenuButton_Project37->GetPropertyPtr<ImGuiWidget::PropertyStruct>("NormalStyle");
    MenuButton_Project37_NormalStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(210, 180, 150, 120));
    MenuButton_Project37_NormalStyle->SetPropertyValue<bool>("HasBorder", false);
    MenuButton_Project37_NormalStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    MenuButton_Project37_NormalStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    MenuButton_Project37_NormalStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));
    MenuButton_Project37->SetPropertyValue<std::string>("TooltipText", "Project");
    ImGuiWidget::PropertyStruct* MenuButton_Project37_HoveredStyle = MenuButton_Project37->GetPropertyPtr<ImGuiWidget::PropertyStruct>("HoveredStyle");
    MenuButton_Project37_HoveredStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(230, 200, 170, 120));
    MenuButton_Project37_HoveredStyle->SetPropertyValue<bool>("HasBorder", false);
    MenuButton_Project37_HoveredStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    MenuButton_Project37_HoveredStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    MenuButton_Project37_HoveredStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));

    Button_Project_Text43->SetPropertyValue<std::string>("Text", "Project");
    Button_Project_Text43->SetPropertyValue<ImU32>("TextColor", IM_COL32(0, 0, 0, 255));
    Button_Project_Text43->SetPropertyValue<UnknownType>("HorizontalAlignment", "Center");
    Button_Project_Text43->SetPropertyValue<UnknownType>("VerticalAlignment", "Center");

    VSplitter02->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    VSplitter02->SetPropertyValue<bool>("HaveBorder", true);
    VSplitter02->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    ImGuiWidget::PropertyStruct* VSplitter02_SplitterStyle = VSplitter02->GetPropertyPtr<ImGuiWidget::PropertyStruct>("SplitterStyle");
    VSplitter02_SplitterStyle->SetPropertyValue<float>("BarHeight", 4.000000f);
    VSplitter02_SplitterStyle->SetPropertyValue<ImU32>("Color", IM_COL32(100, 100, 100, 255));
    VSplitter02_SplitterStyle->SetPropertyValue<ImU32>("HoveredColor", IM_COL32(120, 120, 120, 255));
    VSplitter02_SplitterStyle->SetPropertyValue<ImU32>("ActiveColor", IM_COL32(150, 150, 150, 255));
    VSplitter02_SplitterStyle->SetPropertyValue<float>("Rounding", 0.000000f);

    MiddleBox4->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    MiddleBox4->SetPropertyValue<bool>("HaveBorder", true);
    MiddleBox4->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));

    Splitter01->SetPropertyValue<ImU32>("BarHoveredColor", IM_COL32(120, 120, 120, 255));
    Splitter01->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    Splitter01->SetPropertyValue<ImU32>("BarColor", IM_COL32(100, 100, 100, 255));
    Splitter01->SetPropertyValue<bool>("HaveBorder", true);
    Splitter01->SetPropertyValue<float>("BarRounding", 0.000000f);
    Splitter01->SetPropertyValue<float>("BarWidth", 4.000000f);
    Splitter01->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    Splitter01->SetPropertyValue<ImU32>("BarActiveColor", IM_COL32(150, 150, 150, 255));

    WidgetList_WidgetTreeSplitter5->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    WidgetList_WidgetTreeSplitter5->SetPropertyValue<bool>("HaveBorder", true);
    WidgetList_WidgetTreeSplitter5->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    ImGuiWidget::PropertyStruct* WidgetList_WidgetTreeSplitter5_SplitterStyle = WidgetList_WidgetTreeSplitter5->GetPropertyPtr<ImGuiWidget::PropertyStruct>("SplitterStyle");
    WidgetList_WidgetTreeSplitter5_SplitterStyle->SetPropertyValue<float>("BarHeight", 4.000000f);
    WidgetList_WidgetTreeSplitter5_SplitterStyle->SetPropertyValue<ImU32>("Color", IM_COL32(100, 100, 100, 255));
    WidgetList_WidgetTreeSplitter5_SplitterStyle->SetPropertyValue<ImU32>("HoveredColor", IM_COL32(120, 120, 120, 255));
    WidgetList_WidgetTreeSplitter5_SplitterStyle->SetPropertyValue<ImU32>("ActiveColor", IM_COL32(150, 150, 150, 255));
    WidgetList_WidgetTreeSplitter5_SplitterStyle->SetPropertyValue<float>("Rounding", 0.000000f);

    WidgetList6->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    WidgetList6->SetPropertyValue<bool>("HaveBorder", true);
    WidgetList6->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));








    InputTextTest32->SetPropertyValue<ImU32>("BorderColor", IM_COL32(100, 100, 100, 255));
    InputTextTest32->SetPropertyValue<std::string>("Text", "");
    InputTextTest32->SetPropertyValue<ImU32>("TextColor", IM_COL32(0, 0, 0, 255));
    InputTextTest32->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(255, 255, 255, 255));
    InputTextTest32->SetPropertyValue<float>("BorderThickness", 1.000000f);
    InputTextTest32->SetPropertyValue<float>("Rounding", 4.000000f);

    CheckBoxTest33->SetPropertyValue<float>("BoxRounding", 3.000000f);
    CheckBoxTest33->SetPropertyValue<ImU32>("CheckColor", IM_COL32(0, 0, 0, 255));
    CheckBoxTest33->SetPropertyValue<bool>("Checked", false);
    CheckBoxTest33->SetPropertyValue<ImU32>("BoxColor", IM_COL32(100, 100, 100, 255));
    CheckBoxTest33->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(255, 255, 255, 255));
    CheckBoxTest33->SetPropertyValue<float>("BorderThickness", 1.500000f);


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



    BottomBox34->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    BottomBox34->SetPropertyValue<bool>("HaveBorder", true);
    BottomBox34->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));

    LogList35->SetPropertyValue<float>("ScrollbarRounding", 0.000000f);
    LogList35->SetPropertyValue<ImU32>("TextColor", IM_COL32(0, 0, 0, 255));
    LogList35->SetPropertyValue<ImU32>("ScrollbarBackgroundColor", IM_COL32(200, 200, 200, 255));
    LogList35->SetPropertyValue<float>("LineSpacing", -1.000000f);
    LogList35->SetPropertyValue<float>("ScrollbarWidth", 14.000000f);
    LogList35->SetPropertyValue<ImU32>("ScrollbarGrabColor", IM_COL32(120, 120, 120, 255));
    LogList35->SetPropertyValue<float>("ScrollbarGrabMinHeight", 20.000000f);

    MainBox3->AddChild(m_MenuList36);
    MainBox3->AddChild(VSplitter02);
    m_MenuList36->AddChild(MenuButton_Project37);
    MenuButton_Project37->AddChild(Button_Project_Text43);
    VSplitter02->AddChild(MiddleBox4);
    VSplitter02->AddChild(BottomBox34);
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
    WidgetList6->AddChild(Example_ComboBox30);
    WidgetList6->AddChild(InputTextTest32);
    WidgetList6->AddChild(CheckBoxTest33);
    CenterPageManager12->AddChild(TabBar13);
    TabBar13->AddChild(TabButton_test17);
    TabButton_test17->AddChild(TabItem_test14);
    BottomBox34->AddChild(LogList35);

    SetRootWidget(MainBox3);
}
//----Gen Code End----






