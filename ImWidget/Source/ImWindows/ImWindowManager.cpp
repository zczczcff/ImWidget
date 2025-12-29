#include "ImWindowManager.h"
#include "ImWindow.h"
#include "Application/ImApplication.h"
#include <algorithm>
#include "ImEvent/ImDragEvent.h"
#include "ImWidget/ImWidget.h"
namespace ImGuiWidget
{

    ImWindowManager::ImWindowManager()
        : m_activeWindow(nullptr)
        , m_mainWindow(nullptr)
        , m_nextWindowId(1),
        m_EventSystem(new ImEventSystem(nullptr))
    {
    }

    ImWindowManager::~ImWindowManager()
    {
        // 清理所有窗口
        m_windows.clear();
    }

    ImWindow* ImWindowManager::CreateImWindow(const std::string& title, const ImVec2& size, const ImVec2& pos)
    {
        // 生成唯一窗口ID
        std::string windowId = "Window_" + std::to_string(m_nextWindowId++);

        // 创建新窗口
        auto newWindow = std::make_unique<ImWindow>(title, size, pos, windowId);

        ImWindow* windowPtr = newWindow.get();
        m_windows.push_back(std::move(newWindow));

        // 如果没有活动窗口，设置新窗口为活动窗口
        if (!m_activeWindow)
        {
            SetActiveWindow(windowPtr);
        }

        // 如果没有主窗口，设置第一个创建的窗口为主窗口
        if (!m_mainWindow)
        {
            SetMainWindow(windowPtr);
        }

        return windowPtr;
    }

    void ImWindowManager::CloseWindow(ImWindow* window)
    {
        if (!window) return;

        // 如果是活动窗口，需要重新设置活动窗口
        if (m_activeWindow == window)
        {
            // 查找下一个可用的窗口作为活动窗口
            ImWindow* newActive = nullptr;
            for (auto& w : m_windows)
            {
                if (w.get() != window && w->IsOpen())
                {
                    newActive = w.get();
                    break;
                }
            }
            SetActiveWindow(newActive);
        }

        // 如果是主窗口，需要重新设置主窗口
        if (m_mainWindow == window)
        {
            // 查找下一个可用的窗口作为主窗口
            ImWindow* newMain = nullptr;
            for (auto& w : m_windows)
            {
                if (w.get() != window && w->IsOpen())
                {
                    newMain = w.get();
                    break;
                }
            }
            SetMainWindow(newMain);
        }

        // 从列表中移除窗口
        auto it = std::find_if(m_windows.begin(), m_windows.end(),
            [window](const std::unique_ptr<ImWindow>& w)
            {
                return w.get() == window;
            });

        if (it != m_windows.end())
        {
            m_windows.erase(it);
        }
    }

    void ImWindowManager::SetMainWindow(ImWindow* window)
    {
        if (m_mainWindow == window) return;

        m_mainWindow = window;

        // 如果设置主窗口，同时设置为活动窗口
        if (window)
        {
            SetActiveWindow(window);
        }
    }

    void ImWindowManager::SetActiveWindow(ImWindow* window)
    {
        if (m_activeWindow == window) return;

        // 旧窗口失去焦点
        if (m_activeWindow)
        {
            m_activeWindow->SetIsActive(false);
        }

        m_activeWindow = window;

        // 新窗口获得焦点
        if (m_activeWindow)
        {
            m_activeWindow->SetIsActive(true);
            BringWindowToFront(m_activeWindow);
        }
    }

    ImWindow* ImWindowManager::FindWindowById(const std::string& id)
    {
        auto it = std::find_if(m_windows.begin(), m_windows.end(),
            [&id](const std::unique_ptr<ImWindow>& w)
            {
                return w->GetWindowId() == id;
            });

        if (it != m_windows.end())
        {
            return it->get();
        }
        return nullptr;
    }

    void ImWindowManager::Render()
    {

        // 先渲染非活动窗口（在背景）
        for (auto& window : m_windows)
        {
            if (window->IsOpen() && window.get() != m_activeWindow)
            {
                window->Render();
            }
        }

        // 最后渲染活动窗口（在前景）
        if (m_activeWindow && m_activeWindow->IsOpen())
        {
            m_activeWindow->Render();
        }

        if (auto PreviewWidget = ImDragEvent::GetDragPreview())//渲染拖拽控件
        {
            ImGuiIO& io = ImGui::GetIO();
            // 设置窗口标志
            ImGuiWindowFlags flags = 0;
            flags |= ImGuiWindowFlags_NoTitleBar;// 无标题栏
            flags |= ImGuiWindowFlags_NoResize;// 不可调整大小
            flags |= ImGuiWindowFlags_NoMove;// 不可移动
            flags |= ImGuiWindowFlags_NoScrollbar;// 无滚动条
            flags |= ImGuiWindowFlags_NoCollapse;//不可折叠
            flags |= ImGuiWindowFlags_NoBackground;//无背景

            // 开始窗口
            ImVec2  m_position = ImVec2(0, 0);
            ImVec2   m_size = io.DisplaySize;
            ImGui::SetNextWindowPos(m_position);
            ImGui::SetNextWindowSize(m_size);
            ImGui::Begin("DragPreview", nullptr, flags);
            ImVec2 mousePos = io.MousePos;

            ImVec2 previewwidgetsize = PreviewWidget->GetSize();

            PreviewWidget->SetPosition(mousePos - (previewwidgetsize / 2));
            PreviewWidget->Render();

            ImGui::End();
        }

        // 清理已关闭的窗口
        //CleanupClosedWindows();
    }

    void ImWindowManager::ProcessEvents()
    {
        ImWindow* LastactiveWindow = m_activeWindow;//在事件处理中可能发生activeWindow的改变，必须先存一份副本
        // 先处理活跃窗口的键盘事件
        if (LastactiveWindow && LastactiveWindow->IsOpen())
        {
            m_EventSystem->SetRootWidget(LastactiveWindow->GetRootWidget());
            m_EventSystem->CollectKeyEvent();
        }

        bool bHaveWindowJustOpen = false;
        // 处理其他窗口的事件
        //for (auto& window : m_windows)
        //{
        //    if (window->IsOpen() && window.get() != LastactiveWindow)
        //    {
        //        window->ProcessEvents();
        //    }
        //    if (window->m_JustOpened)
        //    {
        //        bHaveWindowJustOpen = true;
        //    }
        //}
                // 获取ImGui IO状态
        ImGuiIO& io = ImGui::GetIO();
        ImVec2 mousePos = io.MousePos;
        if (ImWindow* MouseHitWindow = WindowHitTest(mousePos))
        {
            m_EventSystem->SetRootWidget(MouseHitWindow->GetRootWidget());
            m_EventSystem->CollectMouseEvent();
        }

        m_EventSystem->DispatchEvents();

        // 检查窗口点击，更新活动窗口
        if (!bHaveWindowJustOpen)
        {
            UpdateActiveWindowFromInput();
        }
        

        for (auto& window : m_windows)//清除刚打开标志
        {
            window->m_JustOpened = false;
        }
        // 清理已关闭的窗口
        //CleanupClosedWindows();
    }

    void ImWindowManager::BringWindowToFront(ImWindow* window)
    {
        if (!window) return;

        // 查找窗口在列表中的位置
        auto it = std::find_if(m_windows.begin(), m_windows.end(),
            [window](const std::unique_ptr<ImWindow>& w)
            {
                return w.get() == window;
            });

        if (it != m_windows.end() && it != m_windows.end() - 1)
        {
            // 将窗口移动到列表末尾（最后渲染的窗口在最前面）
            auto windowPtr = std::move(*it);
            m_windows.erase(it);
            m_windows.push_back(std::move(windowPtr));

            // 设置为活动窗口
            SetActiveWindow(window);
        }
    }

    void ImWindowManager::SendWindowToBack(ImWindow* window)
    {
        if (!window) return;

        // 查找窗口在列表中的位置
        auto it = std::find_if(m_windows.begin(), m_windows.end(),
            [window](const std::unique_ptr<ImWindow>& w)
            {
                return w.get() == window;
            });

        if (it != m_windows.end() && it != m_windows.begin())
        {
            // 将窗口移动到列表开头（最先渲染的窗口在最后面）
            auto windowPtr = std::move(*it);
            m_windows.erase(it);
            m_windows.insert(m_windows.begin(), std::move(windowPtr));

            // 如果移动的是活动窗口，需要设置新的活动窗口
            if (m_activeWindow == window)
            {
                // 查找下一个可用的窗口作为活动窗口
                ImWindow* newActive = nullptr;
                for (auto& w : m_windows)
                {
                    if (w.get() != window && w->IsOpen())
                    {
                        newActive = w.get();
                        break;
                    }
                }
                SetActiveWindow(newActive);
            }
        }
    }

    void ImWindowManager::CleanupClosedWindows()
    {
        // 移除所有已关闭的窗口
        m_windows.erase(
            std::remove_if(m_windows.begin(), m_windows.end(),
                [](const std::unique_ptr<ImWindow>& w)
                {
                    return !w->IsOpen();
                }),
            m_windows.end());
    }

    void ImWindowManager::UpdateActiveWindowFromInput()
    {
        // 获取ImGui IO状态
        ImGuiIO& io = ImGui::GetIO();

        // 检查鼠标点击，更新活动窗口
        if (io.MouseClicked[0]) // 左键点击
        {
            ImVec2 mousePos = io.MousePos;

            if (ImWindow* HitWindow = WindowHitTest(mousePos))
            {
                SetActiveWindow(HitWindow);
            }

            // 从最前面的窗口开始检查（反向遍历）
            //for (auto it = m_windows.rbegin(); it != m_windows.rend(); ++it)
            //{
            //    auto& window = *it;
            //    if (window->IsOpen() && window->ContainsPoint(mousePos))
            //    {
            //        // 找到被点击的窗口，设置为活动窗口
            //        if (window.get() != m_activeWindow)
            //        {
            //            SetActiveWindow(window.get());
            //        }
            //        break; // 只激活最前面的窗口
            //    }
            //}
        }
    }

    ImWindow* ImWindowManager::WindowHitTest(const ImVec2& Pos)
    {
        // 从最前面的窗口开始检查（反向遍历）
        for (auto it = m_windows.rbegin(); it != m_windows.rend(); ++it)
        {
            auto& window = *it;
            if (window->IsOpen() && window->ContainsPoint(Pos))
            {
                return window.get();
            }
        }
        return nullptr;
    }

} // namespace ImGuiWidget