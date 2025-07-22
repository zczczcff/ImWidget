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

#include "ImWindows/ImMenuButton.h"
#include "ImWindows/ImPageManager.h"

#include "ImExampleWidget.h"
#include "ImDesignPanel.h"
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
    ImGuiWidget::ImVerticalBox* m_WidgetList;
    DesiginPanel* m_CenterPanel;
    ImGuiWidget::ImVerticalBox* m_DetailList;
    ImGuiWidget::ImMultiLineTextBlock* m_LogText;
    ImGuiWidget::ImMultiLineTextBlock* m_LogText2;
    ExampleWidget* m_Example_Button;
    void Init()
    {
        m_Box = new ImGuiWidget::ImVerticalBox("Box0");
        m_MiddleSplitter = new ImGuiWidget::ImHorizontalSplitter("Splitter0");
        m_VSplitter = new ImGuiWidget::ImVerticalSplitter("VSplitter0");
        m_MainBox = new ImGuiWidget::ImVerticalBox("MainBox");

        m_MiddleBox= new ImGuiWidget::ImVerticalBox("MiddleBox");
        m_MiddleBox->AddChildToVerticalBox(m_MiddleSplitter);

        m_WidgetList = new ImGuiWidget::ImVerticalBox("WidgetList");
        m_CenterPanel = new DesiginPanel("CenterPanel");
        m_DetailList= new ImGuiWidget::ImVerticalBox("DetailList");

        m_CenterPageManager = new ImWindows::ImPageManager("CenterPageManager",ImWindows::TabDockPosition::Top);
        m_CenterPageManager->AddPage("test", m_CenterPanel);
        m_CenterPageManager->SetTabBarThickness(20.f);

        m_MiddleSplitter->AddPart(m_WidgetList);
        m_MiddleSplitter->AddPart(m_CenterPageManager)->Ratio = 5.0f;
        m_MiddleSplitter->AddPart(m_DetailList);

        m_Example_Button = new ExampleWidget("Example_Button", "Button");
        m_WidgetList->AddChildToVerticalBox(m_Example_Button);

        


        m_BottomBox=new ImGuiWidget::ImVerticalBox("BottomBox");
        m_VSplitter->AddPart(m_MiddleBox)->Ratio = 4.0f;
        m_VSplitter->AddPart(m_BottomBox);
        m_LogText = new  ImGuiWidget::ImMultiLineTextBlock("LogText");
        m_LogText2=new ImGuiWidget::ImMultiLineTextBlock("LogText2");
        m_BottomBox->AddChildToVerticalBox(m_LogText)->SetIfAutoSize(false);
        m_BottomBox->AddChildToVerticalBox(m_LogText2)->SetIfAutoSize(false);
        m_LogText->SetText("abcde fghij klmn opq rst uvw xyz");
        m_LogText2->SetText("abcde fghij klmn opq rst uvw xyz");
        m_MenuList = new ImGuiWidget::ImHorizontalBox("m_MenuList");
        m_MenuList->SetBackGroundColor(IM_COL32(50, 50, 230, 255));
        //Button_Project = new ImGuiWidget::ImMenuButton("Button_Project");

        m_MenuButton_Project = new ImWindows::ImMenuButton("MenuButton_Project");

        m_MenuButton_Project->GetNormalStyle().BackgroundColor = IM_COL32(210, 180, 150, 120);
        m_MenuButton_Project->GetHoveredStyle().BackgroundColor = IM_COL32(230, 200, 170, 120);
        m_MenuButton_Project->GetPressedStyle().BackgroundColor = IM_COL32(250, 220, 190, 120);

        m_MenuButton_Project_History = new ImWindows::ImMenuButton("MenuButton_Project_History");
        m_MenuButton_Project_History->SetDockDirection(ImWindows::MenuDockDirection::Dock_Right);
        m_MenuButton_Project_History1 = new ImWindows::ImMenuButton("MenuButton_Project_History1");
        m_MenuButton_Project_History1->SetDockDirection(ImWindows::MenuDockDirection::Dock_Right);
        ImGuiWidget::ImTextBlock* Button_Project_Text = new ImGuiWidget::ImTextBlock("Button_Project_Text");
        Button_Project_Text->SetText("Project");
        
        m_MenuButton_Project->SetContent(Button_Project_Text);
        

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
        //m_Menu_ProjectMenu->AddChildToVerticalBox(button0);
        //m_Menu_ProjectMenu->AddChildToVerticalBox(button1);
        //m_Menu_ProjectMenu->AddChildToVerticalBox(button2);
        //Button_Project->SetOnPressed([this]()
        //    {
        //        bRenderMenu_ProjectMenu = !bRenderMenu_ProjectMenu;
        //        m_Menu_ProjectMenu->SetPosition(Button_Project->GetPosition() + ImVec2(0.f, Button_Project->GetSize().y));
        //        m_Menu_ProjectMenu->SetSize(ImVec2(50, 200));
        //    });
        

        //TextBlock0->SetText("Test");
        //m_MiddleSplitter->SetSize(ImVec2(600, 300));


        //ImGuiWidget::ImVerticalBox* Box1 = new ImGuiWidget::ImVerticalBox("Box1");
        //ImGuiWidget::ImButton* button3 = new ImGuiWidget::ImButton("button3");
        //ImGuiWidget::ImButton* button4 = new ImGuiWidget::ImButton("button4");
        //ImGuiWidget::ImButton* button5 = new ImGuiWidget::ImButton("button5");
        //ImGuiWidget::ImSingleLineInPutText* InPutText0 = new ImGuiWidget::ImSingleLineInPutText("InputText0");
        //ImGuiWidget::ImSlider* Slider0 = new ImGuiWidget::ImSlider("Slider0");
        //ImGuiWidget::ImScrollBox* ScrollBox0 = new ImGuiWidget::ImScrollBox("ScrollBox0");
        //ScrollBox0->SetSize(ImVec2(500.f, 500.f));
        //ImGuiWidget::ImCanvasPanel* CanvasPanel1 = new ImGuiWidget::ImCanvasPanel("CanvasPanel1");
        //ScrollBox0->SetChild(CanvasPanel1);
        //CanvasPanel1->SetSize(ImVec2(300.f, 300.f));
        //ImGuiWidget::ImButton* button6 = new ImGuiWidget::ImButton("button6");
        //ImGuiWidget::ImButton* button7 = new ImGuiWidget::ImButton("button7");

        //ImGuiWidget::ImResizableBox* ResizableBox0 = new ImGuiWidget::ImResizableBox("ResizableBox0");
        //ImGuiWidget::ImImage* Image0 = new ImGuiWidget::ImImage("Image0", "./Resource/preview.jpg");
        //Image0->SetAlpha(0.5);
        //CanvasPanel1->AddChildToCanvasPanel(button6)->SetSlotPosAndSize(ImVec2(100.f, 100.f), ImVec2(20.f, 20.f));
        //CanvasPanel1->AddChildToCanvasPanel(button7)->SetSlotPosAndSize(ImVec2(280.f, 290.f), ImVec2(20.f, 20.f));


        //m_Box->AddChildToVerticalBox(button0);
        //m_Box->AddChildToVerticalBox(button1);
        //m_Box->AddChildToVerticalBox(button2)->SetIfAutoSize(false);
        //Box1->AddChildToVerticalBox(button3);
        //Box1->AddChildToVerticalBox(button4);
        ////button4->SetContent(TextBlock0);
        //Box1->AddChildToVerticalBox(InPutText0);
        ////Box1->AddChildToVerticalBox(TextBlock0);
        ////m_Box->AddChildToVerticalBox(Box1);
        //m_Box->SetSize(ImVec2(100, 600));

        ////m_Splitter->SetLeftPart(m_Box);
        ////m_Splitter->SetRightPart(Box1);
        //m_MiddleSplitter->AddPart(m_Box);
        //m_MiddleSplitter->AddPart(Box1);
        //m_MiddleSplitter->AddPart(ScrollBox0);
        ////m_Canvas->AddChildToCanvasPanel(button5)->SetSlotPosAndSize(ImVec2(500, 500), ImVec2(50, 50));
        ////m_Canvas->AddChildToCanvasPanel(Slider0)->SetSlotPosAndSize(ImVec2(550, 550), ImVec2(200, 10));
        ////m_Canvas->AddChildToCanvasPanel(ScrollBox0)->SetSlotPosAndSize(ImVec2(100, 100), ImVec2(400, 400));

        //m_Canvas->AddChildToCanvasPanel(ResizableBox0)->SetSlotPosAndSize(ImVec2(400, 400), ImVec2(400, 400));

        //m_Canvas->AddChildToCanvasPanel(m_MiddleSplitter)->SetSlotPosAndSize(ImVec2(500, 500), ImVec2(400, 200));
        //ResizableBox0->SetContent(Image0);
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
