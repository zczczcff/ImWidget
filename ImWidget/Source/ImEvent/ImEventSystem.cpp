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
        CollectDragEvents(io);
        CollectHoverEvents(io); 
    }

    void ImEventSystem::CollectMouseEvents(ImGuiIO& io) {
        // 获取当前修饰键状态
        ImModifierKeys mods = ImModifierKeys::GetCurrent();

        // 鼠标移动事件
        if (!ImVec2Equals(io.MousePos, m_lastMousePos)) {
            auto event = std::make_unique<ImMouseMoveEvent>();
            event->SetPosition(io.MousePos);
            event->SetDelta(ImVec2(io.MousePos.x - m_lastMousePos.x,
                io.MousePos.y - m_lastMousePos.y));
            event->SetModifiers(mods);
            m_eventQueue.push_back(std::move(event));
            m_lastMousePos = io.MousePos;
        }

        // 鼠标按钮事件
        for (int button = 0; button < 5; ++button) {
            // 鼠标按下
            if (ImGui::IsMouseClicked(button)) {
                HandleMouseButtonEvent(ImEventType::MouseDown, button, io.MousePos, mods);
            }

            // 鼠标释放
            if (ImGui::IsMouseReleased(button)) {
                HandleMouseButtonEvent(ImEventType::MouseUp, button, io.MousePos, mods);
            }
        }

        // 鼠标滚轮
        if (io.MouseWheel != 0 || io.MouseWheelH != 0) {
            auto event = std::make_unique<ImMouseWheelEvent>(
                ImVec2(io.MouseWheelH, io.MouseWheel));
            event->SetPosition(io.MousePos);
            event->SetModifiers(mods);
            m_eventQueue.push_back(std::move(event));
        }

        // 新增：滚轮按下检测
        //if (ImGui::IsMouseClicked(2)) { // 中键按下
        //    auto event = std::make_unique<ImMouseWheelClickEvent>();
        //    event->SetPosition(io.MousePos);
        //    event->SetModifiers(mods);
        //    m_eventQueue.push_back(std::move(event));
        //}

        // 新增：滚轮倾斜检测（如果ImGui支持）
        // 注意：ImGui目前可能不支持滚轮倾斜检测，这里预留接口
        if (io.MouseWheel != 0 || io.MouseWheelH != 0) {
            // 可以在这里添加倾斜检测逻辑
        }
    }

	void ImEventSystem::HandleMouseButtonEvent(ImEventType type, int button, const ImVec2& pos, const ImModifierKeys& mods) {
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
                std::unique_ptr<ImMouseEvent> clickEvent;
                if (clickCount >= 2) {
                    clickEvent = std::make_unique<ImMouseDoubleClickEvent>(mouseButton);
                }
                else {
                    clickEvent = std::make_unique<ImMouseClickEvent>(mouseButton, clickCount);
                }
                clickEvent->SetPosition(pos);
                clickEvent->SetModifiers(mods);
                m_eventQueue.push_back(std::move(clickEvent));
            }
        }

        event->SetPosition(pos);
        event->SetModifiers(mods);
        m_eventQueue.push_back(std::move(event));
    }

    void ImEventSystem::CollectHoverEvents(ImGuiIO& io) {
        // 检测当前鼠标下的控件
        ImWidget* currentHovered = HitTest(m_rootWidget, io.MousePos);

        // 处理鼠标进入/离开事件
        if (currentHovered != m_lastHoveredWidget) {
            // 发送离开事件给之前悬停的控件
            if (m_lastHoveredWidget != nullptr) {
                auto leaveEvent = std::make_unique<ImMouseLeaveEvent>();
                leaveEvent->SetPosition(io.MousePos);
                leaveEvent->SetTarget(m_lastHoveredWidget);
                m_eventQueue.push_back(std::move(leaveEvent));
            }

            // 发送进入事件给新悬停的控件
            if (currentHovered != nullptr) {
                auto enterEvent = std::make_unique<ImMouseEnterEvent>();
                enterEvent->SetPosition(io.MousePos);
                enterEvent->SetTarget(currentHovered);
                m_eventQueue.push_back(std::move(enterEvent));

                // 开始悬停计时
                m_hoverStartTime = ImGui::GetTime();
                m_hoveredWidget = currentHovered;
            }
            else {
                m_hoverStartTime = 0.0;
                m_hoveredWidget = nullptr;
            }

            m_lastHoveredWidget = currentHovered;
        }

        // 更新当前悬停控件
        m_hoveredWidget = currentHovered;
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

    void ImGuiWidget::ImEventSystem::CollectDragEvents(ImGuiIO& io) {
        ImModifierKeys mods = ImModifierKeys::GetCurrent();
        // 检查是否开始拖拽
        if (!m_isDragging && ImGui::IsMouseDragging(0)) { // 左键拖拽
            // 检查拖拽阈值（避免误触）
            ImVec2 dragDelta = ImGui::GetMouseDragDelta(0);
            if ( ImVec2Distance(ImVec2(0,0), dragDelta) > 2.0f) { // 2像素阈值
                StartDrag(io.MousePos- dragDelta, mods);
            }
        }

        // 处理拖拽更新
        if (m_isDragging) {
            if (ImGui::IsMouseDragging(0)) {
                UpdateDrag(io.MousePos, mods);
            }
            else {
                // 鼠标释放，结束拖拽
                EndDrag(io.MousePos, mods);
            }
        }

        // 处理拖放目标
        if (m_isDragging) {
            ProcessDropTargets(io.MousePos, mods);
        }
    }

    // 事件分发

    void ImGuiWidget::ImEventSystem::DispatchEvents() {
        for (auto& eventPtr : m_eventQueue) {
            if (!eventPtr) continue;

            // 克隆事件用于分发
            std::unique_ptr<ImEvent> event(eventPtr->Clone());

            // 设置目标控件
            if (ImWidget* target = event->GetTarget())
            {
                // 计算本地坐标
                if (auto mouseEvent = event->As<ImMouseEvent>()) {
                    mouseEvent->SetLocalPosition(CalculateLocalPosition(mouseEvent->GetPosition(), target));
                }

                // 事件传播
                DispatchEventThroughHierarchy(event.get(), target);
            }
            else
            {
                target = FindEventTarget(event.get());
                if (!target) continue;

                event->SetTarget(target);

                // 计算本地坐标
                if (auto mouseEvent = event->As<ImMouseEvent>()) {
                    mouseEvent->SetLocalPosition(CalculateLocalPosition(mouseEvent->GetPosition(), target));
                }

                // 事件传播
                DispatchEventThroughHierarchy(event.get(), target);
            }

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

    void ImEventSystem::StartDrag(const ImVec2& pos, const ImModifierKeys& mods) {
        m_isDragging = true;
        m_dragStartPos = pos;
        m_dragSourceWidget = m_lastHoveredWidget;

        // 创建拖拽开始事件
        auto event = std::make_unique<ImDragStartEvent>();
        event->SetTarget(m_dragSourceWidget);
        event->SetPosition(pos);
        event->SetModifiers(mods);

        m_eventQueue.push_back(std::move(event));
    }

    void ImEventSystem::UpdateDrag(const ImVec2& pos, const ImModifierKeys& mods) {
        auto event = std::make_unique<ImDragUpdateEvent>();
        event->SetPosition(pos);
        event->SetTarget(m_dragSourceWidget);
        event->SetDelta(ImVec2(pos.x - m_lastDragPos.x, pos.y - m_lastDragPos.y));
        event->SetModifiers(mods);

        m_eventQueue.push_back(std::move(event));
        m_lastDragPos = pos;
    }

    void ImEventSystem::EndDrag(const ImVec2& pos, const ImModifierKeys& mods) {
        auto event = std::make_unique<ImDragEndEvent>();
        event->SetPosition(pos);
        event->SetTarget(m_dragSourceWidget);
        event->SetModifiers(mods);

        // 处理拖放
        ImWidget* dropTarget = HitTest(m_rootWidget, pos);
        if (dropTarget && dropTarget != m_dragSourceWidget) {
            ProcessDrop(dropTarget, pos, mods);
        }

        m_eventQueue.push_back(std::move(event));

        // 重置拖拽状态
        m_isDragging = false;
        m_dragSourceWidget = nullptr;
        m_lastDragPos = ImVec2(0, 0);
    }

    void ImEventSystem::ProcessDropTargets(const ImVec2& pos, const ImModifierKeys& mods) {
        ImWidget* currentTarget = HitTest(m_rootWidget, pos);

        // 处理拖拽进入/离开事件
        if (currentTarget != m_lastDragHoveredWidget) {
            // 发送拖拽离开事件给之前的控件
            if (m_lastDragHoveredWidget) {
                auto leaveEvent = std::make_unique<ImDragEvent>(ImEventType::DragLeave);
                leaveEvent->SetPosition(pos);
                leaveEvent->SetModifiers(mods);
                leaveEvent->SetTarget(m_lastDragHoveredWidget);
                m_eventQueue.push_back(std::move(leaveEvent));
            }

            // 发送拖拽进入事件给新控件
            if (currentTarget) {
                auto enterEvent = std::make_unique<ImDragEvent>(ImEventType::DragEnter);
                enterEvent->SetPosition(pos);
                enterEvent->SetModifiers(mods);
                enterEvent->SetTarget(currentTarget);
                m_eventQueue.push_back(std::move(enterEvent));
            }

            m_lastDragHoveredWidget = currentTarget;
        }

        // 发送拖拽经过事件给当前控件
        if (currentTarget) {
            auto overEvent = std::make_unique<ImDragEvent>(ImEventType::DragOver);
            overEvent->SetPosition(pos);
            overEvent->SetModifiers(mods);
            overEvent->SetTarget(currentTarget);
            m_eventQueue.push_back(std::move(overEvent));
        }
    }

    void ImEventSystem::ProcessDrop(ImWidget* target, const ImVec2& pos, const ImModifierKeys& mods) {
        if (!m_dragSourceWidget) return;

        auto dropEvent = std::make_unique<ImDragEvent>(ImEventType::Drop);
        dropEvent->SetPosition(pos);
        dropEvent->SetModifiers(mods);
        dropEvent->SetTarget(target);

        m_eventQueue.push_back(std::move(dropEvent));
    }
}