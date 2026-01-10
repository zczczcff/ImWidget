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

    ImRect ImWindowManager::CalculateAdaptivePopupPosition(const ImRect& OriginalRect, bool HasParent, const ImRect& ParentsRect)
    {
        // 获取主视口和屏幕工作区
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 work_pos = viewport->WorkPos;      // 工作区位置（减去任务栏等）
        ImVec2 work_size = viewport->WorkSize;    // 工作区大小

        ImRect work_rect(work_pos, work_pos + work_size);
        ImRect result_rect = OriginalRect;

        float popup_width = OriginalRect.GetWidth();
        float popup_height = OriginalRect.GetHeight();

        // 如果没有父窗口，只考虑屏幕边界
        if (!HasParent || (ParentsRect.Min.x < 0 && ParentsRect.Min.y < 0))
        {
            // 确保弹窗不超出屏幕右边界
            if (result_rect.Max.x > work_rect.Max.x)
            {
                result_rect.TranslateX(work_rect.Max.x - result_rect.Max.x);
            }

            // 确保弹窗不超出屏幕左边界
            if (result_rect.Min.x < work_rect.Min.x)
            {
                result_rect.TranslateX(work_rect.Min.x - result_rect.Min.x);
            }

            // 确保弹窗不超出屏幕下边界
            if (result_rect.Max.y > work_rect.Max.y)
            {
                result_rect.TranslateY(work_rect.Max.y - result_rect.Max.y);
            }

            // 确保弹窗不超出屏幕上边界
            if (result_rect.Min.y < work_rect.Min.y)
            {
                result_rect.TranslateY(work_rect.Min.y - result_rect.Min.y);
            }

            return result_rect;
        }

        // 有父窗口的情况：需要避免覆盖父窗口
        // 1. 优先考虑父窗口的四个方向
        ImRect candidate_rects[4];
        float scores[4] = { 0 };

        // 右侧
        candidate_rects[0] = ImRect(
            ParentsRect.Max.x,
            ParentsRect.Min.y,
            ParentsRect.Max.x + popup_width,
            ParentsRect.Min.y + popup_height
        );

        // 左侧
        candidate_rects[1] = ImRect(
            ParentsRect.Min.x - popup_width,
            ParentsRect.Min.y,
            ParentsRect.Min.x,
            ParentsRect.Min.y + popup_height
        );

        // 下方
        candidate_rects[2] = ImRect(
            ParentsRect.Min.x,
            ParentsRect.Max.y,
            ParentsRect.Min.x + popup_width,
            ParentsRect.Max.y + popup_height
        );

        // 上方
        candidate_rects[3] = ImRect(
            ParentsRect.Min.x,
            ParentsRect.Min.y - popup_height,
            ParentsRect.Min.x + popup_width,
            ParentsRect.Min.y
        );

        // 计算每个候选位置的得分
        for (int i = 0; i < 4; ++i)
        {
            // 基础分：完全在屏幕内
            if (work_rect.Contains(candidate_rects[i]))
            {
                scores[i] += 100.0f;
            }

            // 不与父窗口重叠
            if (!candidate_rects[i].Overlaps(ParentsRect))
            {
                scores[i] += 50.0f;
            }

            // 距离原始位置的距离（越近得分越高）
            float distance = ImLengthSqr(
                candidate_rects[i].GetCenter() - OriginalRect.GetCenter()
            );
            scores[i] += 100.0f / (1.0f + distance * 0.001f);

            // 可见区域比例
            ImRect visible_rect = work_rect;
            visible_rect.ClipWithFull(candidate_rects[i]);
            float visible_area = visible_rect.GetArea();
            float total_area = candidate_rects[i].GetArea();
            if (total_area > 0)
            {
                scores[i] += 50.0f * (visible_area / total_area);
            }
        }

        // 找到得分最高的位置
        int best_index = 0;
        for (int i = 1; i < 4; ++i)
        {
            if (scores[i] > scores[best_index])
            {
                best_index = i;
            }
        }

        result_rect = candidate_rects[best_index];

        // 2. 如果最佳候选位置不理想，尝试在屏幕内寻找更好的位置
        if (scores[best_index] < 50.0f || result_rect.Overlaps(ParentsRect))
        {
            // 尝试在屏幕内寻找位置
            ImVec2 best_pos = OriginalRect.Min;
            float best_score = -FLT_MAX;

            // 尝试多个位置
            for (int y_dir = -1; y_dir <= 1; ++y_dir)
            {
                for (int x_dir = -1; x_dir <= 1; ++x_dir)
                {
                    if (x_dir == 0 && y_dir == 0) continue;

                    ImVec2 test_pos = OriginalRect.Min;

                    if (x_dir < 0)
                    {
                        test_pos.x = std::max(work_rect.Min.x, ParentsRect.Min.x - popup_width - 5.0f);
                    }
                    else if (x_dir > 0)
                    {
                        test_pos.x = std::min(work_rect.Max.x - popup_width, ParentsRect.Max.x + 5.0f);
                    }

                    if (y_dir < 0)
                    {
                        test_pos.y = std::max(work_rect.Min.y, ParentsRect.Min.y - popup_height - 5.0f);
                    }
                    else if (y_dir > 0)
                    {
                        test_pos.y = std::min(work_rect.Max.y - popup_height, ParentsRect.Max.y + 5.0f);
                    }

                    ImRect test_rect(test_pos, test_pos + ImVec2(popup_width, popup_height));

                    // 计算分数
                    float score = 0.0f;
                    if (!test_rect.Overlaps(ParentsRect))
                    {
                        score += 100.0f;
                    }

                    float visible_area = 1.0f;
                    if (work_rect.Contains(test_rect))
                    {
                        score += 100.0f;
                    }
                    else
                    {
                        ImRect visible_part = work_rect;
                        visible_part.ClipWithFull(test_rect);
                        visible_area = visible_part.GetArea() / test_rect.GetArea();
                        score += 100.0f * visible_area;
                    }

                    float distance = ImLengthSqr(test_rect.GetCenter() - OriginalRect.GetCenter());
                    score += 50.0f / (1.0f + distance * 0.001f);

                    if (score > best_score)
                    {
                        best_score = score;
                        best_pos = test_pos;
                    }
                }
            }

            result_rect = ImRect(best_pos, best_pos + ImVec2(popup_width, popup_height));
        }

        // 3. 最终边界检查
        // 水平方向
        if (result_rect.Min.x < work_rect.Min.x)
        {
            result_rect.TranslateX(work_rect.Min.x - result_rect.Min.x);
        }
        else if (result_rect.Max.x > work_rect.Max.x)
        {
            result_rect.TranslateX(work_rect.Max.x - result_rect.Max.x);
        }

        // 垂直方向
        if (result_rect.Min.y < work_rect.Min.y)
        {
            result_rect.TranslateY(work_rect.Min.y - result_rect.Min.y);
        }
        else if (result_rect.Max.y > work_rect.Max.y)
        {
            result_rect.TranslateY(work_rect.Max.y - result_rect.Max.y);
        }

        return result_rect;
    }

} // namespace ImGuiWidget