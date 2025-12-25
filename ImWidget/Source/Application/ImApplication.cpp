#include "..\..\Include\Application\ImApplication.h"
#include "Application/ImApplication.h"

extern void ImTick();

ImApplication::ImApplication() :m_windowManager(new ImGuiWidget::ImWindowManager())
{
}

void ImApplication::RenderTick()
{
    ImGui::PushFont(GetFont(DefalutFontSize, DefaultFont));
    // 处理窗口事件
    m_windowManager->ProcessEvents();
    // 渲染所有窗口
    m_windowManager->Render();
    ImTick();
    ImGui::PopFont();
}

void ImApplication::SetRootWidget(ImGuiWidget::ImWidget* RootWidget)
{
    // 创建窗口管理器（如果不存在）
    if (!m_windowManager)
    {
        m_windowManager = new ImGuiWidget::ImWindowManager();
    }

    // 创建默认窗口
    auto window = CreateDefaultWindow();
    window->SetRootWidget(RootWidget);
    window->bFillSysWindow = true;
    window->bHasTitleBar = false;
    window->bHasBackground = false;
    window->bIsMovable = false;
    window->bIsResizable = false;
    window->bCollapsible = false;
    //if (auto panel = dynamic_cast<ImGuiWidget::ImPanelWidget*>(RootWidget))
    //{
    //    window->SetRootWidget(panel);
    //}
    //else
    //{
    //    // 如果不是Panel，创建一个Panel作为根容器
    //    auto rootPanel = new ImGuiWidget::ImPanelWidget("RootPanel");
    //    rootPanel->AddChild(RootWidget);
    //    window->SetRootWidget(rootPanel);
    //}

    // 添加到窗口管理器
    m_windowManager->SetMainWindow(window);
}

//void ImApplication::AddWindow(ImGuiWidget::ImWindow* window)
//{
//    if (!m_windowManager)
//    {
//        m_windowManager = new ImGuiWidget::ImWindowManager();
//    }
//
//    // 注意：这里窗口已经由窗口管理器管理，我们只需要确保管理器存在
//}

ImGuiWidget::ImWindow* ImApplication::CreateDefaultWindow(const std::string& title)
{
    if (!m_windowManager)
    {
        m_windowManager = new ImGuiWidget::ImWindowManager();
    }

    // 创建默认大小的窗口
    return m_windowManager->CreateImWindow(title, ImVec2(800, 600), ImVec2(100, 100));
}



