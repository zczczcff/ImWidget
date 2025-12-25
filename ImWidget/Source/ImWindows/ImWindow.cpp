#include "ImWindows/ImWindow.h"
#include "ImEvent/ImEventSystem.h"
#include "Application/ImApplication.h"
#include "ImWidget/ImWidget.h"
namespace ImGuiWidget
{

    ImWindow::ImWindow(const std::string& title, const ImVec2& size, const ImVec2& pos, const std::string& ID)
        :
        m_windowId(ID)
        , m_title(title)
        , m_size(size)
        , m_position(pos)
        , bIsOpen(true)
        , bIsActive(false)
        , bIsMovable(true)
        , bIsResizable(true)
        , bHasTitleBar(true)
        , bHasBackground(true)
        , m_bgColor(IM_COL32(240, 240, 240, 255))
        , m_borderColor(IM_COL32(100, 100, 100, 255))
        , m_borderThickness(1.0f)
        , m_rootWidget(nullptr)
    {
        // 创建窗口专用的事件系统
        m_eventSystem = std::make_unique<ImEventSystem>(nullptr);
    }

    ImWindow::~ImWindow()
    {
        if (m_rootWidget)
        {
            delete m_rootWidget;
        }
    }

    void ImWindow::SetRootWidget(ImWidget* rootWidget)
    {
        if (m_rootWidget)
        {
            //delete m_rootWidget;
        }

        m_rootWidget = rootWidget;
        if (m_rootWidget)
        {
            //m_rootWidget->SetParents(this);
            m_eventSystem->SetRootWidget(m_rootWidget);
        }
    }

    void ImWindow::Render()
    {
        if (!bIsOpen) return;

        // 设置窗口标志
        ImGuiWindowFlags flags = 0;
        if (!bHasTitleBar) flags |= ImGuiWindowFlags_NoTitleBar;// 无标题栏
        if (!bIsResizable) flags |= ImGuiWindowFlags_NoResize;// 不可调整大小
        if (!bIsMovable) flags |= ImGuiWindowFlags_NoMove;// 不可移动
        if(!bHaveScrollbar) flags |= ImGuiWindowFlags_NoScrollbar;// 无滚动条
        if (!bCollapsible)flags |= ImGuiWindowFlags_NoCollapse;//不可折叠
        if (!bHasBackground) flags |= ImGuiWindowFlags_NoBackground;//无背景
        if (!bAllowBringToFrontOnFocus)flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

        // 开始窗口
        bool open = bIsOpen;
        if (bFillSysWindow)
        {
            m_position = ImVec2(0, 0);
            m_size = ImGui::GetIO().DisplaySize;
        }
        ImGui::SetNextWindowPos(m_position);
        ImGui::SetNextWindowSize(m_size);
        ImGui::Begin(m_windowId.c_str(), nullptr, flags);

        // 如果窗口被关闭
        if (!open)
        {
            bIsOpen = false;
            ImGui::End();
            return;
        }

        // 检查窗口是否被激活
        if (ImGui::IsWindowFocused())
        {
            bIsActive = true;
        }
        else
        {
            bIsActive = false;
        }

        // 更新窗口位置和大小
        //m_position = ImGui::GetWindowPos();
        //m_size = ImGui::GetWindowSize();

        // 渲染根控件
        if (m_rootWidget)
        {
            if (bFillSysWindow)
            {
                m_rootWidget->SetPosition(ImVec2(1, 1));
                m_rootWidget->SetSize(ImGui::GetWindowSize() - ImVec2(2.f, 2.f));
            }
            m_rootWidget->Render();
        }

        ImGui::End();
    }

    void ImWindow::ProcessEvents()
    {
        if (m_rootWidget && m_eventSystem)
        {
            m_eventSystem->ProcessEvents();
        }
    }

    bool ImWindow::ContainsPoint(const ImVec2& point) const
    {
        ImRect windowRect(m_position, m_position + m_size);
        return windowRect.Contains(point);
    }
} // namespace ImGuiWidget