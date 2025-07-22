#include "Application/ImApplication.h"
#include "ImWidget/ImButton.h"
#include "ImWidget/ImVerticalBox.h"
#include "ImWidget/ImHorizontalSplitter.h"
#include "ImWidget/ImTextBlock.h"
#include "ImWidget/ImSingleLineInputText.h"
#include "ImWidget/ImCanvasPanel.h"
#include "ImWidget/ImSlider.h"
#include "ImWidget/ImScrollBox.h"
#include "ImWidget/ImResizableBox.h"
#include "ImWidget/ImImage.h"

//namespace ImGuiWidget
//{
//    Application* GlobalApp;
//}
//
//class MyApp : public Application
//{
//public:
//    using Application::Application;
//    ImGuiWidget::ImVerticalBox* m_Box;
//    ImGuiWidget::ImHorizontalSplitter* m_Splitter;
//    ImGuiWidget::ImCanvasPanel* m_Canvas;
//    //ImTextureID TestTexture;
//    void Init()
//    {
//        m_Box = new ImGuiWidget::ImVerticalBox("Box0");
//        m_Splitter = new ImGuiWidget::ImHorizontalSplitter("Splitter0");
//        m_Canvas = new ImGuiWidget::ImCanvasPanel("Canvas0");
//
//        m_Canvas->SetPosition(ImVec2(0,0));
//        m_Canvas->SetSize(ImVec2(1000, 800));
//        //m_Canvas->AddChildToCanvasPanel(m_Splitter)->SetSlotPosAndSize(ImVec2(0, 0), ImVec2(600, 300));
//        ImGuiWidget::ImTextBlock* TextBlock0 = new ImGuiWidget::ImTextBlock("TextBlock0");
//        TextBlock0->SetText("Test");
//        m_Splitter->SetSize(ImVec2(600, 300));
//
//        ImGuiWidget::ImButton* button0 = new ImGuiWidget::ImButton("button0");
//        ImGuiWidget::ImButton* button1 = new ImGuiWidget::ImButton("button1");
//        ImGuiWidget::ImButton* button2 = new ImGuiWidget::ImButton("button2");
//        ImGuiWidget::ImVerticalBox* Box1= new ImGuiWidget::ImVerticalBox("Box1");
//        ImGuiWidget::ImButton* button3 = new ImGuiWidget::ImButton("button3");
//        ImGuiWidget::ImButton* button4 = new ImGuiWidget::ImButton("button4");
//        ImGuiWidget::ImButton* button5 = new ImGuiWidget::ImButton("button5");
//        ImGuiWidget::ImSingleLineInPutText* InPutText0 = new ImGuiWidget::ImSingleLineInPutText("InputText0");
//        ImGuiWidget::ImSlider* Slider0 = new ImGuiWidget::ImSlider("Slider0");
//        ImGuiWidget::ImScrollBox* ScrollBox0 = new ImGuiWidget::ImScrollBox("ScrollBox0");
//        ScrollBox0->SetSize(ImVec2(500.f, 500.f));
//        ImGuiWidget::ImCanvasPanel* CanvasPanel1 = new ImGuiWidget::ImCanvasPanel("CanvasPanel1");
//        ScrollBox0->SetContent(CanvasPanel1);
//        CanvasPanel1->SetSize(ImVec2(300.f, 300.f));
//        ImGuiWidget::ImButton* button6 = new ImGuiWidget::ImButton("button6");
//        ImGuiWidget::ImButton* button7 = new ImGuiWidget::ImButton("button7");
//
//        ImGuiWidget::ImResizableBox* ResizableBox0 = new ImGuiWidget::ImResizableBox("ResizableBox0");
//        ImGuiWidget::ImImage* Image0 = new ImGuiWidget::ImImage("Image0", "./Resource/preview.jpg");
//        Image0->SetAlpha(0.5);
//        CanvasPanel1->AddChildToCanvasPanel(button6)->SetSlotPosAndSize(ImVec2(100.f, 100.f), ImVec2(20.f, 20.f));
//        CanvasPanel1->AddChildToCanvasPanel(button7)->SetSlotPosAndSize(ImVec2(280.f, 290.f), ImVec2(20.f, 20.f));
//
//
//        m_Box->AddChildToVerticalBox(button0);
//        m_Box->AddChildToVerticalBox(button1);
//        m_Box->AddChildToVerticalBox(button2)->SetIfAutoSize(false);
//        Box1->AddChildToVerticalBox(button3);
//        Box1->AddChildToVerticalBox(button4);
//        button4->SetContent(TextBlock0);
//        Box1->AddChildToVerticalBox(InPutText0);
//        //Box1->AddChildToVerticalBox(TextBlock0);
//        //m_Box->AddChildToVerticalBox(Box1);
//        m_Box->SetSize(ImVec2(100, 600));
//
//        m_Splitter->SetLeftPart(m_Box);
//        m_Splitter->SetRightPart(Box1);
//
//        //m_Canvas->AddChildToCanvasPanel(button5)->SetSlotPosAndSize(ImVec2(500, 500), ImVec2(50, 50));
//        //m_Canvas->AddChildToCanvasPanel(Slider0)->SetSlotPosAndSize(ImVec2(550, 550), ImVec2(200, 10));
//        m_Canvas->AddChildToCanvasPanel(ScrollBox0)->SetSlotPosAndSize(ImVec2(100, 100), ImVec2(400, 400));
//    
//        m_Canvas->AddChildToCanvasPanel(ResizableBox0)->SetSlotPosAndSize(ImVec2(400, 400), ImVec2(400, 400));
//        ResizableBox0->SetContent(Image0);
//        //m_Canvas->AddChildToCanvasPanel(Image0)->SetSlotPosAndSize(ImVec2(0, 0), ImVec2(400, 400));
//        //TestTexture = LoadTextureFromFile("./Resource/preview.jpg");
//    }
//    void Render() override
//    {
//        // 在此处添加你的ImGui控件
//        //ImGui::Begin("Hello World");
//        //ImGui::BeginChild("Test");
//        //m_Box->Render();
//        //m_Splitter->Render();
//        //ImGui::EndChild();
//        //ImGui::End();
//        //ImGui::Image((void*)TestTexture, ImVec2(600, 600));
//        m_Canvas->Render();
//        
//    }
//};
//
//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
//{
//    MyApp app(hInstance, nCmdShow);
//    ImGuiWidget::GlobalApp = &app;
//    if (!app.Initialize())
//        return 1;
//    app.Init();
//    app.Run();
//    return 0;
//}