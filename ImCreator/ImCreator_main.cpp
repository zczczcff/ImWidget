#include "Application/ImApplication.h"
#include "ImWidget/ImButton.h"
#include "ImWidget/ImVerticalBox.h"
#include "ImWidget/ImVerticalSplitter.h"
#include "ImWidget/ImHorizontalSplitter.h"
#include "ImWidget/ImHorizontalBox.h"
#include "ImWidget/ImTextBlock.h"
#include "ImWidget/ImSingleLineInputText.h"
#include "ImWidget/ImCanvasPanel.h"
#include "ImWidget/ImSlider.h"
#include "ImWidget/ImScrollBox.h"
#include "ImWidget/ImResizableBox.h"
#include "ImWidget/ImImage.h"
#include "ImWidget/ImMenuButton.h"
#include "ImWidget/ImMultiLineTextBlock.h"
#include "ImWidget/ImScrollingTextList.h"
#include "ImWidget/ImInputText.h"
#include "ImWidget/ImCheckBox.h"
#include "ImWidget/ImWidgetCodeGenerator.h"

#include "ImWidget/ImWidgetSerializer.h"

#include "ImWindows/ImMenuButton.h"
#include "ImWindows/ImPageManager.h"

#include "WidgetTreeView.h"

#include "ImExampleWidget.h"
#include "ImDesignPanel.h"
#include "ImCreator_DetailList.h"
namespace ImGuiWidget
{
    Application* GlobalApp;
}

class MyApp : public Application
{
public:
    using Application::Application;
    ImGuiWidget::ImVerticalBox* m_Box;
    ImGuiWidget::ImHorizontalSplitter* m_MiddleSplitter;
    ImGuiWidget::ImVerticalSplitter* m_VSplitter;
    //ImGuiWidget::ImCanvasPanel* m_Canvas;
    ImGuiWidget::ImHorizontalBox* m_MenuList;
    ImGuiWidget::ImVerticalBox* m_MainBox;
    ImGuiWidget::ImVerticalBox* m_MiddleBox;
    ImGuiWidget::ImVerticalBox* m_BottomBox;
    //ImGuiWidget::ImMenuButton* Button_Project;
    //ImGuiWidget::ImVerticalBox* m_Menu_ProjectMenu;
    ImWindows::ImMenuButton* m_MenuButton_Project;
    ImWindows::ImMenuButton* m_MenuButton_Project_History;
    ImWindows::ImMenuButton* m_MenuButton_Project_History1;
    ImWindows::ImPageManager* m_CenterPageManager;

    ImGuiWidget::ImVerticalSplitter* m_WidgetList_WidgetTreeSplitter;
    ImGuiWidget::ImVerticalBox* m_WidgetList;
    WidgetTreeView* m_WidgetTreeView;

    DesiginPanel* m_DesiginPanel;

    DetailList* m_DetailList;
    //ImGuiWidget::ImVerticalBox* m_DetailList;

    ImGuiWidget::ImScrollingTextList* m_LogList;

    ImGuiWidget::ImInputText* m_InputTextTest;
    ImGuiWidget::ImCheckBox* m_CheckBoxTest;

    //ImGuiWidget::ImScrollBox* m_LogBox;
    ExampleWidget* m_Example_Button;
    ExampleWidget* m_Example_TextBlock;
    ExampleWidget* m_Example_Image;

    ExampleWidget* m_Example_ImCanvasPanel;
    ExampleWidget* m_Example_HorizontalBox;
    ExampleWidget* m_Example_VerticalBox;
    void Init()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        // 设置工具提示背景颜色（RGBA，这里设置为半透明的蓝色）
        style.Colors[ImGuiCol_PopupBg] = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);
        // 设置工具提示文本颜色
        style.Colors[ImGuiCol_Text] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);


        m_Box = new ImGuiWidget::ImVerticalBox("Box0");
        m_MiddleSplitter = new ImGuiWidget::ImHorizontalSplitter("Splitter0");
        m_VSplitter = new ImGuiWidget::ImVerticalSplitter("VSplitter0");
        m_MainBox = new ImGuiWidget::ImVerticalBox("MainBox");

        m_MiddleBox= new ImGuiWidget::ImVerticalBox("MiddleBox");
        m_MiddleBox->AddChildToVerticalBox(m_MiddleSplitter);


        m_WidgetList_WidgetTreeSplitter= new ImGuiWidget::ImVerticalSplitter("WidgetList_WidgetTreeSplitter");
        m_WidgetList = new ImGuiWidget::ImVerticalBox("WidgetList");
        m_WidgetTreeView = new WidgetTreeView("WidgetTreeView");
        m_WidgetList_WidgetTreeSplitter->AddPart(m_WidgetList);
        m_WidgetList_WidgetTreeSplitter->AddPart(m_WidgetTreeView);

        m_DesiginPanel = new DesiginPanel("CenterPanel");
       // m_DetailList= new ImGuiWidget::ImVerticalBox("DetailList");
        m_DetailList = new DetailList("DetailList");

        m_DesiginPanel->SetOnSelected([this](ImGuiWidget::ImWidget* selectedwidget) 
            {
                if (selectedwidget)
                {
                    m_DetailList->SetCurrentWidget(selectedwidget);
                }
            });
        m_DesiginPanel->SetOnDragWidgetOn([this](ImGuiWidget::ImWidget* NewWidget) 
            {
                if (!m_WidgetTreeView->GetTargetWidget())
                {
                    m_WidgetTreeView->SetTargetWidget(NewWidget);
                }
                else
                {
                    m_WidgetTreeView->Refresh();
                }
            });
        m_CenterPageManager = new ImWindows::ImPageManager("CenterPageManager",ImWindows::TabDockPosition::Top);
        m_CenterPageManager->AddPage("test", m_DesiginPanel);
        m_CenterPageManager->SetTabBarThickness(20.f);

        m_MiddleSplitter->AddPart(m_WidgetList_WidgetTreeSplitter);
        m_MiddleSplitter->AddPart(m_CenterPageManager)->Ratio = 5.0f;
        m_MiddleSplitter->AddPart(m_DetailList);

        m_Example_Button = new ExampleWidget("Example_Button", "Button", WidgetType::ImButton);
        m_Example_TextBlock = new ExampleWidget("Example_TextBlock", "TextBlock", WidgetType::ImTextBlock);
        m_Example_Image = new ExampleWidget("Example_Image", "Image", WidgetType::ImImage);
        m_Example_ImCanvasPanel = new ExampleWidget("Example_CanvasPanel", "CanvasPanel", WidgetType::ImCanvasPanel);
        m_Example_HorizontalBox = new ExampleWidget("Example_HorizontalBox", "HorizontalBox", WidgetType::ImHorizontalBox);
        m_Example_VerticalBox = new ExampleWidget("Example_VerticalBox", "VerticalBox", WidgetType::ImVerticalBox);
        m_InputTextTest = new ImGuiWidget::ImInputText("InputTextTest");
        m_CheckBoxTest = new ImGuiWidget::ImCheckBox("CheckBoxTest");
        //ImGuiWidget::ImButton* button_test = new ImGuiWidget::ImButton("buttontest");
        m_WidgetList->AddChildToVerticalBox(m_Example_Button)->SetIfAutoSize(false);
        m_WidgetList->AddChildToVerticalBox(m_Example_TextBlock)->SetIfAutoSize(false);
        m_WidgetList->AddChildToVerticalBox(m_Example_Image)->SetIfAutoSize(false);
        m_WidgetList->AddChildToVerticalBox(m_Example_ImCanvasPanel)->SetIfAutoSize(false);
        m_WidgetList->AddChildToVerticalBox(m_Example_HorizontalBox)->SetIfAutoSize(false);
        m_WidgetList->AddChildToVerticalBox(m_Example_VerticalBox)->SetIfAutoSize(false);
        m_WidgetList->AddChildToVerticalBox(m_InputTextTest)->SetIfAutoSize(false);
        m_WidgetList->AddChildToVerticalBox(m_CheckBoxTest)->SetIfAutoSize(false);

        m_BottomBox=new ImGuiWidget::ImVerticalBox("BottomBox");
        m_VSplitter->AddPart(m_MiddleBox)->Ratio = 4.0f;
        m_VSplitter->AddPart(m_BottomBox);

        m_LogList = new ImGuiWidget::ImScrollingTextList("LogList");
        for (int i = 0; i < 20; i++)
        {
            m_LogList->AddItem("abcdesafsafefa tesrt skanf isf nin aifn ansifasf nia fni asfaf",IM_COL32(150,100,10*i,255));
        }
        

        m_BottomBox->AddChildToVerticalBox(m_LogList)->SetIfAutoSize(true);

        m_MenuList = new ImGuiWidget::ImHorizontalBox("m_MenuList");
        m_MenuList->SetBackGroundColor(IM_COL32(50, 50, 230, 255));
        //Button_Project = new ImGuiWidget::ImMenuButton("Button_Project");

        m_MenuButton_Project = new ImWindows::ImMenuButton("MenuButton_Project");

        m_MenuButton_Project->GetNormalStyle().BackgroundColor = IM_COL32(210, 180, 150, 120);
        m_MenuButton_Project->GetHoveredStyle().BackgroundColor = IM_COL32(230, 200, 170, 120);
        m_MenuButton_Project->GetPressedStyle().BackgroundColor = IM_COL32(250, 220, 190, 120);
        m_MenuButton_Project->SetTooltipText("Project");
        m_MenuButton_Project_History = new ImWindows::ImMenuButton("MenuButton_Project_History");
        m_MenuButton_Project_History->SetDockDirection(ImWindows::MenuDockDirection::Dock_Right);
        m_MenuButton_Project_History1 = new ImWindows::ImMenuButton("MenuButton_Project_History1");
        m_MenuButton_Project_History1->SetDockDirection(ImWindows::MenuDockDirection::Dock_Right);
        ImGuiWidget::ImTextBlock* Button_Project_Text = new ImGuiWidget::ImTextBlock("Button_Project_Text");
        Button_Project_Text->SetText("Project");
        
        m_MenuButton_Project->SetContent(Button_Project_Text);
        //ImGuiWidget::ImButton* m_MenuButton_Project;
        //auto properties = m_MenuButton_Project->GetProperties();
        //for (auto& SingleProperty : properties)
        //{
        //    if (SingleProperty.name == "TooltipText")
        //    {
        //        //设置TooltipText
        //        std::string Temp_TooltipText = "Test";
        //        SingleProperty.setter(&Temp_TooltipText);
        //    }
        //    else if (SingleProperty.name == "NormalStyle")
        //    {
        //        //此处递归展开生成代码
        //    }
        //}
        
        //ImGuiWidget::LoadWidgetFromFile(m_MenuButton_Project, "test.imui");
        
        

        m_MenuList->AddChildToHorizontalBox(m_MenuButton_Project)->SetIfAutoSize(false);
        m_MainBox->AddChildToVerticalBox(m_MenuList)->SetIfAutoSize(false);
        m_MainBox->AddChildToVerticalBox(m_VSplitter);


        m_MenuButton_Project->AddMenuOption(m_MenuButton_Project_History);
        m_MenuButton_Project->AddMenuOption(m_MenuButton_Project_History1);
        //m_Menu_ProjectMenu = new ImGuiWidget::ImVerticalBox("Menu_ProjectMenu");
        //Button_Project->SetMenu(m_Menu_ProjectMenu);
		ImGuiWidget::ImButton* button0 = new ImGuiWidget::ImButton("button0");
		ImGuiWidget::ImButton* button1 = new ImGuiWidget::ImButton("button1");
		ImGuiWidget::ImButton* button2 = new ImGuiWidget::ImButton("button2");

        ImGuiWidget::ImButton* button3 = new ImGuiWidget::ImButton("button0");
        ImGuiWidget::ImButton* button4 = new ImGuiWidget::ImButton("button1");
        ImGuiWidget::ImButton* button5 = new ImGuiWidget::ImButton("button2");

        m_MenuButton_Project_History->AddMenuOption(button0);
        m_MenuButton_Project_History->AddMenuOption(button1);
        m_MenuButton_Project_History->AddMenuOption(button2);

        m_MenuButton_Project_History1->AddMenuOption(button3);
        m_MenuButton_Project_History1->AddMenuOption(button4);
        m_MenuButton_Project_History1->AddMenuOption(button5);

        ImGuiWidget::SaveWidgetTreeToFile(m_MainBox, "test.imui");
        ImGuiWidget::ExportWidgetTreeToFile(m_MainBox, "test.cpp", "Test");
    }
    void Render() override
    {
        m_MainBox->SetPosition(ImVec2(1.f, 1.f));
        m_MainBox->SetSize(ImGui::GetWindowSize()-ImVec2(2.f,2.f));
        m_MainBox->Render();

        //if (bRenderMenu_ProjectMenu)
        //{
        //    m_Menu_ProjectMenu->Render();
        //}

        //m_Canvas->Render();
    }
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    MyApp app(hInstance, nCmdShow);
    ImGuiWidget::GlobalApp = &app;
    if (!app.Initialize())
        return 1;
    app.Init();
    app.Run();
    return 0;
}
