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
        ImWindow* windowPtr = new ImWindow(title, size, pos, windowId, this);
        m_windows.push_back(windowPtr);

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

    ImWindow* ImWindowManager::CreatePopupWindow(const ImVec2& size, const ImVec2& pos, ImWidget* RootWidget, bool ControlRootWidget)
    {
        ImWindow* window = CreateImWindow("Popup", size, pos);
        window->bCollapsible = false;
        window->bHasTitleBar = false;
        window->bIsMovable = false;
        window->bIsResizable = false;
        window->bAllowBringToFrontOnFocus = true;
        window->bAutoCloseWhenLostFocus = true;
        window->SetRootWidget(RootWidget, ControlRootWidget);
        return window;
    }

    // 创建模态窗口

    ImWindow* ImWindowManager::CreateModalWindow(const std::string& title, const ImVec2& size, const ImVec2& pos, ImWidget* rootWidget, bool controlRootWidget)
    {
        ImWindow* window = CreateImWindow(title, size, pos);
        window->bCollapsible = false;
        window->bIsMovable = true;
        window->bIsResizable = false;
        window->bAllowBringToFrontOnFocus = false;
        window->bAutoCloseWhenLostFocus = false;

        if (rootWidget)
        {
            window->SetRootWidget(rootWidget, controlRootWidget);
        }

        PushModalWindow(window);
        return window;
    }

    void ImWindowManager::CloseWindow(ImWindow* window)
    {
        if (!window) return;
        if (!window->IsOpen()) return;

        window->Close();

        // 如果是模态窗口，从模态栈中移除
        if (window->IsModal())
        {
            PopModalWindow(window);
            return; // PopModalWindow 会调用 CloseWindow，避免重复处理
        }

        // 如果是活动窗口，需要重新设置活动窗口
        if (m_activeWindow == window)
        {
            // 查找下一个可用的窗口作为活动窗口
            ImWindow* newActive = nullptr;
            for (auto& w : m_windows)
            {
                if (w != window && w->IsOpen())
                {
                    newActive = w;
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
                if (w != window && w->IsOpen())
                {
                    newMain = w;
                    break;
                }
            }
            SetMainWindow(newMain);
        }

        // 从列表中移除窗口
        auto it = std::find_if(m_windows.begin(), m_windows.end(),
            [window](ImWindow* w)
            {
                return w == window;
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
            m_activeWindow->SetInactive();
        }

        m_activeWindow = window;

        // 新窗口获得焦点
        if (m_activeWindow)
        {
            m_activeWindow->SetActive();
            BringWindowToFront(m_activeWindow);
        }
    }

    ImWindow* ImWindowManager::FindWindowById(const std::string& id)
    {
        auto it = std::find_if(m_windows.begin(), m_windows.end(),
            [&id](ImWindow* w)
            {
                return w->GetWindowId() == id;
            });

        if (it != m_windows.end())
        {
            return *it;
        }
        return nullptr;
    }

    void ImWindowManager::Render()
    {

        // 渲染非模态窗口
        for (auto& window : m_windows)
        {
            if (window->IsOpen() && !window->IsModal() && window != m_activeWindow)
            {
                window->Render();
            }
        }

        // 渲染活动非模态窗口
        if (m_activeWindow && m_activeWindow->IsOpen() && !m_activeWindow->IsModal())
        {
            m_activeWindow->Render();
        }

        // 如果有模态窗口，渲染模态背景和模态窗口
        if (m_hasActiveModal)
        {
            // 渲染模态背景
            ImGui::GetForegroundDrawList()->AddRectFilled(
                ImVec2(0, 0),
                ImGui::GetIO().DisplaySize,
                IM_COL32(0, 0, 0, (int)(255 * 0.5f)) // 50% 黑色背景
            );

            // 渲染所有模态窗口（按栈顺序）
            for (auto modalWindow : m_modalStack)
            {
                if (modalWindow->IsOpen())
                {
                    modalWindow->Render();
                }
            }
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
    }

    void ImWindowManager::ProcessEvents()
    {
        // 如果有活动模态窗口，只处理模态窗口的事件
        if (m_hasActiveModal)
        {
            ImWindow* topModal = GetTopModalWindow();
            if (topModal && topModal->IsOpen())
            {
                // 只处理顶层模态窗口的事件
                m_EventSystem->SetRootWidget(topModal->GetRootWidget());

                // 收集键盘事件（仅模态窗口）
                m_EventSystem->CollectKeyEvent();

                // 收集鼠标事件（仅当鼠标在模态窗口内）
                ImGuiIO& io = ImGui::GetIO();
                ImVec2 mousePos = io.MousePos;
                if (topModal->ContainsPoint(mousePos))
                {
                    m_EventSystem->CollectMouseEvent();
                }

                m_EventSystem->DispatchEvents();
            }
        }
        else
        {
            // 原有的非模态事件处理逻辑
            ImWindow* LastactiveWindow = m_activeWindow;
            if (LastactiveWindow && LastactiveWindow->IsOpen())
            {
                m_EventSystem->SetRootWidget(LastactiveWindow->GetRootWidget());
                m_EventSystem->CollectKeyEvent();
            }

            bool bHaveWindowJustOpen = false;
            ImGuiIO& io = ImGui::GetIO();
            ImVec2 mousePos = io.MousePos;
            if (ImWindow* MouseHitWindow = WindowHitTest(mousePos))
            {
                m_EventSystem->SetRootWidget(MouseHitWindow->GetRootWidget());
                m_EventSystem->CollectMouseEvent();
            }

            m_EventSystem->DispatchEvents();

            if (!bHaveWindowJustOpen)
            {
                UpdateActiveWindowFromInput();
            }
        }

        // 清除刚打开标志
        for (auto& window : m_windows)
        {
            window->m_JustOpened = false;
        }
    }

    void ImWindowManager::BringWindowToFront(ImWindow* window)
    {
        if (!window) return;

        // 查找窗口在列表中的位置
        auto it = std::find_if(m_windows.begin(), m_windows.end(),
            [window](ImWindow* w)
            {
                return w == window;
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
            [window](ImWindow* w)
            {
                return w == window;
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
                    if (w != window && w->IsOpen())
                    {
                        newActive = w;
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
                [](ImWindow* w)
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
        // 如果有模态窗口，只检查模态窗口
        if (m_hasActiveModal)
        {
            for (auto it = m_modalStack.rbegin(); it != m_modalStack.rend(); ++it)
            {
                auto window = *it;
                if (window->IsOpen() && window->ContainsPoint(Pos))
                {
                    return window;
                }
            }
            return nullptr; // 模态窗口外不命中任何窗口
        }

        // 原有的非模态窗口命中测试逻辑
        for (auto it = m_windows.rbegin(); it != m_windows.rend(); ++it)
        {
            auto& window = *it;
            if (window->IsOpen() && window->ContainsPoint(Pos))
            {
                return window;
            }
        }
        return nullptr;
    }

    void ImWindowManager::PushModalWindow(ImWindow* window)
    {
        if (!window) return;

        // 设置窗口为模态
        window->SetModal(true);

        // 如果已经有活动模态窗口，暂停它的事件
        if (!m_modalStack.empty())
        {
            ImWindow* topModal = m_modalStack.back();
            // 可以在这里保存前一个模态窗口的状态
        }

        // 将新窗口压入模态栈
        m_modalStack.push_back(window);

        // 设置新窗口为活动窗口
        SetActiveWindow(window);
        BringWindowToFront(window);

        UpdateModalState();
    }

    void ImWindowManager::PopModalWindow(ImWindow* window)
    {
        if (m_modalStack.empty()) return;

        // 确保弹出的是栈顶窗口
        if (window != m_modalStack.back())
        {
            // 如果不是栈顶，需要从栈中移除指定的窗口
            auto it = std::find(m_modalStack.begin(), m_modalStack.end(), window);
            if (it != m_modalStack.end())
            {
                m_modalStack.erase(it);
            }
        }
        else
        {
            // 弹出栈顶窗口
            m_modalStack.pop_back();

            // 恢复前一个模态窗口或设置新的活动窗口
            if (!m_modalStack.empty())
            {
                ImWindow* prevModal = m_modalStack.back();
                SetActiveWindow(prevModal);
                BringWindowToFront(prevModal);
            }
            else
            {
                // 没有模态窗口了，设置合适的活动窗口
                ImWindow* newActive = nullptr;
                for (auto& w : m_windows)
                {
                    if (w->IsOpen() && w != window)
                    {
                        newActive = w;
                        break;
                    }
                }
                SetActiveWindow(newActive);
            }
        }

        // 关闭窗口
        CloseWindow(window);
        UpdateModalState();
    }

    ImWindow* ImWindowManager::GetTopModalWindow() const
    {
        if (m_modalStack.empty()) return nullptr;
        return m_modalStack.back();
    }

    void ImWindowManager::UpdateModalState()
    {
        m_hasActiveModal = !m_modalStack.empty();
    }

} // namespace ImGuiWidget