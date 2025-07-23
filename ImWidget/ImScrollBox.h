#pragma once
#include "ImPanelWidget.h"

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
        float m_ScrollbarThickness = 16.0f;        // 滚动条厚度
        ImU32 m_ScrollbarBgColor = IM_COL32(40, 40, 40, 255);   // 滚动条背景色
        ImU32 m_ScrollbarGrabColor = IM_COL32(100, 100, 100, 255); // 滚动条抓取颜色
        ImU32 m_ScrollbarGrabHoveredColor = IM_COL32(120, 120, 120, 255); // 滚动条悬停颜色
        ImU32 m_ScrollbarGrabActiveColor = IM_COL32(150, 150, 150, 255); // 滚动条激活颜色

        // 滚动条交互状态
        int m_DraggingScrollbar = 0; // 0:无拖动, 1:水平, 2:垂直
        ImVec2 m_DragStartPos;      // 拖动起始鼠标位置
        float m_DragOffset;         // 拖动起始偏移量（滑块内的位置）

    public:
        ImScrollBox(const std::string& WidgetName) : ImPanelWidget(WidgetName) {}

        // 设置内容控件
        ImSlot* SetContent(ImWidget* content)
        {
            if (m_Slots.size() >= 1 && m_Slots[0])
            {
                delete m_Slots[0];
                m_Slots[0] = new ImSlot(content);
            }
            else
            {
                m_Slots.push_back(new ImSlot(content));
            }

            return m_Slots[0];
        }

        // 设置滚动位置
        void SetScrollPosition(const ImVec2& position)
        {
            m_ScrollPosition = position;
            // 确保滚动位置在有效范围内
            ClampScrollPosition();
        }

        // 设置滚动方向启用状态
        void EnableHorizontalScroll(bool enable) {
            m_HorizontalScrollEnabled = enable;
            if (!enable) m_ScrollPosition.x = 0; // 禁用时重置水平滚动位置
        }
        void EnableVerticalScroll(bool enable) {
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
            ImGuiWindow* window = ImGui::GetCurrentWindow();
            ImGuiID id = ImGui::GetID(m_WidgetName.c_str());
            ImRect bb(Position, Position + Size);



            // 处理滚动条拖动结束
            if (m_DraggingScrollbar != 0 && !ImGui::IsMouseDown(0))
            {
                m_DraggingScrollbar = 0; // 结束拖动
            }

            // 渲染背景
            RenderBackGround();

            // 计算内容区域（排除滚动条）
            ImVec2 contentAvail = Size;
            if (m_ShowVerticalScrollbar && m_VerticalScrollEnabled) contentAvail.x -= m_ScrollbarThickness;
            if (m_ShowHorizontalScrollbar && m_HorizontalScrollEnabled) contentAvail.y -= m_ScrollbarThickness;

            if (m_Slots.size() >= 1 && m_Slots[0])
            {
                //预设置子项大小

                m_Slots[0]->GetContent()->SetSize(contentAvail);
                // 计算内容大小
                ImVec2 minSize = m_Slots[0]->GetContent()->GetMinSize();

                // 禁用滚动方向：限制子控件大小
                ImVec2 childSize = minSize;
                if (!m_HorizontalScrollEnabled) {
                    childSize.x = ImMin(minSize.x, contentAvail.x);
                }
                if (!m_VerticalScrollEnabled) {
                    childSize.y = ImMin(minSize.y, contentAvail.y);
                }

                // 设置内容区域大小
                m_ContentSize = ImVec2(
                    m_HorizontalScrollEnabled ? minSize.x : childSize.x,
                    m_VerticalScrollEnabled ? minSize.y : childSize.y
                );

                // 开始滚动区域 - 使用自定义滚动而非ImGui自带滚动
                ImGui::SetNextWindowPos(Position);
                ImGui::BeginChild(id, contentAvail, ImGuiChildFlags_None,
                    ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

                // 检测整个滚动框的悬停状态
                bool isHovered = ImGui::IsWindowHovered() &&
                    ImGui::IsMouseHoveringRect(Position, Position + Size);

                // 处理鼠标滚轮滚动
                if (isHovered && ImGui::GetIO().MouseWheel != 0.0f)
                {
                    if (m_VerticalScrollEnabled)
                    {
                        m_ScrollPosition.y -= ImGui::GetIO().MouseWheel * 30.0f;
                        
                    }
                    else if (m_HorizontalScrollEnabled)
                    {
                        m_ScrollPosition.x -= ImGui::GetIO().MouseWheel * 30.0f;
                        
                    }
                }
                ClampScrollPosition();
                // 设置子控件位置
                ImVec2 childPos = ImGui::GetCursorPos();
                if (m_HorizontalScrollEnabled) childPos.x -= m_ScrollPosition.x;
                if (m_VerticalScrollEnabled) childPos.y -= m_ScrollPosition.y;

                // 禁用滚动方向：固定位置
                if (!m_HorizontalScrollEnabled) {
                    childPos.x = 0;
                }
                if (!m_VerticalScrollEnabled) {
                    childPos.y = 0;
                }

                ImGui::SetCursorPos(childPos);
                m_Slots[0]->GetContent()->SetPosition(ImVec2(Position.x + childPos.x, Position.y + childPos.y));
                m_Slots[0]->GetContent()->SetSize(childSize);
                m_Slots[0]->GetContent()->Render();

                // 确保内容区域大小正确
                ImGui::SetCursorPos(ImVec2(0, 0));
                ImGui::Dummy(m_ContentSize);

                ImGui::EndChild();

                // 自定义渲染滚动条
                RenderCustomScrollbars();
            }
        }

    protected:
        // 确保滚动位置在有效范围内
        void ClampScrollPosition()
        {
            float maxScrollX = ImMax(0.0f, m_ContentSize.x - Size.x + (m_ShowVerticalScrollbar ? m_ScrollbarThickness : 0));
            float maxScrollY = ImMax(0.0f, m_ContentSize.y - Size.y + (m_ShowHorizontalScrollbar ? m_ScrollbarThickness : 0));

            if (m_HorizontalScrollEnabled) {
                m_ScrollPosition.x = ImClamp(m_ScrollPosition.x, 0.0f, maxScrollX);
            }
            else {
                m_ScrollPosition.x = 0.0f;
            }

            if (m_VerticalScrollEnabled) {
                m_ScrollPosition.y = ImClamp(m_ScrollPosition.y, 0.0f, maxScrollY);
            }
            else {
                m_ScrollPosition.y = 0.0f;
            }
        }

        // 自定义滚动条渲染
        void RenderCustomScrollbars()
        {
            ImGuiWindow* window = ImGui::GetCurrentWindow();
            const ImRect scrollArea(Position, Position + Size);

            // 渲染水平滚动条（仅在启用水平滚动时）
            if (m_HorizontalScrollEnabled && m_ShowHorizontalScrollbar && m_ContentSize.x > Size.x - (m_ShowVerticalScrollbar ? m_ScrollbarThickness : 0))
            {
                ImRect scrollbarRect(
                    ImVec2(Position.x, Position.y + Size.y - m_ScrollbarThickness),
                    ImVec2(Position.x + Size.x - (m_ShowVerticalScrollbar ? m_ScrollbarThickness : 0),
                        Position.y + Size.y)
                );

                // 计算滑块大小和位置
                float maxScroll = m_ContentSize.x - Size.x + (m_ShowVerticalScrollbar ? m_ScrollbarThickness : 0);
                float grabWidth = ImMax(m_ScrollbarThickness, (Size.x / m_ContentSize.x) * scrollbarRect.GetWidth());
                float grabPosX = scrollbarRect.Min.x + (m_ScrollPosition.x / maxScroll) * (scrollbarRect.GetWidth() - grabWidth);

                ImRect grabRect(
                    ImVec2(grabPosX, scrollbarRect.Min.y),
                    ImVec2(grabPosX + grabWidth, scrollbarRect.Max.y)
                );

                // 绘制滚动条背景
                window->DrawList->AddRectFilled(scrollbarRect.Min, scrollbarRect.Max, m_ScrollbarBgColor);

                // 绘制滑块
                bool isHovered = ImGui::IsMouseHoveringRect(grabRect.Min, grabRect.Max);
                bool isActive = (m_DraggingScrollbar == 1);
                ImU32 grabColor = isActive ? m_ScrollbarGrabActiveColor :
                    isHovered ? m_ScrollbarGrabHoveredColor :
                    m_ScrollbarGrabColor;
                window->DrawList->AddRectFilled(grabRect.Min, grabRect.Max, grabColor);

                // 处理滚动条交互
                HandleScrollbarInteraction(scrollbarRect, grabRect, true);
            }

            // 渲染垂直滚动条（仅在启用垂直滚动时）
            if (m_VerticalScrollEnabled && m_ShowVerticalScrollbar && m_ContentSize.y > Size.y - (m_ShowHorizontalScrollbar ? m_ScrollbarThickness : 0))
            {
                ImRect scrollbarRect(
                    ImVec2(Position.x + Size.x - m_ScrollbarThickness, Position.y),
                    ImVec2(Position.x + Size.x,
                        Position.y + Size.y - (m_ShowHorizontalScrollbar ? m_ScrollbarThickness : 0))
                );

                // 计算滑块大小和位置
                float maxScroll = m_ContentSize.y - Size.y + (m_ShowHorizontalScrollbar ? m_ScrollbarThickness : 0);
                float grabHeight = ImMax(m_ScrollbarThickness, (Size.y / m_ContentSize.y) * scrollbarRect.GetHeight());
                float grabPosY = scrollbarRect.Min.y + (m_ScrollPosition.y / maxScroll) * (scrollbarRect.GetHeight() - grabHeight);

                ImRect grabRect(
                    ImVec2(scrollbarRect.Min.x, grabPosY),
                    ImVec2(scrollbarRect.Max.x, grabPosY + grabHeight)
                );

                // 绘制滚动条背景
                window->DrawList->AddRectFilled(scrollbarRect.Min, scrollbarRect.Max, m_ScrollbarBgColor);

                // 绘制滑块
                bool isHovered = ImGui::IsMouseHoveringRect(grabRect.Min, grabRect.Max);
                bool isActive = (m_DraggingScrollbar == 2);
                ImU32 grabColor = isActive ? m_ScrollbarGrabActiveColor :
                    isHovered ? m_ScrollbarGrabHoveredColor :
                    m_ScrollbarGrabColor;
                window->DrawList->AddRectFilled(grabRect.Min, grabRect.Max, grabColor);

                // 处理滚动条交互
                HandleScrollbarInteraction(scrollbarRect, grabRect, false);
            }
        }

        // 处理滚动条交互（使用鼠标位置）
        void HandleScrollbarInteraction(const ImRect& scrollbarRect, const ImRect& grabRect, bool isHorizontal)
        {
            ImGuiContext& g = *GImGui;

            // 检查是否在滑块上按下鼠标
            if (ImGui::IsMouseHoveringRect(grabRect.Min, grabRect.Max) && ImGui::IsMouseClicked(0))
            {
                // 开始拖动
                m_DraggingScrollbar = isHorizontal ? 1 : 2;
                m_DragStartPos = g.IO.MousePos;

                // 计算滑块内的偏移量（鼠标在滑块上的相对位置）
                if (isHorizontal) {
                    m_DragOffset = g.IO.MousePos.x - grabRect.Min.x;
                }
                else {
                    m_DragOffset = g.IO.MousePos.y - grabRect.Min.y;
                }
            }

            // 处理持续拖动
            if (m_DraggingScrollbar == (isHorizontal ? 1 : 2) && ImGui::IsMouseDown(0))
            {
                if (isHorizontal)
                {
                    // 计算滑块的目标位置（基于鼠标位置）
                    float targetGrabPos = g.IO.MousePos.x - m_DragOffset - scrollbarRect.Min.x;
                    float maxGrabPos = scrollbarRect.GetWidth() - grabRect.GetWidth();

                    // 确保在有效范围内
                    targetGrabPos = ImClamp(targetGrabPos, 0.0f, maxGrabPos);

                    // 计算滚动位置
                    float maxScroll = m_ContentSize.x - Size.x + (m_ShowVerticalScrollbar ? m_ScrollbarThickness : 0);
                    m_ScrollPosition.x = (targetGrabPos / maxGrabPos) * maxScroll;
                }
                else
                {
                    // 计算滑块的目标位置（基于鼠标位置）
                    float targetGrabPos = g.IO.MousePos.y - m_DragOffset - scrollbarRect.Min.y;
                    float maxGrabPos = scrollbarRect.GetHeight() - grabRect.GetHeight();

                    // 确保在有效范围内
                    targetGrabPos = ImClamp(targetGrabPos, 0.0f, maxGrabPos);

                    // 计算滚动位置
                    float maxScroll = m_ContentSize.y - Size.y + (m_ShowHorizontalScrollbar ? m_ScrollbarThickness : 0);
                    m_ScrollPosition.y = (targetGrabPos / maxGrabPos) * maxScroll;
                }

                ClampScrollPosition();
            }
        }
    };
}