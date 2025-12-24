#include "ImEvent/ImEventSystem.h"
#include "ImWidget/ImPanelWidget.h"
#include "ImEvent/ImEvent.h"
#include "ImEvent/ImMouseEvent.h"
#include "ImEvent/ImKeyEvent.h"
#include "ImEvent/ImFocusEvent.h"
#include "ImEvent/ImDragEvent.h"
namespace ImGuiWidget
{
    ImGuiWidget::ImEventSystem::ImEventSystem(ImWidget* root) : m_rootWidget(root) {}

    // 主处理入口

    void ImGuiWidget::ImEventSystem::ProcessEvents() {
        CollectEventsFromImGui();
        DispatchEvents();
    }

    // 从ImGui收集事件

    void ImGuiWidget::ImEventSystem::CollectEventsFromImGui() {
        ImGuiIO& io = ImGui::GetIO();

        CollectMouseEvents(io);
        CollectKeyboardEvents(io);
        CollectFocusEvents(io);
        CollectDragEvents(io);
    }

    void ImGuiWidget::ImEventSystem::CollectMouseEvents(ImGuiIO& io) {
        // 鼠标移动事件
        if (!ImVec2Equals(io.MousePos, m_lastMousePos)) {
            auto event = std::make_unique<ImMouseMoveEvent>();
            event->SetPosition(io.MousePos);
            event->SetDelta(ImVec2(io.MousePos.x - m_lastMousePos.x,
                io.MousePos.y - m_lastMousePos.y));
            m_eventQueue.push_back(std::move(event));
            m_lastMousePos = io.MousePos;
        }

        // 鼠标按钮事件
        for (int button = 0; button < 5; ++button) {
            // 鼠标按下
            if (ImGui::IsMouseClicked(button)) {
                HandleMouseButtonEvent(ImEventType::MouseDown, button, io.MousePos);
            }

            // 鼠标释放
            if (ImGui::IsMouseReleased(button)) {
                HandleMouseButtonEvent(ImEventType::MouseUp, button, io.MousePos);
            }
        }

        // 鼠标滚轮
        if (io.MouseWheel != 0 || io.MouseWheelH != 0) {
            auto event = std::make_unique<ImMouseWheelEvent>(
                ImVec2(io.MouseWheelH, io.MouseWheel));
            event->SetPosition(io.MousePos);
            m_eventQueue.push_back(std::move(event));
        }
    }

    void ImGuiWidget::ImEventSystem::HandleMouseButtonEvent(ImEventType type, int button, const ImVec2& pos) {
        ImMouseButton mouseButton = static_cast<ImMouseButton>(button);
        int clickCount = CalculateClickCount(button, pos);

        std::unique_ptr<ImMouseEvent> event;

        if (type == ImEventType::MouseDown) {
            event = std::make_unique<ImMouseDownEvent>(mouseButton, clickCount);
        }
        else {
            event = std::make_unique<ImMouseUpEvent>(mouseButton, clickCount);

            // 如果是鼠标释放，额外发送点击事件
            if (clickCount > 0) {
                auto clickEvent = std::make_unique<ImMouseEvent>(
                    clickCount >= 2 ? ImEventType::MouseDoubleClick : ImEventType::MouseClick,
                    mouseButton, clickCount);
                clickEvent->SetPosition(pos);
                m_eventQueue.push_back(std::move(clickEvent));
            }
        }

        event->SetPosition(pos);
        m_eventQueue.push_back(std::move(event));
    }

    int ImGuiWidget::ImEventSystem::CalculateClickCount(int button, const ImVec2& pos) {
        double currentTime = ImGui::GetTime();
        int buttonIndex = static_cast<int>(button);

        // 检查是否在双击时间和距离范围内
        bool isDoubleClick = (currentTime - m_lastClickTime[buttonIndex] <= DOUBLE_CLICK_TIME) &&
            (ImVec2Distance(pos, m_lastClickPos[buttonIndex]) <= DOUBLE_CLICK_DISTANCE);

        int clickCount = isDoubleClick ? 2 : 1;

        // 更新上次点击信息
        m_lastClickTime[buttonIndex] = currentTime;
        m_lastClickPos[buttonIndex] = pos;

        return clickCount;
    }

    void ImGuiWidget::ImEventSystem::CollectKeyboardEvents(ImGuiIO& io) {
        // 处理按键事件
        for (int key = ImGuiKey_NamedKey_BEGIN; key < ImGuiKey_NamedKey_END; ++key) {
            if (ImGui::IsKeyPressed((ImGuiKey)key, false)) {  // false表示不重复触发
                auto event = std::make_unique<ImKeyDownEvent>(key);
                m_eventQueue.push_back(std::move(event));
            }

            if (ImGui::IsKeyReleased((ImGuiKey)key)) {
                auto event = std::make_unique<ImKeyUpEvent>(key);
                m_eventQueue.push_back(std::move(event));
            }
        }

        // 处理输入字符
        for (int i = 0; i < io.InputQueueCharacters.Size; i++) {
            unsigned int c = io.InputQueueCharacters[i];
            if (c > 0 && c < 0x10000) {  // 有效字符
                auto event = std::make_unique<ImKeyPressEvent>(static_cast<char>(c));
                m_eventQueue.push_back(std::move(event));
            }
        }
    }

    void ImGuiWidget::ImEventSystem::CollectFocusEvents(ImGuiIO& io) {
        // 焦点事件处理（需要与ImGui的焦点系统配合）
        // 这里简化实现，实际需要更复杂的焦点跟踪
    }

    void ImGuiWidget::ImEventSystem::CollectDragEvents(ImGuiIO& io) {
        // 拖拽事件处理
        // 需要与现有的拖拽系统集成
    }

    // 事件分发

    void ImGuiWidget::ImEventSystem::DispatchEvents() {
        for (auto& eventPtr : m_eventQueue) {
            if (!eventPtr) continue;

            // 克隆事件用于分发
            std::unique_ptr<ImEvent> event(eventPtr->Clone());

            // 设置目标控件
            ImWidget* target = FindEventTarget(event.get());
            if (!target) continue;

            event->SetTarget(target);

            // 计算本地坐标
            if (auto mouseEvent = event->As<ImMouseEvent>()) {
                mouseEvent->SetLocalPosition(CalculateLocalPosition(mouseEvent->GetPosition(), target));
            }

            // 事件传播
            DispatchEventThroughHierarchy(event.get(), target);
        }

        m_eventQueue.clear();
    }

    ImWidget* ImGuiWidget::ImEventSystem::FindEventTarget(ImEvent* event) {
        if (auto mouseEvent = event->As<ImMouseEvent>()) {
            return HitTest(m_rootWidget, mouseEvent->GetPosition());
        }
        return m_focusedWidget;  // 键盘事件发送给焦点控件
    }

    ImVec2 ImGuiWidget::ImEventSystem::CalculateLocalPosition(const ImVec2& globalPos, ImWidget* widget) {
        if (!widget) return globalPos;
        return ImVec2(globalPos.x - widget->GetPosition().x,
            globalPos.y - widget->GetPosition().y);
    }

    void ImGuiWidget::ImEventSystem::DispatchEventThroughHierarchy(ImEvent* event, ImWidget* target) {
        // 构建传播路径
        std::vector<ImWidget*> path;
        ImWidget* current = target;
        while (current) {
            path.push_back(current);
            current = current->GetParents();
        }

        // 捕获阶段（从根到目标）
        event->SetPhase(ImEventPhase::Capture);
        for (auto it = path.rbegin(); it != path.rend() && !event->IsHandled(); ++it) {
            event->SetCurrentTarget(*it);
            (*it)->HandleEvent(event);
        }

        // 目标阶段
        if (!event->IsHandled()) {
            event->SetPhase(ImEventPhase::Target);
            event->SetCurrentTarget(target);
            target->HandleEvent(event);
        }

        // 冒泡阶段（从目标到根）
        if (event->Bubbles() && !event->IsHandled()) {
            event->SetPhase(ImEventPhase::Bubble);
            for (auto it = path.begin(); it != path.end() && !event->IsHandled(); ++it) {
                event->SetCurrentTarget(*it);
                (*it)->HandleEvent(event);
            }
        }
    }

    // HitTest实现（与之前相同）

    ImWidget* ImGuiWidget::ImEventSystem::HitTest(ImWidget* widget, const ImVec2& point) {
        if (!widget || !widget->IsVisible()) return nullptr;

        if (auto panel = dynamic_cast<ImPanelWidget*>(widget)) {
            for (int i = panel->GetSlotNum() - 1; i >= 0; --i) {
                if (auto child = panel->GetChildAt(i)) {
                    if (auto result = HitTest(child, point)) {
                        return result;
                    }
                }
            }
        }

        ImRect rect(widget->GetPosition(), widget->GetPosition() + widget->GetSize());
        return rect.Contains(point) ? widget : nullptr;
    }

    // 工具函数

    bool ImGuiWidget::ImEventSystem::ImVec2Equals(const ImVec2& a, const ImVec2& b, float epsilon) {
        return std::abs(a.x - b.x) < epsilon && std::abs(a.y - b.y) < epsilon;
    }

    float ImGuiWidget::ImEventSystem::ImVec2Distance(const ImVec2& a, const ImVec2& b) {
        float dx = a.x - b.x;
        float dy = a.y - b.y;
        return std::sqrt(dx * dx + dy * dy);
    }
}