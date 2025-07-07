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
#include "ImWidget/ImExpandableBox.h"
class MyApp : public Application
{
public:
    using Application::Application;

    ImGuiWidget::ImCanvasPanel* m_Canvas;
    void Init()
    {
        m_Canvas = new ImGuiWidget::ImCanvasPanel("Canvas0");
        ImGuiWidget::ImTextBlock* ButtonTestDisplayText = new ImGuiWidget::ImTextBlock("ButtonTestDisplayText");
        ImGuiWidget::ImButton* button0 = new ImGuiWidget::ImButton("button0");
        ImGuiWidget::ImButton* button1 = new ImGuiWidget::ImButton("button1");
        ImGuiWidget::ImButton* button2 = new ImGuiWidget::ImButton("button2");

        ImGuiWidget::ImExpandableBox* ExpandableBox0 = new ImGuiWidget::ImExpandableBox("ExpandableBox0");

        //m_Canvas->AddChildToCanvasPanel(ResizableBox0)->SetSlotPosAndSize(ImVec2(400, 400), ImVec2(400, 400));
        m_Canvas->AddChildToCanvasPanel(ExpandableBox0)->SetSlotPosAndSize(ImVec2(10, 10), ImVec2(100, 20));

        //m_Canvas->AddChildToCanvasPanel(button1)->SetSlotPosAndSize(ImVec2(10, 32), ImVec2(100, 20));
        //m_Canvas->AddChildToCanvasPanel(button2)->SetSlotPosAndSize(ImVec2(10, 54), ImVec2(100, 20));
        //m_Canvas->AddChildToCanvasPanel(ButtonTestDisplayText)->SetSlotPosAndSize(ImVec2(10, 76), ImVec2(100, 20));
    }
    void Render() override
    {
        m_Canvas->Render();
    }
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    MyApp app(hInstance, nCmdShow);
    app.Init();
    if (!app.Initialize())
        return 1;

    app.Run();
    return 0;
}