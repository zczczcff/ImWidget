//----Gen Include Begin----
// Auto-generated includes
#include "Test1.h"
#include "ImUserWidget.h"
#include "ImButton.h"
#include "ImCheckBox.h"
#include "ImHorizontalBox.h"
#include "ImHorizontalSplitter.h"
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
    m_MenuList30 = new ImGuiWidget::ImHorizontalBox("m_MenuList30");
    MenuButton_Project31 = new ImGuiWidget::ImButton("MenuButton_Project31");
    Button_Project_Text33 = new ImGuiWidget::ImTextBlock("Button_Project_Text33");
    Button_NewUI34 = new ImGuiWidget::ImButton("Button_NewUI34");
    NewUI_MenuText36 = new ImGuiWidget::ImTextBlock("NewUI_MenuText36");
    Button_SaveUI41 = new ImGuiWidget::ImButton("Button_SaveUI41");
    SaveUI_MenuText42 = new ImGuiWidget::ImTextBlock("SaveUI_MenuText42");
    VSplitter02 = new ImGuiWidget::ImHorizontalSplitter("VSplitter02");
    MiddleBox4 = new ImGuiWidget::ImVerticalBox("MiddleBox4");
    Splitter01 = new ImGuiWidget::ImHorizontalSplitter("Splitter01");
    WidgetList_WidgetTreeSplitter5 = new ImGuiWidget::ImHorizontalSplitter("WidgetList_WidgetTreeSplitter5");
    WidgetList6 = new ImGuiWidget::ImVerticalBox("WidgetList6");
    Example_Button12 = new ImGuiWidget::ImWidget("Example_Button12");
    Example_TextBlock14 = new ImGuiWidget::ImWidget("Example_TextBlock14");
    Example_Image16 = new ImGuiWidget::ImWidget("Example_Image16");
    Example_CanvasPanel18 = new ImGuiWidget::ImWidget("Example_CanvasPanel18");
    Example_HorizontalBox20 = new ImGuiWidget::ImWidget("Example_HorizontalBox20");
    Example_VerticalBox22 = new ImGuiWidget::ImWidget("Example_VerticalBox22");
    Example_ComboBox24 = new ImGuiWidget::ImWidget("Example_ComboBox24");
    InputTextTest26 = new ImGuiWidget::ImWidget("InputTextTest26");
    CheckBoxTest27 = new ImGuiWidget::ImCheckBox("CheckBoxTest27");
    WidgetTreeView7 = new ImGuiWidget::ImWidget("WidgetTreeView7");
    ImCreatorUIPageManager9 = new ImGuiWidget::ImWidget("ImCreatorUIPageManager9");
    DetailList8 = new ImGuiWidget::ImWidget("DetailList8");
    BottomBox28 = new ImGuiWidget::ImVerticalBox("BottomBox28");
    LogList29 = new ImGuiWidget::ImScrollingTextList("LogList29");

    MainBox3->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    MainBox3->SetPropertyValue<bool>("HaveBorder", true);
    MainBox3->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));

    m_MenuList30->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(50, 50, 230, 255));
    m_MenuList30->SetPropertyValue<bool>("HaveBorder", true);
    m_MenuList30->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));

    MenuButton_Project31->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    MenuButton_Project31->SetPropertyValue<bool>("HaveBorder", true);
    ImGuiWidget::PropertyStruct* MenuButton_Project31_PressedStyle = MenuButton_Project31->GetPropertyPtr<ImGuiWidget::PropertyStruct>("PressedStyle");
    MenuButton_Project31_PressedStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(250, 220, 190, 120));
    MenuButton_Project31_PressedStyle->SetPropertyValue<bool>("HasBorder", false);
    MenuButton_Project31_PressedStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    MenuButton_Project31_PressedStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    MenuButton_Project31_PressedStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));
    MenuButton_Project31->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    ImGuiWidget::PropertyStruct* MenuButton_Project31_NormalStyle = MenuButton_Project31->GetPropertyPtr<ImGuiWidget::PropertyStruct>("NormalStyle");
    MenuButton_Project31_NormalStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(210, 180, 150, 120));
    MenuButton_Project31_NormalStyle->SetPropertyValue<bool>("HasBorder", false);
    MenuButton_Project31_NormalStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    MenuButton_Project31_NormalStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    MenuButton_Project31_NormalStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));
    MenuButton_Project31->SetPropertyValue<std::string>("TooltipText", "Project");
    ImGuiWidget::PropertyStruct* MenuButton_Project31_HoveredStyle = MenuButton_Project31->GetPropertyPtr<ImGuiWidget::PropertyStruct>("HoveredStyle");
    MenuButton_Project31_HoveredStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(230, 200, 170, 120));
    MenuButton_Project31_HoveredStyle->SetPropertyValue<bool>("HasBorder", false);
    MenuButton_Project31_HoveredStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    MenuButton_Project31_HoveredStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    MenuButton_Project31_HoveredStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));

    Button_Project_Text33->SetPropertyValue<std::string>("Text", "Project");
    Button_Project_Text33->SetPropertyValue<ImU32>("TextColor", IM_COL32(0, 0, 0, 255));
    Button_Project_Text33->SetPropertyValue<UnknownType>("HorizontalAlignment", "Center");
    Button_Project_Text33->SetPropertyValue<UnknownType>("VerticalAlignment", "Center");

    Button_NewUI34->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    Button_NewUI34->SetPropertyValue<bool>("HaveBorder", true);
    ImGuiWidget::PropertyStruct* Button_NewUI34_PressedStyle = Button_NewUI34->GetPropertyPtr<ImGuiWidget::PropertyStruct>("PressedStyle");
    Button_NewUI34_PressedStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(0, 51, 153, 255));
    Button_NewUI34_PressedStyle->SetPropertyValue<bool>("HasBorder", false);
    Button_NewUI34_PressedStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    Button_NewUI34_PressedStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    Button_NewUI34_PressedStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));
    Button_NewUI34->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    ImGuiWidget::PropertyStruct* Button_NewUI34_NormalStyle = Button_NewUI34->GetPropertyPtr<ImGuiWidget::PropertyStruct>("NormalStyle");
    Button_NewUI34_NormalStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(0, 102, 204, 255));
    Button_NewUI34_NormalStyle->SetPropertyValue<bool>("HasBorder", false);
    Button_NewUI34_NormalStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    Button_NewUI34_NormalStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    Button_NewUI34_NormalStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));
    Button_NewUI34->SetPropertyValue<std::string>("TooltipText", "");
    ImGuiWidget::PropertyStruct* Button_NewUI34_HoveredStyle = Button_NewUI34->GetPropertyPtr<ImGuiWidget::PropertyStruct>("HoveredStyle");
    Button_NewUI34_HoveredStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(51, 153, 255, 255));
    Button_NewUI34_HoveredStyle->SetPropertyValue<bool>("HasBorder", false);
    Button_NewUI34_HoveredStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    Button_NewUI34_HoveredStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    Button_NewUI34_HoveredStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));

    NewUI_MenuText36->SetPropertyValue<std::string>("Text", "NewUI");
    NewUI_MenuText36->SetPropertyValue<ImU32>("TextColor", IM_COL32(0, 0, 0, 255));
    NewUI_MenuText36->SetPropertyValue<UnknownType>("HorizontalAlignment", "Center");
    NewUI_MenuText36->SetPropertyValue<UnknownType>("VerticalAlignment", "Center");

    Button_SaveUI41->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    Button_SaveUI41->SetPropertyValue<bool>("HaveBorder", true);
    ImGuiWidget::PropertyStruct* Button_SaveUI41_PressedStyle = Button_SaveUI41->GetPropertyPtr<ImGuiWidget::PropertyStruct>("PressedStyle");
    Button_SaveUI41_PressedStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(0, 51, 153, 255));
    Button_SaveUI41_PressedStyle->SetPropertyValue<bool>("HasBorder", false);
    Button_SaveUI41_PressedStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    Button_SaveUI41_PressedStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    Button_SaveUI41_PressedStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));
    Button_SaveUI41->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    ImGuiWidget::PropertyStruct* Button_SaveUI41_NormalStyle = Button_SaveUI41->GetPropertyPtr<ImGuiWidget::PropertyStruct>("NormalStyle");
    Button_SaveUI41_NormalStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(0, 102, 204, 255));
    Button_SaveUI41_NormalStyle->SetPropertyValue<bool>("HasBorder", false);
    Button_SaveUI41_NormalStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    Button_SaveUI41_NormalStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    Button_SaveUI41_NormalStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));
    Button_SaveUI41->SetPropertyValue<std::string>("TooltipText", "");
    ImGuiWidget::PropertyStruct* Button_SaveUI41_HoveredStyle = Button_SaveUI41->GetPropertyPtr<ImGuiWidget::PropertyStruct>("HoveredStyle");
    Button_SaveUI41_HoveredStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(51, 153, 255, 255));
    Button_SaveUI41_HoveredStyle->SetPropertyValue<bool>("HasBorder", false);
    Button_SaveUI41_HoveredStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    Button_SaveUI41_HoveredStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    Button_SaveUI41_HoveredStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));

    SaveUI_MenuText42->SetPropertyValue<std::string>("Text", "Save");
    SaveUI_MenuText42->SetPropertyValue<ImU32>("TextColor", IM_COL32(0, 0, 0, 255));
    SaveUI_MenuText42->SetPropertyValue<UnknownType>("HorizontalAlignment", "Center");
    SaveUI_MenuText42->SetPropertyValue<UnknownType>("VerticalAlignment", "Center");

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








    InputTextTest26->SetPropertyValue<ImU32>("BorderColor", IM_COL32(100, 100, 100, 255));
    InputTextTest26->SetPropertyValue<std::string>("Text", "");
    InputTextTest26->SetPropertyValue<ImU32>("TextColor", IM_COL32(0, 0, 0, 255));
    InputTextTest26->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(255, 255, 255, 255));
    InputTextTest26->SetPropertyValue<float>("BorderThickness", 1.000000f);
    InputTextTest26->SetPropertyValue<float>("Rounding", 4.000000f);

    CheckBoxTest27->SetPropertyValue<float>("BoxRounding", 3.000000f);
    CheckBoxTest27->SetPropertyValue<ImU32>("CheckColor", IM_COL32(0, 0, 0, 255));
    CheckBoxTest27->SetPropertyValue<bool>("Checked", false);
    CheckBoxTest27->SetPropertyValue<ImU32>("BoxColor", IM_COL32(100, 100, 100, 255));
    CheckBoxTest27->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(255, 255, 255, 255));
    CheckBoxTest27->SetPropertyValue<float>("BorderThickness", 1.500000f);




    BottomBox28->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    BottomBox28->SetPropertyValue<bool>("HaveBorder", true);
    BottomBox28->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));

    LogList29->SetPropertyValue<float>("ScrollbarRounding", 0.000000f);
    LogList29->SetPropertyValue<ImU32>("TextColor", IM_COL32(0, 0, 0, 255));
    LogList29->SetPropertyValue<ImU32>("ScrollbarBackgroundColor", IM_COL32(200, 200, 200, 255));
    LogList29->SetPropertyValue<float>("LineSpacing", -1.000000f);
    LogList29->SetPropertyValue<float>("ScrollbarWidth", 14.000000f);
    LogList29->SetPropertyValue<ImU32>("ScrollbarGrabColor", IM_COL32(120, 120, 120, 255));
    LogList29->SetPropertyValue<float>("ScrollbarGrabMinHeight", 20.000000f);

    MainBox3->AddChild(m_MenuList30);
    MainBox3->AddChild(VSplitter02);
    m_MenuList30->AddChild(MenuButton_Project31);
    m_MenuList30->AddChild(Button_NewUI34);
    m_MenuList30->AddChild(Button_SaveUI41);
    MenuButton_Project31->AddChild(Button_Project_Text33);
    Button_NewUI34->AddChild(NewUI_MenuText36);
    Button_SaveUI41->AddChild(SaveUI_MenuText42);
    VSplitter02->AddChild(MiddleBox4);
    VSplitter02->AddChild(BottomBox28);
    MiddleBox4->AddChild(Splitter01);
    Splitter01->AddChild(WidgetList_WidgetTreeSplitter5);
    Splitter01->AddChild(ImCreatorUIPageManager9);
    Splitter01->AddChild(DetailList8);
    WidgetList_WidgetTreeSplitter5->AddChild(WidgetList6);
    WidgetList_WidgetTreeSplitter5->AddChild(WidgetTreeView7);
    WidgetList6->AddChild(Example_Button12);
    WidgetList6->AddChild(Example_TextBlock14);
    WidgetList6->AddChild(Example_Image16);
    WidgetList6->AddChild(Example_CanvasPanel18);
    WidgetList6->AddChild(Example_HorizontalBox20);
    WidgetList6->AddChild(Example_VerticalBox22);
    WidgetList6->AddChild(Example_ComboBox24);
    WidgetList6->AddChild(InputTextTest26);
    WidgetList6->AddChild(CheckBoxTest27);
    BottomBox28->AddChild(LogList29);

    SetRootWidget(MainBox3);
}
//----Gen Code End----

























