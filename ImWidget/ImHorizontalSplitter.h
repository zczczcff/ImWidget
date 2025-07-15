#pragma once
#include "ImPanelWidget.h"
#include "imgui_internal.h"

namespace ImGuiWidget
{
    struct ImHorizontalSplitterStyle
    {
        float BarWidth = 4.0f;
        ImU32 Color = IM_COL32(100, 100, 100, 255);
        ImU32 HoveredColor = IM_COL32(120, 120, 120, 255);
        ImU32 ActiveColor = IM_COL32(150, 150, 150, 255);
        float Rounding = 0.0f;
    };

    class ImHorizontalSplitterSlot : public ImSlot
    {
    public:
        float Ratio = 1.0f;
        float MinSize = 30.0f;

        ImHorizontalSplitterSlot(ImWidget* Content)
            : ImSlot(Content)
        {
        }
    };

    class ImHorizontalSplitter : public ImPanelWidget
    {
    private:
        ImHorizontalSplitterStyle m_Style;
        int m_DraggingIndex = -1;
        float m_DragStartPos = 0.0f;
        float m_DragStartSplitterPos = 0.0f;

    public:
        ImHorizontalSplitter(const std::string& WidgetName)
            : ImPanelWidget(WidgetName)
        {
        }

        void SetSplitterStyle(const ImHorizontalSplitterStyle& style) {
            m_Style = style;
        }

        ImHorizontalSplitterSlot* AddPart(ImWidget* widget, float ratio = 1.0f) {
            auto* slot = new ImHorizontalSplitterSlot(widget);
            slot->Ratio = ratio;
            m_Slots.push_back(slot);
            return slot;
        }

        void SetMinSize(int index, float minSize) {
            if (index >= 0 && index < m_Slots.size()) {
                static_cast<ImHorizontalSplitterSlot*>(m_Slots[index])->MinSize = minSize;
            }
        }

    private:
        // 绘制分隔条
        void RenderSplitterBar(const ImRect& barRect, const ImVec2& MousePos,int barIndex)
        {
            ImGuiWindow* window = ImGui::GetCurrentWindow();
            ImU32 color;
            
            if (m_DraggingIndex == barIndex) 
            {
                color = m_Style.ActiveColor;
            }
            else if (barRect.Contains(MousePos))
            {
                color = m_Style.HoveredColor;
            }
            else 
            {
                color = m_Style.Color;
            }

            window->DrawList->AddRectFilled(
                barRect.Min,
                barRect.Max,
                color,
                m_Style.Rounding
            );
        }

        // 应用最小尺寸约束
        void ApplyMinSizeConstraints(std::vector<float>& widths, float availableWidth)
        {
            const int numParts = m_Slots.size();
            if (numParts == 0) return;

            // 计算总的最小宽度
            float totalMinWidth = 0;
            for (int i = 0; i < numParts; i++) {
                auto* slot = static_cast<ImHorizontalSplitterSlot*>(m_Slots[i]);
                totalMinWidth += slot->MinSize;
            }

            // 如果可用空间小于最小总宽度，按比例分配
            if (availableWidth < totalMinWidth) {
                for (int i = 0; i < numParts; i++) {
                    auto* slot = static_cast<ImHorizontalSplitterSlot*>(m_Slots[i]);
                    widths[i] = (slot->MinSize / totalMinWidth) * availableWidth;
                }
                return;
            }

            // 应用最小尺寸约束
            bool needsAdjustment;
            do {
                needsAdjustment = false;
                float remainingWidth = availableWidth;
                float totalDynamicRatio = 0.0f;

                // 设置所有部件的最小尺寸
                for (int i = 0; i < numParts; i++) {
                    auto* slot = static_cast<ImHorizontalSplitterSlot*>(m_Slots[i]);
                    if (widths[i] < slot->MinSize) {
                        widths[i] = slot->MinSize;
                        needsAdjustment = true;
                    }
                    remainingWidth -= widths[i];
                }

                // 如果空间不足，需要重新分配
                if (remainingWidth < 0) {
                    // 计算动态部分的总比例
                    totalDynamicRatio = 0;
                    for (int i = 0; i < numParts; i++) {
                        auto* slot = static_cast<ImHorizontalSplitterSlot*>(m_Slots[i]);
                        if (widths[i] > slot->MinSize) {
                            totalDynamicRatio += slot->Ratio;
                        }
                    }

                    // 按比例调整动态部分
                    if (totalDynamicRatio > 0) {
                        for (int i = 0; i < numParts; i++) {
                            auto* slot = static_cast<ImHorizontalSplitterSlot*>(m_Slots[i]);
                            if (widths[i] > slot->MinSize) {
                                float reduction = (remainingWidth * slot->Ratio / totalDynamicRatio);
                                widths[i] += reduction;
                                if (widths[i] < slot->MinSize) {
                                    widths[i] = slot->MinSize;
                                    needsAdjustment = true;
                                }
                            }
                        }
                    }
                }
            } while (needsAdjustment);
        }

        // 计算实际宽度
        void CalculateWidths(std::vector<float>& widths, float contentWidth)
        {
            const int numParts = m_Slots.size();
            if (numParts == 0) return;

            // 计算总比例
            float totalRatio = 0.0f;
            for (auto* slot : m_Slots) {
                auto* splitterSlot = static_cast<ImHorizontalSplitterSlot*>(slot);
                totalRatio += splitterSlot->Ratio;
            }

            // 计算初始宽度
            widths.clear();
            for (auto* slot : m_Slots) {
                auto* splitterSlot = static_cast<ImHorizontalSplitterSlot*>(slot);
                widths.push_back((splitterSlot->Ratio / totalRatio) * contentWidth);
            }

            // 应用最小尺寸约束
            ApplyMinSizeConstraints(widths, contentWidth);
        }

    public:
        virtual void Render() override
        {
            ImGuiWindow* window = ImGui::GetCurrentWindow();
            if (window->SkipItems || m_Slots.empty()) return;

            const int numParts = m_Slots.size();
            const int numBars = numParts - 1;
            const float totalBarWidth = numBars * m_Style.BarWidth;
            const float contentWidth = Size.x - totalBarWidth;

            // 计算实际宽度
            std::vector<float> widths;
            CalculateWidths(widths, contentWidth);

            // 计算位置
            float cursorX = Position.x;
            std::vector<ImRect> partRects;
            std::vector<ImRect> barRects;

            // 计算每个部分的矩形区域
            for (int i = 0; i < numParts; i++) {
                // 部件区域
                ImRect partRect(
                    cursorX,
                    Position.y,
                    cursorX + widths[i],
                    Position.y + Size.y
                );
                partRects.push_back(partRect);
                cursorX += widths[i];

                // 如果不是最后一部分，添加分隔条
                if (i < numBars) {
                    ImRect barRect(
                        cursorX,
                        Position.y,
                        cursorX + m_Style.BarWidth,
                        Position.y + Size.y
                    );
                    barRects.push_back(barRect);
                    cursorX += m_Style.BarWidth;
                }
            }

            // 处理拖动事件
            for (int i = 0; i < numBars; i++) {
                ImGuiID barId = window->GetID((m_WidgetName + "_Bar" + std::to_string(i)).c_str());

                // 注册交互区域
                ImGui::ItemAdd(barRects[i], barId);
                ImGui::SetItemAllowOverlap();

                // 鼠标悬停效果
                if (ImGui::IsItemHovered()) {
                    ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
                }

                // 开始拖动
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsItemHovered()) {
                    m_DraggingIndex = i;
                    m_DragStartPos = ImGui::GetIO().MousePos.x;
                    m_DragStartSplitterPos = barRects[i].GetCenter().x;
                }
            }

            // 获取当前鼠标位置
            const ImVec2 MousePos = ImGui::GetIO().MousePos;
            const float currentMousePos = MousePos.x;

            // 处理拖动中的更新
            if (m_DraggingIndex >= 0 && m_DraggingIndex < numBars) {
                if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                    
                    // 计算新的分隔条位置
                    float newSplitterPos = m_DragStartSplitterPos + (currentMousePos - m_DragStartPos);

                    // 获取左右部分的slot
                    auto* leftSlot = static_cast<ImHorizontalSplitterSlot*>(m_Slots[m_DraggingIndex]);
                    auto* rightSlot = static_cast<ImHorizontalSplitterSlot*>(m_Slots[m_DraggingIndex + 1]);

                    // 计算左侧部分的新宽度
                    float newLeftWidth = newSplitterPos - partRects[m_DraggingIndex].Min.x - m_Style.BarWidth * 0.5f;

                    // 计算右侧部分的新宽度
                    float newRightWidth = partRects[m_DraggingIndex + 1].Max.x - newSplitterPos - m_Style.BarWidth * 0.5f;

                    // 应用最小尺寸约束
                    if (newLeftWidth < leftSlot->MinSize) {
                        newLeftWidth = leftSlot->MinSize;
                        newRightWidth = widths[m_DraggingIndex] + widths[m_DraggingIndex + 1] - newLeftWidth;
                    }
                    if (newRightWidth < rightSlot->MinSize) {
                        newRightWidth = rightSlot->MinSize;
                        newLeftWidth = widths[m_DraggingIndex] + widths[m_DraggingIndex + 1] - newRightWidth;
                    }

                    // 更新比例
                    float totalRatio = leftSlot->Ratio + rightSlot->Ratio;
                    leftSlot->Ratio = totalRatio * newLeftWidth / (newLeftWidth + newRightWidth);
                    rightSlot->Ratio = totalRatio - leftSlot->Ratio;

                    // 立即重新计算布局
                    CalculateWidths(widths, contentWidth);

                    // 更新部件矩形区域
                    float updatedCursorX = Position.x;
                    partRects.clear();
                    barRects.clear();

                    for (int i = 0; i < numParts; i++) {
                        ImRect partRect(
                            updatedCursorX,
                            Position.y,
                            updatedCursorX + widths[i],
                            Position.y + Size.y
                        );
                        partRects.push_back(partRect);
                        updatedCursorX += widths[i];

                        if (i < numBars) {
                            ImRect barRect(
                                updatedCursorX,
                                Position.y,
                                updatedCursorX + m_Style.BarWidth,
                                Position.y + Size.y
                            );
                            barRects.push_back(barRect);
                            updatedCursorX += m_Style.BarWidth;
                        }
                    }
                }
                else if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                    // 结束拖动
                    m_DraggingIndex = -1;
                }
            }

            // 渲染所有部件
            for (int i = 0; i < numParts; i++) {
                m_Slots[i]->GetContent()->SetPosition(partRects[i].Min);
                m_Slots[i]->GetContent()->SetSize(partRects[i].GetSize());
                m_Slots[i]->GetContent()->Render();
            }

            // 渲染所有分隔条
            for (int i = 0; i < numBars; i++) {
                RenderSplitterBar(barRects[i], MousePos ,i);
            }
        }
    };
}