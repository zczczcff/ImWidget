#pragma once
#include "ImPanelWidget.h"
#include "ImEvent/ImMouseEvent.h"
#include "ImEvent/ImDragEvent.h"

namespace ImGuiWidget
{
    class ImScrollBox : public ImPanelWidget
    {
    protected:
        ImVec2 m_ScrollPosition = { 0, 0 };         // 当前滚动位置
        ImVec2 m_ContentSize = { 0, 0 };            // 内容区域大小
        bool m_HorizontalScrollEnabled = true;     // 水平滚动是否启用
        bool m_VerticalScrollEnabled = true;       // 垂直滚动是否启用
        bool m_ShowHorizontalScrollbar = true;     // 是否显示水平滚动条
        bool m_ShowVerticalScrollbar = true;       // 是否显示垂直滚动条
        bool bHaveHorizonScrollbar = false;
        bool bHaveVerticalScrollbar = false;
        float m_ScrollbarThickness = 16.0f;        // 滚动条厚度
        ImU32 m_ScrollbarBgColor = IM_COL32(225, 225, 225, 255);   // 滚动条背景色
        ImU32 m_ScrollbarGrabColor = IM_COL32(100, 100, 100, 255); // 滚动条抓取颜色
        ImU32 m_ScrollbarGrabHoveredColor = IM_COL32(120, 120, 120, 255); // 滚动条悬停颜色
        ImU32 m_ScrollbarGrabActiveColor = IM_COL32(150, 150, 150, 255); // 滚动条激活颜色

        // 滚动条交互状态
        int m_DraggingScrollbar = 0; // 0:无拖动, 1:水平, 2:垂直
        ImVec2 m_DragStartPos;      // 拖动起始鼠标位置
        float m_DragOffset;         // 拖动起始偏移量（滑块内的位置）

        // 鼠标悬停状态
        bool m_IsMouseOver = false;
        int m_HoveredScrollbar = 0; // 0:无悬停, 1:水平, 2:垂直
        ImRect m_HorizontalScrollbarRect;
        ImRect m_VerticalScrollbarRect;
        ImRect m_HorizontalGrabRect;
        ImRect m_VerticalGrabRect;

    public:
        ImScrollBox(const std::string& WidgetName) : ImPanelWidget(WidgetName)
        {
            SetFocusable(true);
        }

        // 设置内容控件
        ImSlot* SetContent(ImWidget* content, bool DeleteOld = true)
        {
            if (!content)
            {
                if (DeleteOld)
                {
                    RemoveChildAt(0);
                }
                else
                {
                    ExtractChildAt(0);
                }
                return nullptr;
            }

            if (GetSlotNum() > 0)
            {
                SetChildAt(0, content, DeleteOld);
            }
            else
            {
                AddChild(content);
            }
            return GetSlotAt(0);
        }

        // 设置滚动位置
        void SetScrollPosition(const ImVec2& position)
        {
            m_ScrollPosition = position;
            // 确保滚动位置在有效范围内
            ClampScrollPosition();
        }

        // 设置滚动方向启用状态
        void EnableHorizontalScroll(bool enable)
        {
            m_HorizontalScrollEnabled = enable;
            if (!enable) m_ScrollPosition.x = 0; // 禁用时重置水平滚动位置
        }
        void EnableVerticalScroll(bool enable)
        {
            m_VerticalScrollEnabled = enable;
            if (!enable) m_ScrollPosition.y = 0; // 禁用时重置垂直滚动位置
        }

        // 设置滚动条可见性
        void ShowHorizontalScrollbar(bool show) { m_ShowHorizontalScrollbar = show; }
        void ShowVerticalScrollbar(bool show) { m_ShowVerticalScrollbar = show; }

        // 设置滚动条样式
        void SetScrollbarThickness(float thickness) { m_ScrollbarThickness = thickness; }
        void SetScrollbarColors(ImU32 bg, ImU32 grab, ImU32 grabHovered, ImU32 grabActive)
        {
            m_ScrollbarBgColor = bg;
            m_ScrollbarGrabColor = grab;
            m_ScrollbarGrabHoveredColor = grabHovered;
            m_ScrollbarGrabActiveColor = grabActive;
        }

        // 获取内容区域大小
        ImVec2 GetContentSize() const { return m_ContentSize; }

        // 获取当前滚动位置
        ImVec2 GetScrollPosition() const { return m_ScrollPosition; }

        virtual ImVec2 GetMinSize() override
        {
            // 滚动框的最小大小包括滚动条厚度
            float minWidth = (m_ShowVerticalScrollbar && m_VerticalScrollEnabled) ? m_ScrollbarThickness : 0.0f;
            float minHeight = (m_ShowHorizontalScrollbar && m_HorizontalScrollEnabled) ? m_ScrollbarThickness : 0.0f;
            return ImVec2(minWidth, minHeight);
        }

        virtual void Render() override
        {
            HandleLayout(); // 确保布局更新

            // 渲染背景
            RenderBackGround();

            if (GetSlotNum() > 0 && GetSlotAt(0) && GetSlotAt(0)->GetContent())
            {
                // 使用布局计算后的滚动条状态
                ImVec2 contentAvail = Size;
                if (bHaveVerticalScrollbar)
                    contentAvail.x -= m_ScrollbarThickness;
                if (bHaveHorizonScrollbar)
                    contentAvail.y -= m_ScrollbarThickness;

                // 创建子窗口用于内容裁剪
                ImGuiWindow* window = ImGui::GetCurrentWindow();
                ImGuiID id = ImGui::GetID(m_WidgetID.c_str());

                ImGui::SetNextWindowPos(Position);
                ImGui::BeginChild(id, contentAvail, ImGuiChildFlags_None,
                    ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

                // 设置滚动偏移
                ImVec2 childPos = ImGui::GetCursorPos();
                if (m_HorizontalScrollEnabled) childPos.x -= m_ScrollPosition.x;
                if (m_VerticalScrollEnabled) childPos.y -= m_ScrollPosition.y;
                ImGui::SetCursorPos(childPos);

                // 渲染所有子控件
                RenderChild();

                // 确保内容区域大小正确
                ImGui::SetCursorPos(ImVec2(0, 0));
                ImGui::Dummy(m_ContentSize);

                ImGui::EndChild();

                // 渲染滚动条
                RenderCustomScrollbars();
            }
        }

    protected:
        // 确保滚动位置在有效范围内
        void ClampScrollPosition()
        {
            // 计算实际需要显示的滚动条
            bool actuallyShowHorizontal = bHaveHorizonScrollbar;
            bool actuallyShowVertical = bHaveVerticalScrollbar;

            float maxScrollX = ImMax(0.0f, m_ContentSize.x - Size.x + (actuallyShowVertical ? m_ScrollbarThickness : 0));
            float maxScrollY = ImMax(0.0f, m_ContentSize.y - Size.y + (actuallyShowHorizontal ? m_ScrollbarThickness : 0));

            if (m_HorizontalScrollEnabled)
            {
                m_ScrollPosition.x = ImClamp(m_ScrollPosition.x, 0.0f, maxScrollX);
            }
            else
            {
                m_ScrollPosition.x = 0.0f;
            }

            if (m_VerticalScrollEnabled)
            {
                m_ScrollPosition.y = ImClamp(m_ScrollPosition.y, 0.0f, maxScrollY);
            }
            else
            {
                m_ScrollPosition.y = 0.0f;
            }
        }

        // 自定义滚动条渲染（只负责渲染，不处理交互）
        void RenderCustomScrollbars()
        {
            bHaveHorizonScrollbar = false;
            bHaveVerticalScrollbar = false;

            ImGuiWindow* window = ImGui::GetCurrentWindow();
            const ImRect scrollArea(Position, Position + Size);

            // 计算实际需要显示的滚动条
            bool shouldShowHorizontal = m_HorizontalScrollEnabled && m_ShowHorizontalScrollbar &&
                m_ContentSize.x > Size.x - (m_ShowVerticalScrollbar ? m_ScrollbarThickness : 0);
            bool shouldShowVertical = m_VerticalScrollEnabled && m_ShowVerticalScrollbar &&
                m_ContentSize.y > Size.y - (m_ShowHorizontalScrollbar ? m_ScrollbarThickness : 0);

            // 渲染水平滚动条
            if (shouldShowHorizontal)
            {
                bHaveHorizonScrollbar = true;

                // 计算水平滚动条的高度和垂直位置
                float scrollbarY = Position.y + Size.y - m_ScrollbarThickness;
                float scrollbarWidth = Size.x - (shouldShowVertical ? m_ScrollbarThickness : 0);

                m_HorizontalScrollbarRect = ImRect(
                    ImVec2(Position.x, scrollbarY),
                    ImVec2(Position.x + scrollbarWidth, scrollbarY + m_ScrollbarThickness)
                );

                // 计算滑块大小和位置
                float maxScroll = m_ContentSize.x - Size.x + (shouldShowVertical ? m_ScrollbarThickness : 0);
                float grabWidth = ImMax(m_ScrollbarThickness, (Size.x / m_ContentSize.x) * m_HorizontalScrollbarRect.GetWidth());
                float grabPosX = m_HorizontalScrollbarRect.Min.x + (m_ScrollPosition.x / maxScroll) * (m_HorizontalScrollbarRect.GetWidth() - grabWidth);

                m_HorizontalGrabRect = ImRect(
                    ImVec2(grabPosX, m_HorizontalScrollbarRect.Min.y),
                    ImVec2(grabPosX + grabWidth, m_HorizontalScrollbarRect.Max.y)
                );

                // 绘制滚动条背景
                window->DrawList->AddRectFilled(m_HorizontalScrollbarRect.Min, m_HorizontalScrollbarRect.Max, m_ScrollbarBgColor);

                // 绘制滑块
                bool isHovered = (m_HoveredScrollbar == 1);
                bool isActive = (m_DraggingScrollbar == 1);
                ImU32 grabColor = isActive ? m_ScrollbarGrabActiveColor :
                    isHovered ? m_ScrollbarGrabHoveredColor :
                    m_ScrollbarGrabColor;
                window->DrawList->AddRectFilled(m_HorizontalGrabRect.Min, m_HorizontalGrabRect.Max, grabColor);
            }

            // 渲染垂直滚动条
            if (shouldShowVertical)
            {
                bHaveVerticalScrollbar = true;

                // 计算垂直滚动条的宽度和水平位置
                float scrollbarX = Position.x + Size.x - m_ScrollbarThickness;
                float scrollbarHeight = Size.y - (shouldShowHorizontal ? m_ScrollbarThickness : 0);

                m_VerticalScrollbarRect = ImRect(
                    ImVec2(scrollbarX, Position.y),
                    ImVec2(scrollbarX + m_ScrollbarThickness, Position.y + scrollbarHeight)
                );

                // 计算滑块大小和位置
                float maxScroll = m_ContentSize.y - Size.y + (shouldShowHorizontal ? m_ScrollbarThickness : 0);
                float grabHeight = ImMax(m_ScrollbarThickness, (Size.y / m_ContentSize.y) * m_VerticalScrollbarRect.GetHeight());
                float grabPosY = m_VerticalScrollbarRect.Min.y + (m_ScrollPosition.y / maxScroll) * (m_VerticalScrollbarRect.GetHeight() - grabHeight);

                m_VerticalGrabRect = ImRect(
                    ImVec2(m_VerticalScrollbarRect.Min.x, grabPosY),
                    ImVec2(m_VerticalScrollbarRect.Max.x, grabPosY + grabHeight)
                );

                // 绘制滚动条背景
                window->DrawList->AddRectFilled(m_VerticalScrollbarRect.Min, m_VerticalScrollbarRect.Max, m_ScrollbarBgColor);

                // 绘制滑块
                bool isHovered = (m_HoveredScrollbar == 2);
                bool isActive = (m_DraggingScrollbar == 2);
                ImU32 grabColor = isActive ? m_ScrollbarGrabActiveColor :
                    isHovered ? m_ScrollbarGrabHoveredColor :
                    m_ScrollbarGrabColor;
                window->DrawList->AddRectFilled(m_VerticalGrabRect.Min, m_VerticalGrabRect.Max, grabColor);
            }
        }

        virtual void Relayout() override
        {
            if (GetSlotNum() > 0 && GetSlotAt(0) && GetSlotAt(0)->GetContent())
            {
                ImSlot* slot = GetSlotAt(0);
                ImWidget* content = slot->GetContent();

                // 先计算是否需要显示滚动条
                bool shouldShowHorizontal = m_HorizontalScrollEnabled && m_ShowHorizontalScrollbar;
                bool shouldShowVertical = m_VerticalScrollEnabled && m_ShowVerticalScrollbar;

                // 临时计算内容区域可用空间（排除可能显示的滚动条）
                ImVec2 tempContentAvail = Size;
                if (shouldShowVertical)
                    tempContentAvail.x -= m_ScrollbarThickness;
                if (shouldShowHorizontal)
                    tempContentAvail.y -= m_ScrollbarThickness;

                // 计算内容尺寸
                ImVec2 minSize = content->GetMinSize();
                m_ContentSize = ImVec2(
                    m_HorizontalScrollEnabled ? minSize.x : ImMin(minSize.x, tempContentAvail.x),
                    m_VerticalScrollEnabled ? minSize.y : ImMin(minSize.y, tempContentAvail.y)
                );

                // 重新计算实际需要显示的滚动条
                bool actuallyShowHorizontal = shouldShowHorizontal && m_ContentSize.x > Size.x - (shouldShowVertical ? m_ScrollbarThickness : 0);
                bool actuallyShowVertical = shouldShowVertical && m_ContentSize.y > Size.y - (shouldShowHorizontal ? m_ScrollbarThickness : 0);

                // 最终的内容区域可用空间
                ImVec2 finalContentAvail = Size;
                if (actuallyShowVertical)
                    finalContentAvail.x -= m_ScrollbarThickness;
                if (actuallyShowHorizontal)
                    finalContentAvail.y -= m_ScrollbarThickness;

                // 更新滚动条状态
                bHaveHorizonScrollbar = actuallyShowHorizontal;
                bHaveVerticalScrollbar = actuallyShowVertical;

                // 设置Slot属性
                slot->SetSlotPosition(Position - m_ScrollPosition);
                slot->SetSlotSize(finalContentAvail);
                slot->ApplyLayout();
            }
        }

        virtual void HandleChildSizeDirty() override
        {
            MarkLayoutDirty(); // 内容尺寸变化需要重新布局
        }

        // 事件处理（新增）
        virtual void HandleEventInternal(ImEvent* event) override
        {
            if (event->IsHandled()) return;

            switch (event->GetType())
            {
            case ImEventType::MouseEnter:
                HandleMouseEnter(event->As<ImMouseEnterEvent>());
                break;

            case ImEventType::MouseLeave:
                HandleMouseLeave(event->As<ImMouseLeaveEvent>());
                break;

            case ImEventType::MouseMove:
                HandleMouseMove(event->As<ImMouseMoveEvent>());
                break;

            case ImEventType::MouseDown:
                HandleMouseDown(event->As<ImMouseDownEvent>());
                break;

            case ImEventType::MouseUp:
                HandleMouseUp(event->As<ImMouseUpEvent>());
                break;

            case ImEventType::MouseWheel:
                HandleMouseWheel(event->As<ImMouseWheelEvent>());
                break;

            case ImEventType::DragStart:
                HandleDragStart(event->As<ImDragStartEvent>());
                break;

            case ImEventType::DragUpdate:
                HandleDragUpdate(event->As<ImDragUpdateEvent>());
                break;

            case ImEventType::DragEnd:
                HandleDragEnd(event->As<ImDragEndEvent>());
                break;

            default:
                // 其他事件继续冒泡
                break;
            }
        }

    private:
        // 鼠标进入事件处理
        void HandleMouseEnter(ImMouseEnterEvent* event)
        {
            m_IsMouseOver = true;
            UpdateHoveredScrollbar(event->GetPosition());
            event->StopPropagation();
        }

        // 鼠标离开事件处理
        void HandleMouseLeave(ImMouseLeaveEvent* event)
        {
            m_IsMouseOver = false;
            m_HoveredScrollbar = 0;
            event->StopPropagation();
        }

        // 鼠标移动事件处理
        void HandleMouseMove(ImMouseMoveEvent* event)
        {
            if (m_IsMouseOver)
            {
                UpdateHoveredScrollbar(event->GetPosition());
            }
            event->StopPropagation();
        }

        // 鼠标按下事件处理
        void HandleMouseDown(ImMouseDownEvent* event)
        {
            if (event->GetButton() == ImMouseButton::Left)
            {
                const ImVec2 mousePos = event->GetPosition();

                // 检查是否在滚动条滑块上按下
                if (m_HoveredScrollbar == 1 && m_HorizontalGrabRect.Contains(mousePos))
                {
                    m_DraggingScrollbar = 1;
                    m_DragStartPos = mousePos;
                    m_DragOffset = mousePos.x - m_HorizontalGrabRect.Min.x;
                    event->StopPropagation();
                }
                else if (m_HoveredScrollbar == 2 && m_VerticalGrabRect.Contains(mousePos))
                {
                    m_DraggingScrollbar = 2;
                    m_DragStartPos = mousePos;
                    m_DragOffset = mousePos.y - m_VerticalGrabRect.Min.y;
                    event->StopPropagation();
                }
            }
        }

        // 鼠标释放事件处理
        void HandleMouseUp(ImMouseUpEvent* event)
        {
            if (event->GetButton() == ImMouseButton::Left && m_DraggingScrollbar != 0)
            {
                m_DraggingScrollbar = 0;
                event->StopPropagation();
            }
        }

        // 鼠标滚轮事件处理
        void HandleMouseWheel(ImMouseWheelEvent* event)
        {
            if (m_IsMouseOver)
            {
                ImVec2 wheelDelta = event->GetWheelDelta();

                if (m_VerticalScrollEnabled && wheelDelta.y != 0.0f)
                {
                    m_ScrollPosition.y -= wheelDelta.y * 30.0f;
                    ClampScrollPosition();
                    event->StopPropagation();
                }
                else if (m_HorizontalScrollEnabled && wheelDelta.x != 0.0f)
                {
                    m_ScrollPosition.x -= wheelDelta.x * 30.0f;
                    ClampScrollPosition();
                    event->StopPropagation();
                }
            }
        }

        // 拖拽开始事件处理
        void HandleDragStart(ImDragStartEvent* event)
        {
            // 如果当前正在拖拽滚动条，阻止事件继续传播
            if (m_DraggingScrollbar != 0)
            {
                event->StopPropagation();
            }
        }

        // 拖拽更新事件处理
        void HandleDragUpdate(ImDragUpdateEvent* event)
        {
            if (m_DraggingScrollbar != 0)
            {
                const ImVec2 mousePos = event->GetPosition();
                ImGuiContext& g = *GImGui;

                if (m_DraggingScrollbar == 1) // 水平滚动条
                {
                    // 计算滑块的目标位置（基于鼠标位置）
                    float targetGrabPos = mousePos.x - m_DragOffset - m_HorizontalScrollbarRect.Min.x;
                    float maxGrabPos = m_HorizontalScrollbarRect.GetWidth() - m_HorizontalGrabRect.GetWidth();

                    // 确保在有效范围内
                    targetGrabPos = ImClamp(targetGrabPos, 0.0f, maxGrabPos);

                    // 计算滚动位置
                    float maxScroll = m_ContentSize.x - Size.x + (m_ShowVerticalScrollbar ? m_ScrollbarThickness : 0);
                    m_ScrollPosition.x = (targetGrabPos / maxGrabPos) * maxScroll;
                }
                else if (m_DraggingScrollbar == 2) // 垂直滚动条
                {
                    // 计算滑块的目标位置（基于鼠标位置）
                    float targetGrabPos = mousePos.y - m_DragOffset - m_VerticalScrollbarRect.Min.y;
                    float maxGrabPos = m_VerticalScrollbarRect.GetHeight() - m_VerticalGrabRect.GetHeight();

                    // 确保在有效范围内
                    targetGrabPos = ImClamp(targetGrabPos, 0.0f, maxGrabPos);

                    // 计算滚动位置
                    float maxScroll = m_ContentSize.y - Size.y + (m_ShowHorizontalScrollbar ? m_ScrollbarThickness : 0);
                    m_ScrollPosition.y = (targetGrabPos / maxGrabPos) * maxScroll;
                }

                ClampScrollPosition();
                event->StopPropagation();
            }
        }

        // 拖拽结束事件处理
        void HandleDragEnd(ImDragEndEvent* event)
        {
            if (m_DraggingScrollbar != 0)
            {
                m_DraggingScrollbar = 0;
                event->StopPropagation();
            }
        }

        // 更新悬停的滚动条状态
        void UpdateHoveredScrollbar(const ImVec2& mousePos)
        {
            int newHoveredScrollbar = 0;

            if (m_HorizontalScrollbarRect.Contains(mousePos) && m_HorizontalGrabRect.Contains(mousePos))
            {
                newHoveredScrollbar = 1;
            }
            else if (m_VerticalScrollbarRect.Contains(mousePos) && m_VerticalGrabRect.Contains(mousePos))
            {
                newHoveredScrollbar = 2;
            }

            if (newHoveredScrollbar != m_HoveredScrollbar)
            {
                m_HoveredScrollbar = newHoveredScrollbar;

                // 更新鼠标光标
                if (m_HoveredScrollbar != 0)
                {
                    ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
                }
            }
        }

    public:
        virtual std::unordered_set<PropertyInfo, PropertyInfo::Hasher> GetProperties() override
        {
            auto props = ImPanelWidget::GetProperties();

            // 滚动条属性
            props.insert({
                "ScrollbarThickness", PropertyType::Float, "Scrollbar",
                [this](void* v) { this->m_ScrollbarThickness = *static_cast<float*>(v); },
                [this]() -> void* { return &this->m_ScrollbarThickness; }
                });

            props.insert({
                "ScrollbarBgColor", PropertyType::Color, "Scrollbar",
                [this](void* v) { this->m_ScrollbarBgColor = *static_cast<ImU32*>(v); },
                [this]() -> void* { return &this->m_ScrollbarBgColor; }
                });

            props.insert({
                "ScrollbarGrabColor", PropertyType::Color, "Scrollbar",
                [this](void* v) { this->m_ScrollbarGrabColor = *static_cast<ImU32*>(v); },
                [this]() -> void* { return &this->m_ScrollbarGrabColor; }
                });

            props.insert({
                "ScrollbarGrabHoveredColor", PropertyType::Color, "Scrollbar",
                [this](void* v) { this->m_ScrollbarGrabHoveredColor = *static_cast<ImU32*>(v); },
                [this]() -> void* { return &this->m_ScrollbarGrabHoveredColor; }
                });

            props.insert({
                "ScrollbarGrabActiveColor", PropertyType::Color, "Scrollbar",
                [this](void* v) { this->m_ScrollbarGrabActiveColor = *static_cast<ImU32*>(v); },
                [this]() -> void* { return &this->m_ScrollbarGrabActiveColor; }
                });

            // 滚动方向属性
            props.insert({
                "HorizontalScrollEnabled", PropertyType::Bool, "Behavior",
                [this](void* v) { this->m_HorizontalScrollEnabled = *static_cast<bool*>(v); },
                [this]() -> void* { return &this->m_HorizontalScrollEnabled; }
                });

            props.insert({
                "VerticalScrollEnabled", PropertyType::Bool, "Behavior",
                [this](void* v) { this->m_VerticalScrollEnabled = *static_cast<bool*>(v); },
                [this]() -> void* { return &this->m_VerticalScrollEnabled; }
                });

            // 滚动条可见性
            props.insert({
                "ShowHorizontalScrollbar", PropertyType::Bool, "Appearance",
                [this](void* v) { this->m_ShowHorizontalScrollbar = *static_cast<bool*>(v); },
                [this]() -> void* { return &this->m_ShowHorizontalScrollbar; }
                });

            props.insert({
                "ShowVerticalScrollbar", PropertyType::Bool, "Appearance",
                [this](void* v) { this->m_ShowVerticalScrollbar = *static_cast<bool*>(v); },
                [this]() -> void* { return &this->m_ShowVerticalScrollbar; }
                });

            return props;
        }

        virtual std::string GetRegisterTypeName()override { return "ImScrollBox"; }

        virtual ImWidget* CopyWidget()
        {
            return new ImScrollBox(*this);
        }
    };
}