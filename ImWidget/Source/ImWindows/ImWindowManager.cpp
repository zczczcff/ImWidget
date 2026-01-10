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
        m_EventSystem(new ImEventSystem(nullptr)),
        ToolTipPadding(10.f)
    {
        m_EventSystem->OnEnableToolTip = [this](ImWidget* SetToolTipWidget,ImVec2 Pos) 
        {
            if (!SetToolTipWidget) return;
            ImRect OriginalRect = ImRect(Pos, Pos + SetToolTipWidget->GetMinSize() + ImVec2(ToolTipPadding, ToolTipPadding));
            ImRect ToolTipRect = CalculateAdaptivePopupPosition(OriginalRect);
            ToolTipPos = ToolTipRect.Min;
            ToolTipSize = ToolTipRect.GetSize();
            SetToolTipWidget->SetSize(ToolTipSize);
            SetToolTipWidget->SetPosition(ToolTipPos);
            ToolTipWidget = SetToolTipWidget;
        };
        m_EventSystem->OnDisableToolTip = [this]()
        {
            ToolTipWidget = nullptr;
        };
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

    ImWindow* ImWindowManager::CreatePopupWindow(const ImVec2& size, const ImVec2& pos, ImWidget* RootWidget,
        bool ControlRootWidget, ImWindow* parentPopup)
    {
        ImWindow* window = CreateImWindow("Popup", size, pos);
        window->bCollapsible = false;
        window->bHasTitleBar = false;
        window->bIsMovable = false;
        window->bIsResizable = false;
        window->bAllowBringToFrontOnFocus = true;
        window->bAutoCloseWhenLostFocus = true;
        window->SetPopup(true);
        window->SetPosition(pos);

        if (RootWidget)
        {
            window->SetRootWidget(RootWidget, ControlRootWidget);
        }

        // 设置父子关系
        if (parentPopup)
        {
            parentPopup->AddChildWindow(window);
        }

        return window;
    }

    ImWindow* ImWindowManager::CreateSubMenuPopup(const ImVec2& size, const ImVec2& pos, ImWidget* RootWidget,
        bool ControlRootWidget, ImWindow* parentPopup)
    {
        ImWindow* window = CreatePopupWindow(size, pos, RootWidget, ControlRootWidget, parentPopup);
        window->SetSubMenu(true);
        return window;
    }

    // 创建模态窗口

    void ImWindowManager::CloseWindow(ImWindow* window)
    {
        if (!window) return;
        if (!window->IsOpen()) return;
        window->bIsOpen = false;
        // 递归关闭所有子窗口
        window->CloseAllChildren();

        // 弹出窗口处理
        if (window->IsPopup())
        {
            PopPopupWindow(window);
        }

        // 活动窗口和主窗口处理（保持不变）
        if (m_activeWindow == window)
        {
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

        if (m_mainWindow == window)
        {
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
        //auto it = std::find(m_windows.begin(), m_windows.end(), window);
        //if (it != m_windows.end())
        //{
        //    m_windows.erase(it);
        //}
    }

    void ImWindowManager::OpenWindow(ImWindow* window)
    {
        if (!window->bIsOpen)
        {
            window->bIsOpen = true;
            m_popupStack.push_back(window);
        }
    }

    void ImWindowManager::CloseAllPopups()
    {
        // 关闭所有弹出窗口（从栈顶开始）
        while (!m_popupStack.empty())
        {
            ImWindow* popup = m_popupStack.back();
            CloseWindow(popup);
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
        
        // 如果新窗口是某个弹出窗口的子窗口，确保父窗口保持打开状态
        if (window)
        {
            if (window->IsPopup())
            {
                if (ImWindow* parent = window->GetParentWindow())// 确保所有祖先窗口都保持打开状态
                {
                    std::vector<ImWindow*> Parents;
                    while (parent)
                    {
                        Parents.push_back(parent);
                        //if (!parent->IsOpen())
                        //{
                        //    parent->Open();
                        //}
                        parent = parent->GetParentWindow();
                    }
                    for (int i = Parents.size() - 1; i >= 0; i--)//按父->子顺序打开
                    {
                        Parents[i]->Open();
                    }
                }
                //m_popupStack.push_back(window);
            }
            window->Open();
            BringWindowToFront(window);
        }

        // 旧窗口失去焦点
        if (m_activeWindow)
        {
            m_activeWindow->SetInactive();
        }

        m_activeWindow = window;
    }

    void ImWindowManager::SetWindowInactive(ImWindow* window)
    {
    }

    void ImWindowManager::SetPopupWindowRect(ImWindow* window, const ImVec2& Min, const ImVec2& Max)
    {
        if (!window)return;
        if (!window->IsPopup()) return;
        ImRect OriginalRect = ImRect(Min, Max);
        ImRect AdaptiveRect = OriginalRect;
        if (ImWindow* parentwindow = window->GetParentWindow())
        {
            ImRect ParentRect(parentwindow->GetPosition(), parentwindow->GetPosition() + parentwindow->GetSize());
            AdaptiveRect = CalculateAdaptivePopupPosition(OriginalRect, true, ParentRect);
        }
        else
        {
            AdaptiveRect = CalculateAdaptivePopupPosition(OriginalRect);
        }
        window->SetPosition(AdaptiveRect.Min);
        window->SetSize(AdaptiveRect.GetSize());
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

        // 渲染窗口
        for (auto& window : m_windows)
        {
            if (window->IsOpen() && window != m_activeWindow)
            {
                window->Render();
            }
        }

        // 渲染活动窗口
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

        if (ToolTipWidget)
        {
            ImGuiIO& io = ImGui::GetIO();
            // 设置窗口标志
            ImGuiWindowFlags flags = 0;
            flags |= ImGuiWindowFlags_NoTitleBar;// 无标题栏
            flags |= ImGuiWindowFlags_NoResize;// 不可调整大小
            flags |= ImGuiWindowFlags_NoMove;// 不可移动
            flags |= ImGuiWindowFlags_NoScrollbar;// 无滚动条
            flags |= ImGuiWindowFlags_NoCollapse;//不可折叠
            //flags |= ImGuiWindowFlags_NoBackground;//无背景

            // 开始窗口

            ImGui::SetNextWindowPos(ToolTipPos);
            ImGui::SetNextWindowSize(ToolTipSize);
            ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(255, 255, 255, 255));
            ImGui::Begin("ToolTipWindow", nullptr, flags);
            
            ToolTipWidget->Render();

            ImGui::End();
            ImGui::PopStyleColor();
        }

    }

    void ImWindowManager::ProcessEvents()
    {

            // 非模态事件处理
            ImWindow* LastactiveWindow = m_activeWindow;
            if (LastactiveWindow && LastactiveWindow->IsOpen())
            {
                m_EventSystem->SetRootWidget(LastactiveWindow->GetRootWidget());
                m_EventSystem->CollectKeyEvent();
            }

            bool bHaveWindowJustOpen = false;
            ImGuiIO& io = ImGui::GetIO();
            ImVec2 mousePos = io.MousePos;

            // 检查点击是否在弹出窗口内
            if (ImWindow* MouseHitWindow = WindowHitTest(mousePos))
            {
                m_EventSystem->SetRootWidget(MouseHitWindow->GetRootWidget());
                m_EventSystem->CollectMouseEvent();

                // 如果点击了非活动窗口，且不是当前活动窗口的子窗口，则关闭所有不相关的弹出窗口
                if (io.MouseClicked[0] && MouseHitWindow != m_activeWindow)
                {
                    if (!m_activeWindow || !m_activeWindow->IsAncestorOf(MouseHitWindow))
                    {
                        // 关闭所有弹出窗口
                        CloseAllPopups();
                    }
                }
            }

            m_EventSystem->DispatchEvents();

            if (!bHaveWindowJustOpen)
            {
                UpdateActiveWindowFromInput();
            }

        // 清除刚打开标志
        //for (auto& window : m_windows)
        //{
        //    window->m_JustOpened = false;
        //}
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
            //SetActiveWindow(window);
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
        // 先检查弹出窗口（从栈顶开始）
        for (auto it = m_popupStack.rbegin(); it != m_popupStack.rend(); ++it)
        {
            auto window = *it;
            if (window->IsOpen() && window->ContainsPoint(Pos))
            {
                return window;
            }
        }

        // 然后检查普通窗口
        for (auto it = m_windows.rbegin(); it != m_windows.rend(); ++it)
        {
            auto& window = *it;
            if (window->IsOpen() && !window->IsPopup() && window->ContainsPoint(Pos))
            {
                return window;
            }
        }

        return nullptr;
    }

    void ImWindowManager::PushPopupWindow(ImWindow* popup)
    {
        if (!popup || !popup->IsPopup()) return;

        // 移除已存在的相同弹出窗口（避免重复）
        auto it = std::find(m_popupStack.begin(), m_popupStack.end(), popup);
        if (it != m_popupStack.end())
        {
            m_popupStack.erase(it);
        }

        m_popupStack.push_back(popup);
        UpdatePopupState();
    }

    void ImWindowManager::PopPopupWindow(ImWindow* popup)
    {
        if (m_popupStack.empty()) return;

        auto it = std::find(m_popupStack.begin(), m_popupStack.end(), popup);
        if (it != m_popupStack.end())
        {
            m_popupStack.erase(it);
        }
        UpdatePopupState();
    }

    ImWindow* ImWindowManager::GetTopPopupWindow() const
    {
        if (m_popupStack.empty()) return nullptr;
        return m_popupStack.back();
    }

    bool ImWindowManager::IsPointInAnyPopup(const ImVec2& point) const
    {
        for (auto popup : m_popupStack)
        {
            if (popup->IsOpen() && popup->ContainsPoint(point))
            {
                return true;
            }
        }
        return false;
    }

    void ImWindowManager::UpdatePopupState()
    {
    }
    ImRect ImWindowManager::CalculateAdaptivePopupPosition(const ImRect& OriginalRect, bool HasParent, const ImRect& ParentRect)
    {
        // 获取主视口和可用工作区域
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 workPos = viewport->WorkPos;
        ImVec2 workSize = viewport->WorkSize;
        ImRect availableRect(workPos.x, workPos.y,
            workPos.x + workSize.x,
            workPos.y + workSize.y);

        // 初始化结果矩形为原始矩形
        ImRect resultRect = OriginalRect;
        ImVec2 popupSize(OriginalRect.GetWidth(), OriginalRect.GetHeight());

        // 如果有父窗口，需要考虑避让父窗口
        if (HasParent && ParentRect.Min.x >= 0 && ParentRect.Min.y >= 0)
        {
            // 策略1: 右侧放置（二级菜单常见布局）
            ImRect rightPlacement(
                ParentRect.Max.x,
                OriginalRect.Min.y,
                ParentRect.Max.x + popupSize.x,
                OriginalRect.Min.y + popupSize.y
            );

            // 策略2: 上下调整，判断是否可用
            auto adjustVertical = [&](ImRect& rect)
            {
                if (rect.Min.y < availableRect.Min.y)
                {
                    float offset = availableRect.Min.y - rect.Min.y;
                    rect.Min.y += offset;
                    rect.Max.y += offset;
                }
                if (rect.Max.y > availableRect.Max.y)
                {
                    float offset = rect.Max.y - availableRect.Max.y;
                    rect.Min.y -= offset;
                    rect.Max.y -= offset;
                }
            };

            // 策略3: 左侧放置
            ImRect leftPlacement(
                ParentRect.Min.x - popupSize.x,
                OriginalRect.Min.y,
                ParentRect.Min.x,
                OriginalRect.Min.y + popupSize.y
            );

            // 策略4: 下方放置
            ImRect bottomPlacement(
                OriginalRect.Min.x,
                ParentRect.Max.y,
                OriginalRect.Min.x + popupSize.x,
                ParentRect.Max.y + popupSize.y
            );

            // 策略5: 上方放置
            ImRect topPlacement(
                OriginalRect.Min.x,
                ParentRect.Min.y - popupSize.y,
                OriginalRect.Min.x + popupSize.x,
                ParentRect.Min.y
            );

            // 检查各策略是否在可用区域内且不覆盖父窗口
            auto isValidPlacement = [&](const ImRect& rect) -> bool
            {
                // 检查是否在可用区域内
                bool inBounds = rect.Min.x >= availableRect.Min.x &&
                    rect.Min.y >= availableRect.Min.y &&
                    rect.Max.x <= availableRect.Max.x &&
                    rect.Max.y <= availableRect.Max.y;

                // 检查是否与父窗口重叠
                bool notOverlap = !rect.Overlaps(ParentRect);

                return inBounds && notOverlap;
            };

            // 按优先级尝试各策略
            if (isValidPlacement(rightPlacement))
            {
                resultRect = rightPlacement;
            }
            else
            {
                // 如果右侧不行，则上下调整
                adjustVertical(rightPlacement);
                if (isValidPlacement(rightPlacement))
                {
                    resultRect = rightPlacement;
                }
                else
                {
                    // 否则尝试左侧放置
                    if (isValidPlacement(leftPlacement))
                    {
                        resultRect = leftPlacement;
                    }
                    else
                    {
                        adjustVertical(leftPlacement);
                        if (isValidPlacement(leftPlacement))
                        {
                            resultRect = leftPlacement;
                        }
                        else
                        {
                            // 尝试下方放置
                            if (isValidPlacement(bottomPlacement))
                            {
                                resultRect = bottomPlacement;
                            }
                            else
                            {
                                adjustVertical(bottomPlacement);
                                if (isValidPlacement(bottomPlacement))
                                {
                                    resultRect = bottomPlacement;
                                }
                                else
                                {
                                    // 尝试上方放置
                                    if (isValidPlacement(topPlacement))
                                    {
                                        resultRect = topPlacement;
                                    }
                                    else
                                    {
                                        adjustVertical(topPlacement);
                                        if (isValidPlacement(topPlacement))
                                        {
                                            resultRect = topPlacement;
                                        }
                                        else
                                        {
                                            // 无法避免父窗口重叠，覆盖父窗口
                                            resultRect = ParentRect;
                                            // 调整到可用区域内
                                            if (resultRect.Max.x > availableRect.Max.x)
                                            {
                                                float offset = resultRect.Max.x - availableRect.Max.x;
                                                resultRect.Min.x -= offset;
                                                resultRect.Max.x -= offset;
                                            }
                                            if (resultRect.Min.x < availableRect.Min.x)
                                            {
                                                float offset = availableRect.Min.x - resultRect.Min.x;
                                                resultRect.Min.x += offset;
                                                resultRect.Max.x += offset;
                                            }
                                            if (resultRect.Max.y > availableRect.Max.y)
                                            {
                                                float offset = resultRect.Max.y - availableRect.Max.y;
                                                resultRect.Min.y -= offset;
                                                resultRect.Max.y -= offset;
                                            }
                                            if (resultRect.Min.y < availableRect.Min.y)
                                            {
                                                float offset = availableRect.Min.y - resultRect.Min.y;
                                                resultRect.Min.y += offset;
                                                resultRect.Max.y += offset;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        // 如果没有父窗口，确保弹窗在可用区域内
        else
        {
            // 保证弹窗在可用屏幕范围内
            if (resultRect.Max.x > availableRect.Max.x)
            {
                float offset = resultRect.Max.x - availableRect.Max.x;
                resultRect.Min.x -= offset;
                resultRect.Max.x -= offset;
            }
            if (resultRect.Min.x < availableRect.Min.x)
            {
                float offset = availableRect.Min.x - resultRect.Min.x;
                resultRect.Min.x += offset;
                resultRect.Max.x += offset;
            }
            if (resultRect.Max.y > availableRect.Max.y)
            {
                float offset = resultRect.Max.y - availableRect.Max.y;
                resultRect.Min.y -= offset;
                resultRect.Max.y -= offset;
            }
            if (resultRect.Min.y < availableRect.Min.y)
            {
                float offset = availableRect.Min.y - resultRect.Min.y;
                resultRect.Min.y += offset;
                resultRect.Max.y += offset;
            }
        }

        // 如果仍然超出可用区域，将弹窗放置在原点
        if (resultRect.Min.x < availableRect.Min.x || resultRect.Min.y < availableRect.Min.y ||
            resultRect.Max.x > availableRect.Max.x || resultRect.Max.y > availableRect.Max.y)
        {
            resultRect.Min.x = availableRect.Min.x;
            resultRect.Min.y = availableRect.Min.y;
            resultRect.Max.x = availableRect.Min.x + popupSize.x;
            resultRect.Max.y = availableRect.Min.y + popupSize.y;
        }

        return resultRect;
    }

} // namespace ImGuiWidget