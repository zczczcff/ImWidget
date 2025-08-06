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
    MainBox = new ImGuiWidget::ImVerticalBox("MainBox");
    m_MenuList = new ImGuiWidget::ImHorizontalBox("m_MenuList");
    MenuButton_Project = new ImGuiWidget::ImButton("MenuButton_Project");
    Button_Project_Text = new ImGuiWidget::ImTextBlock("Button_Project_Text");
    Button_NewUI = new ImGuiWidget::ImButton("Button_NewUI");
    NewUI_MenuText = new ImGuiWidget::ImTextBlock("NewUI_MenuText");
    Button_SaveUI = new ImGuiWidget::ImButton("Button_SaveUI");
    SaveUI_MenuText = new ImGuiWidget::ImTextBlock("SaveUI_MenuText");
    Button_GenCode = new ImGuiWidget::ImButton("Button_GenCode");
    GenCode_MenuText = new ImGuiWidget::ImTextBlock("GenCode_MenuText");
    VSplitter0 = new ImGuiWidget::ImHorizontalSplitter("VSplitter0");
    MiddleBox = new ImGuiWidget::ImVerticalBox("MiddleBox");
    Splitter0 = new ImGuiWidget::ImHorizontalSplitter("Splitter0");
    WidgetList_WidgetTreeSplitter = new ImGuiWidget::ImHorizontalSplitter("WidgetList_WidgetTreeSplitter");
    WidgetList = new ImGuiWidget::ImVerticalBox("WidgetList");
    Example_Button = new ImGuiWidget::ImWidget("Example_Button");
    Example_TextBlock = new ImGuiWidget::ImWidget("Example_TextBlock");
    Example_Image = new ImGuiWidget::ImWidget("Example_Image");
    Example_CanvasPanel = new ImGuiWidget::ImWidget("Example_CanvasPanel");
    Example_HorizontalBox = new ImGuiWidget::ImWidget("Example_HorizontalBox");
    Example_VerticalBox = new ImGuiWidget::ImWidget("Example_VerticalBox");
    Example_ComboBox = new ImGuiWidget::ImWidget("Example_ComboBox");
    InputTextTest = new ImGuiWidget::ImWidget("InputTextTest");
    CheckBoxTest = new ImGuiWidget::ImCheckBox("CheckBoxTest");
    WidgetTreeView = new ImGuiWidget::ImWidget("WidgetTreeView");
    ImCreatorUIPageManager = new ImGuiWidget::ImWidget("ImCreatorUIPageManager");
    DetailList = new ImGuiWidget::ImWidget("DetailList");
    BottomBox = new ImGuiWidget::ImVerticalBox("BottomBox");
    LogList = new ImGuiWidget::ImScrollingTextList("LogList");

    MainBox->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    MainBox->SetPropertyValue<std::string>("Name", "MainBox");
    MainBox->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    MainBox->SetPropertyValue<bool>("HaveBorder", true);

    m_MenuList->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    m_MenuList->SetPropertyValue<std::string>("Name", "m_MenuList");
    m_MenuList->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(50, 50, 230, 255));
    m_MenuList->SetPropertyValue<bool>("HaveBorder", true);

    ImGuiWidget::PropertyStruct* MenuButton_Project_PressedStyle = MenuButton_Project->GetPropertyPtr<ImGuiWidget::PropertyStruct>("PressedStyle");
    MenuButton_Project_PressedStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(250, 220, 190, 120));
    MenuButton_Project_PressedStyle->SetPropertyValue<bool>("HasBorder", false);
    MenuButton_Project_PressedStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    MenuButton_Project_PressedStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    MenuButton_Project_PressedStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));
    MenuButton_Project->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    MenuButton_Project->SetPropertyValue<std::string>("Name", "MenuButton_Project");
    MenuButton_Project->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    MenuButton_Project->SetPropertyValue<bool>("HaveBorder", true);
    ImGuiWidget::PropertyStruct* MenuButton_Project_NormalStyle = MenuButton_Project->GetPropertyPtr<ImGuiWidget::PropertyStruct>("NormalStyle");
    MenuButton_Project_NormalStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(210, 180, 150, 120));
    MenuButton_Project_NormalStyle->SetPropertyValue<bool>("HasBorder", false);
    MenuButton_Project_NormalStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    MenuButton_Project_NormalStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    MenuButton_Project_NormalStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));
    MenuButton_Project->SetPropertyValue<std::string>("TooltipText", "Project");
    ImGuiWidget::PropertyStruct* MenuButton_Project_HoveredStyle = MenuButton_Project->GetPropertyPtr<ImGuiWidget::PropertyStruct>("HoveredStyle");
    MenuButton_Project_HoveredStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(230, 200, 170, 120));
    MenuButton_Project_HoveredStyle->SetPropertyValue<bool>("HasBorder", false);
    MenuButton_Project_HoveredStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    MenuButton_Project_HoveredStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    MenuButton_Project_HoveredStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));

    Button_Project_Text->SetPropertyValue<std::string>("Text", "Project");
    Button_Project_Text->SetPropertyValue<std::string>("Name", "Button_Project_Text");
    Button_Project_Text->SetPropertyValue<ImU32>("TextColor", IM_COL32(0, 0, 0, 255));
    Button_Project_Text->SetPropertyValue<UnknownType>("HorizontalAlignment", "Center");
    Button_Project_Text->SetPropertyValue<UnknownType>("VerticalAlignment", "Center");

    ImGuiWidget::PropertyStruct* Button_NewUI_PressedStyle = Button_NewUI->GetPropertyPtr<ImGuiWidget::PropertyStruct>("PressedStyle");
    Button_NewUI_PressedStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(0, 51, 153, 255));
    Button_NewUI_PressedStyle->SetPropertyValue<bool>("HasBorder", false);
    Button_NewUI_PressedStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    Button_NewUI_PressedStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    Button_NewUI_PressedStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));
    Button_NewUI->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    Button_NewUI->SetPropertyValue<std::string>("Name", "Button_NewUI");
    Button_NewUI->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    Button_NewUI->SetPropertyValue<bool>("HaveBorder", true);
    ImGuiWidget::PropertyStruct* Button_NewUI_NormalStyle = Button_NewUI->GetPropertyPtr<ImGuiWidget::PropertyStruct>("NormalStyle");
    Button_NewUI_NormalStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(0, 102, 204, 255));
    Button_NewUI_NormalStyle->SetPropertyValue<bool>("HasBorder", false);
    Button_NewUI_NormalStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    Button_NewUI_NormalStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    Button_NewUI_NormalStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));
    Button_NewUI->SetPropertyValue<std::string>("TooltipText", "");
    ImGuiWidget::PropertyStruct* Button_NewUI_HoveredStyle = Button_NewUI->GetPropertyPtr<ImGuiWidget::PropertyStruct>("HoveredStyle");
    Button_NewUI_HoveredStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(51, 153, 255, 255));
    Button_NewUI_HoveredStyle->SetPropertyValue<bool>("HasBorder", false);
    Button_NewUI_HoveredStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    Button_NewUI_HoveredStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    Button_NewUI_HoveredStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));

    NewUI_MenuText->SetPropertyValue<std::string>("Text", "NewUI");
    NewUI_MenuText->SetPropertyValue<std::string>("Name", "NewUI_MenuText");
    NewUI_MenuText->SetPropertyValue<ImU32>("TextColor", IM_COL32(0, 0, 0, 255));
    NewUI_MenuText->SetPropertyValue<UnknownType>("HorizontalAlignment", "Center");
    NewUI_MenuText->SetPropertyValue<UnknownType>("VerticalAlignment", "Center");

    ImGuiWidget::PropertyStruct* Button_SaveUI_PressedStyle = Button_SaveUI->GetPropertyPtr<ImGuiWidget::PropertyStruct>("PressedStyle");
    Button_SaveUI_PressedStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(0, 51, 153, 255));
    Button_SaveUI_PressedStyle->SetPropertyValue<bool>("HasBorder", false);
    Button_SaveUI_PressedStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    Button_SaveUI_PressedStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    Button_SaveUI_PressedStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));
    Button_SaveUI->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    Button_SaveUI->SetPropertyValue<std::string>("Name", "Button_SaveUI");
    Button_SaveUI->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    Button_SaveUI->SetPropertyValue<bool>("HaveBorder", true);
    ImGuiWidget::PropertyStruct* Button_SaveUI_NormalStyle = Button_SaveUI->GetPropertyPtr<ImGuiWidget::PropertyStruct>("NormalStyle");
    Button_SaveUI_NormalStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(0, 102, 204, 255));
    Button_SaveUI_NormalStyle->SetPropertyValue<bool>("HasBorder", false);
    Button_SaveUI_NormalStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    Button_SaveUI_NormalStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    Button_SaveUI_NormalStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));
    Button_SaveUI->SetPropertyValue<std::string>("TooltipText", "");
    ImGuiWidget::PropertyStruct* Button_SaveUI_HoveredStyle = Button_SaveUI->GetPropertyPtr<ImGuiWidget::PropertyStruct>("HoveredStyle");
    Button_SaveUI_HoveredStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(51, 153, 255, 255));
    Button_SaveUI_HoveredStyle->SetPropertyValue<bool>("HasBorder", false);
    Button_SaveUI_HoveredStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    Button_SaveUI_HoveredStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    Button_SaveUI_HoveredStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));

    SaveUI_MenuText->SetPropertyValue<std::string>("Text", "Save");
    SaveUI_MenuText->SetPropertyValue<std::string>("Name", "SaveUI_MenuText");
    SaveUI_MenuText->SetPropertyValue<ImU32>("TextColor", IM_COL32(0, 0, 0, 255));
    SaveUI_MenuText->SetPropertyValue<UnknownType>("HorizontalAlignment", "Center");
    SaveUI_MenuText->SetPropertyValue<UnknownType>("VerticalAlignment", "Center");

    ImGuiWidget::PropertyStruct* Button_GenCode_PressedStyle = Button_GenCode->GetPropertyPtr<ImGuiWidget::PropertyStruct>("PressedStyle");
    Button_GenCode_PressedStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(0, 51, 153, 255));
    Button_GenCode_PressedStyle->SetPropertyValue<bool>("HasBorder", false);
    Button_GenCode_PressedStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    Button_GenCode_PressedStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    Button_GenCode_PressedStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));
    Button_GenCode->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    Button_GenCode->SetPropertyValue<std::string>("Name", "Button_GenCode");
    Button_GenCode->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    Button_GenCode->SetPropertyValue<bool>("HaveBorder", true);
    ImGuiWidget::PropertyStruct* Button_GenCode_NormalStyle = Button_GenCode->GetPropertyPtr<ImGuiWidget::PropertyStruct>("NormalStyle");
    Button_GenCode_NormalStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(0, 102, 204, 255));
    Button_GenCode_NormalStyle->SetPropertyValue<bool>("HasBorder", false);
    Button_GenCode_NormalStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    Button_GenCode_NormalStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    Button_GenCode_NormalStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));
    Button_GenCode->SetPropertyValue<std::string>("TooltipText", "");
    ImGuiWidget::PropertyStruct* Button_GenCode_HoveredStyle = Button_GenCode->GetPropertyPtr<ImGuiWidget::PropertyStruct>("HoveredStyle");
    Button_GenCode_HoveredStyle->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(51, 153, 255, 255));
    Button_GenCode_HoveredStyle->SetPropertyValue<bool>("HasBorder", false);
    Button_GenCode_HoveredStyle->SetPropertyValue<float>("Rounding", 0.000000f);
    Button_GenCode_HoveredStyle->SetPropertyValue<float>("BorderThickness", 1.000000f);
    Button_GenCode_HoveredStyle->SetPropertyValue<ImU32>("BorderColor", IM_COL32(255, 255, 255, 255));

    GenCode_MenuText->SetPropertyValue<std::string>("Text", "Gen Code");
    GenCode_MenuText->SetPropertyValue<std::string>("Name", "GenCode_MenuText");
    GenCode_MenuText->SetPropertyValue<ImU32>("TextColor", IM_COL32(0, 0, 0, 255));
    GenCode_MenuText->SetPropertyValue<UnknownType>("HorizontalAlignment", "Center");
    GenCode_MenuText->SetPropertyValue<UnknownType>("VerticalAlignment", "Center");

    VSplitter0->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    VSplitter0->SetPropertyValue<std::string>("Name", "VSplitter0");
    VSplitter0->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    VSplitter0->SetPropertyValue<bool>("HaveBorder", true);
    ImGuiWidget::PropertyStruct* VSplitter0_SplitterStyle = VSplitter0->GetPropertyPtr<ImGuiWidget::PropertyStruct>("SplitterStyle");
    VSplitter0_SplitterStyle->SetPropertyValue<float>("BarHeight", 4.000000f);
    VSplitter0_SplitterStyle->SetPropertyValue<ImU32>("Color", IM_COL32(100, 100, 100, 255));
    VSplitter0_SplitterStyle->SetPropertyValue<ImU32>("HoveredColor", IM_COL32(120, 120, 120, 255));
    VSplitter0_SplitterStyle->SetPropertyValue<ImU32>("ActiveColor", IM_COL32(150, 150, 150, 255));
    VSplitter0_SplitterStyle->SetPropertyValue<float>("Rounding", 0.000000f);

    MiddleBox->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    MiddleBox->SetPropertyValue<std::string>("Name", "MiddleBox");
    MiddleBox->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    MiddleBox->SetPropertyValue<bool>("HaveBorder", true);

    Splitter0->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    Splitter0->SetPropertyValue<float>("BarWidth", 4.000000f);
    Splitter0->SetPropertyValue<std::string>("Name", "Splitter0");
    Splitter0->SetPropertyValue<ImU32>("BarHoveredColor", IM_COL32(120, 120, 120, 255));
    Splitter0->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    Splitter0->SetPropertyValue<ImU32>("BarColor", IM_COL32(100, 100, 100, 255));
    Splitter0->SetPropertyValue<bool>("HaveBorder", true);
    Splitter0->SetPropertyValue<ImU32>("BarActiveColor", IM_COL32(150, 150, 150, 255));
    Splitter0->SetPropertyValue<float>("BarRounding", 0.000000f);

    WidgetList_WidgetTreeSplitter->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    WidgetList_WidgetTreeSplitter->SetPropertyValue<std::string>("Name", "WidgetList_WidgetTreeSplitter");
    WidgetList_WidgetTreeSplitter->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    WidgetList_WidgetTreeSplitter->SetPropertyValue<bool>("HaveBorder", true);
    ImGuiWidget::PropertyStruct* WidgetList_WidgetTreeSplitter_SplitterStyle = WidgetList_WidgetTreeSplitter->GetPropertyPtr<ImGuiWidget::PropertyStruct>("SplitterStyle");
    WidgetList_WidgetTreeSplitter_SplitterStyle->SetPropertyValue<float>("BarHeight", 4.000000f);
    WidgetList_WidgetTreeSplitter_SplitterStyle->SetPropertyValue<ImU32>("Color", IM_COL32(100, 100, 100, 255));
    WidgetList_WidgetTreeSplitter_SplitterStyle->SetPropertyValue<ImU32>("HoveredColor", IM_COL32(120, 120, 120, 255));
    WidgetList_WidgetTreeSplitter_SplitterStyle->SetPropertyValue<ImU32>("ActiveColor", IM_COL32(150, 150, 150, 255));
    WidgetList_WidgetTreeSplitter_SplitterStyle->SetPropertyValue<float>("Rounding", 0.000000f);

    WidgetList->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    WidgetList->SetPropertyValue<std::string>("Name", "WidgetList");
    WidgetList->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    WidgetList->SetPropertyValue<bool>("HaveBorder", true);

    Example_Button->SetPropertyValue<std::string>("Name", "Example_Button");

    Example_TextBlock->SetPropertyValue<std::string>("Name", "Example_TextBlock");

    Example_Image->SetPropertyValue<std::string>("Name", "Example_Image");

    Example_CanvasPanel->SetPropertyValue<std::string>("Name", "Example_CanvasPanel");

    Example_HorizontalBox->SetPropertyValue<std::string>("Name", "Example_HorizontalBox");

    Example_VerticalBox->SetPropertyValue<std::string>("Name", "Example_VerticalBox");

    Example_ComboBox->SetPropertyValue<std::string>("Name", "Example_ComboBox");

    InputTextTest->SetPropertyValue<ImU32>("BorderColor", IM_COL32(100, 100, 100, 255));
    InputTextTest->SetPropertyValue<std::string>("Text", "");
    InputTextTest->SetPropertyValue<ImU32>("TextColor", IM_COL32(0, 0, 0, 255));
    InputTextTest->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(255, 255, 255, 255));
    InputTextTest->SetPropertyValue<float>("BorderThickness", 1.000000f);
    InputTextTest->SetPropertyValue<float>("Rounding", 4.000000f);

    CheckBoxTest->SetPropertyValue<float>("BoxRounding", 3.000000f);
    CheckBoxTest->SetPropertyValue<ImU32>("CheckColor", IM_COL32(0, 0, 0, 255));
    CheckBoxTest->SetPropertyValue<bool>("Checked", false);
    CheckBoxTest->SetPropertyValue<ImU32>("BoxColor", IM_COL32(100, 100, 100, 255));
    CheckBoxTest->SetPropertyValue<ImU32>("BackgroundColor", IM_COL32(255, 255, 255, 255));
    CheckBoxTest->SetPropertyValue<float>("BorderThickness", 1.500000f);

    WidgetTreeView->SetPropertyValue<std::string>("Name", "WidgetTreeView");

    ImCreatorUIPageManager->SetPropertyValue<std::string>("Name", "ImCreatorUIPageManager");

    DetailList->SetPropertyValue<std::string>("Name", "DetailList");

    BottomBox->SetPropertyValue<ImU32>("BorderColor", IM_COL32(0, 0, 0, 255));
    BottomBox->SetPropertyValue<std::string>("Name", "BottomBox");
    BottomBox->SetPropertyValue<ImU32>("BackGroundColor", IM_COL32(255, 255, 255, 255));
    BottomBox->SetPropertyValue<bool>("HaveBorder", true);

    LogList->SetPropertyValue<std::string>("Name", "LogList");
    LogList->SetPropertyValue<float>("ScrollbarRounding", 0.000000f);
    LogList->SetPropertyValue<ImU32>("TextColor", IM_COL32(0, 0, 0, 255));
    LogList->SetPropertyValue<ImU32>("ScrollbarBackgroundColor", IM_COL32(200, 200, 200, 255));
    LogList->SetPropertyValue<float>("LineSpacing", -1.000000f);
    LogList->SetPropertyValue<float>("ScrollbarWidth", 14.000000f);
    LogList->SetPropertyValue<ImU32>("ScrollbarGrabColor", IM_COL32(120, 120, 120, 255));
    LogList->SetPropertyValue<float>("ScrollbarGrabMinHeight", 20.000000f);

    MainBox->AddChild(m_MenuList);
    MainBox->AddChild(VSplitter0);
    m_MenuList->AddChild(MenuButton_Project);
    m_MenuList->AddChild(Button_NewUI);
    m_MenuList->AddChild(Button_SaveUI);
    m_MenuList->AddChild(Button_GenCode);
    MenuButton_Project->AddChild(Button_Project_Text);
    Button_NewUI->AddChild(NewUI_MenuText);
    Button_SaveUI->AddChild(SaveUI_MenuText);
    Button_GenCode->AddChild(GenCode_MenuText);
    VSplitter0->AddChild(MiddleBox);
    VSplitter0->AddChild(BottomBox);
    MiddleBox->AddChild(Splitter0);
    Splitter0->AddChild(WidgetList_WidgetTreeSplitter);
    Splitter0->AddChild(ImCreatorUIPageManager);
    Splitter0->AddChild(DetailList);
    WidgetList_WidgetTreeSplitter->AddChild(WidgetList);
    WidgetList_WidgetTreeSplitter->AddChild(WidgetTreeView);
    WidgetList->AddChild(Example_Button);
    WidgetList->AddChild(Example_TextBlock);
    WidgetList->AddChild(Example_Image);
    WidgetList->AddChild(Example_CanvasPanel);
    WidgetList->AddChild(Example_HorizontalBox);
    WidgetList->AddChild(Example_VerticalBox);
    WidgetList->AddChild(Example_ComboBox);
    WidgetList->AddChild(InputTextTest);
    WidgetList->AddChild(CheckBoxTest);
    BottomBox->AddChild(LogList);

    SetRootWidget(MainBox);
}
//----Gen Code End----





































