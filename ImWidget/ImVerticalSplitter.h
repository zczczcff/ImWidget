#pragma once
#include "ImPanelWidget.h"
#include "imgui_internal.h"

namespace ImGuiWidget
{
    struct ImSplitterStyle
    {
        float BarHeight = 4.0f;
        ImU32 Color = IM_COL32(100, 100, 100, 255);
        ImU32 HoveredColor = IM_COL32(120, 120, 120, 255);
        ImU32 ActiveColor = IM_COL32(150, 150, 150, 255);
        float Rounding = 0.0f;
    };

    class ImVerticalSplitterSlot : public ImSlot
    {
    public:
        float Ratio = 1.0f;
        float MinSize = 30.0f;

        ImVerticalSplitterSlot(ImWidget* Content)
            : ImSlot(Content)
        {
        }
    };

    class ImVerticalSplitter : public ImPanelWidget
    {
    private:
        ImSplitterStyle m_Style;
        int m_DraggingIndex = -1;
        float m_DragStartPos = 0.0f;
        float m_DragStartSplitterPos = 0.0f;

    public:
        ImVerticalSplitter(const std::string& WidgetName)
            : ImPanelWidget(WidgetName)
        {
        }

        void SetSplitterStyle(const ImSplitterStyle& style) {
            m_Style = style;
        }

        ImVerticalSplitterSlot* AddPart(ImWidget* widget, float ratio = 1.0f) {
            auto* slot = new ImVerticalSplitterSlot(widget);
            slot->Ratio = ratio;
            m_Slots.push_back(slot);
            return slot;
        }

        void SetMinSize(int index, float minSize) {
            if (index >= 0 && index < m_Slots.size()) {
                static_cast<ImVerticalSplitterSlot*>(m_Slots[index])->MinSize = minSize;
            }
        }

    private:
        // 绘制分隔条
        void RenderSplitterBar(const ImRect& barRect, const ImVec2& MousePos, int barIndex)
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
        void ApplyMinSizeConstraints(std::vector<float>& heights, float availableHeight)
        {
            const int numParts = m_Slots.size();
            if (numParts == 0) return;

            // 计算总的最小高度
            float totalMinHeight = 0;
            for (int i = 0; i < numParts; i++) {
                auto* slot = static_cast<ImVerticalSplitterSlot*>(m_Slots[i]);
                totalMinHeight += slot->MinSize;
            }

            // 如果可用空间小于最小总高度，按比例分配
            if (availableHeight < totalMinHeight) {
                for (int i = 0; i < numParts; i++) {
                    auto* slot = static_cast<ImVerticalSplitterSlot*>(m_Slots[i]);
                    heights[i] = (slot->MinSize / totalMinHeight) * availableHeight;
                }
                return;
            }

            // 应用最小尺寸约束
            bool needsAdjustment;
            do {
                needsAdjustment = false;
                float remainingHeight = availableHeight;
                float totalDynamicRatio = 0.0f;

                // 设置所有部件的最小尺寸
                for (int i = 0; i < numParts; i++) {
                    auto* slot = static_cast<ImVerticalSplitterSlot*>(m_Slots[i]);
                    if (heights[i] < slot->MinSize) {
                        heights[i] = slot->MinSize;
                        needsAdjustment = true;
                    }
                    remainingHeight -= heights[i];
                }

                // 如果空间不足，需要重新分配
                if (remainingHeight < 0) {
                    // 计算动态部分的总比例
                    totalDynamicRatio = 0;
                    for (int i = 0; i < numParts; i++) {
                        auto* slot = static_cast<ImVerticalSplitterSlot*>(m_Slots[i]);
                        if (heights[i] > slot->MinSize) {
                            totalDynamicRatio += slot->Ratio;
                        }
                    }

                    // 按比例调整动态部分
                    if (totalDynamicRatio > 0) {
                        for (int i = 0; i < numParts; i++) {
                            auto* slot = static_cast<ImVerticalSplitterSlot*>(m_Slots[i]);
                            if (heights[i] > slot->MinSize) {
                                float reduction = (remainingHeight * slot->Ratio / totalDynamicRatio);
                                heights[i] += reduction;
                                if (heights[i] < slot->MinSize) {
                                    heights[i] = slot->MinSize;
                                    needsAdjustment = true;
                                }
                            }
                        }
                    }
                }
            } while (needsAdjustment);
        }

        // 计算实际高度
        void CalculateHeights(std::vector<float>& heights, float contentHeight)
        {
            const int numParts = m_Slots.size();
            if (numParts == 0) return;

            // 计算总比例
            float totalRatio = 0.0f;
            for (auto* slot : m_Slots) {
                auto* splitterSlot = static_cast<ImVerticalSplitterSlot*>(slot);
                totalRatio += splitterSlot->Ratio;
            }

            // 计算初始高度
            heights.clear();
            for (auto* slot : m_Slots) {
                auto* splitterSlot = static_cast<ImVerticalSplitterSlot*>(slot);
                heights.push_back((splitterSlot->Ratio / totalRatio) * contentHeight);
            }

            // 应用最小尺寸约束
            ApplyMinSizeConstraints(heights, contentHeight);
        }

    public:
        virtual void Render() override
        {
            ImGuiWindow* window = ImGui::GetCurrentWindow();
            if (window->SkipItems || m_Slots.empty()) return;

            const int numParts = m_Slots.size();
            const int numBars = numParts - 1;
            const float totalBarHeight = numBars * m_Style.BarHeight;
            const float contentHeight = Size.y - totalBarHeight;

            // 计算实际高度
            std::vector<float> heights;
            CalculateHeights(heights, contentHeight);

            // 计算位置
            float cursorY = Position.y;
            std::vector<ImRect> partRects;
            std::vector<ImRect> barRects;

            // 计算每个部分的矩形区域
            for (int i = 0; i < numParts; i++) {
                // 部件区域
                ImRect partRect(
                    Position.x,
                    cursorY,
                    Position.x + Size.x,
                    cursorY + heights[i]
                );
                partRects.push_back(partRect);
                cursorY += heights[i];

                // 如果不是最后一部分，添加分隔条
                if (i < numBars) {
                    ImRect barRect(
                        Position.x,
                        cursorY,
                        Position.x + Size.x,
                        cursorY + m_Style.BarHeight
                    );
                    barRects.push_back(barRect);
                    cursorY += m_Style.BarHeight;
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
                    ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
                }

                // 开始拖动
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsItemHovered()) {
                    m_DraggingIndex = i;
                    m_DragStartPos = ImGui::GetIO().MousePos.y;
                    m_DragStartSplitterPos = barRects[i].GetCenter().y;
                }
            }

            // 获取当前鼠标位置
            const ImVec2 MousePos = ImGui::GetIO().MousePos;
            const float currentMousePos = MousePos.y;

            // 处理拖动中的更新
            if (m_DraggingIndex >= 0 && m_DraggingIndex < numBars) {
                if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {

                    // 计算新的分隔条位置
                    float newSplitterPos = m_DragStartSplitterPos + (currentMousePos - m_DragStartPos);

                    // 获取上下部分的slot
                    auto* topSlot = static_cast<ImVerticalSplitterSlot*>(m_Slots[m_DraggingIndex]);
                    auto* bottomSlot = static_cast<ImVerticalSplitterSlot*>(m_Slots[m_DraggingIndex + 1]);

                    // 计算顶部部分的新高度
                    float newTopHeight = newSplitterPos - partRects[m_DraggingIndex].Min.y - m_Style.BarHeight * 0.5f;

                    // 计算底部部分的新高度
                    float newBottomHeight = partRects[m_DraggingIndex + 1].Max.y - newSplitterPos - m_Style.BarHeight * 0.5f;

                    // 应用最小尺寸约束
                    if (newTopHeight < topSlot->MinSize) {
                        newTopHeight = topSlot->MinSize;
                        newBottomHeight = heights[m_DraggingIndex] + heights[m_DraggingIndex + 1] - newTopHeight;
                    }
                    if (newBottomHeight < bottomSlot->MinSize) {
                        newBottomHeight = bottomSlot->MinSize;
                        newTopHeight = heights[m_DraggingIndex] + heights[m_DraggingIndex + 1] - newBottomHeight;
                    }

                    // 更新比例
                    float totalRatio = topSlot->Ratio + bottomSlot->Ratio;
                    topSlot->Ratio = totalRatio * newTopHeight / (newTopHeight + newBottomHeight);
                    bottomSlot->Ratio = totalRatio - topSlot->Ratio;

                    // 立即重新计算布局
                    CalculateHeights(heights, contentHeight);

                    // 更新部件矩形区域
                    float updatedCursorY = Position.y;
                    partRects.clear();
                    barRects.clear();

                    for (int i = 0; i < numParts; i++) {
                        ImRect partRect(
                            Position.x,
                            updatedCursorY,
                            Position.x + Size.x,
                            updatedCursorY + heights[i]
                        );
                        partRects.push_back(partRect);
                        updatedCursorY += heights[i];

                        if (i < numBars) {
                            ImRect barRect(
                                Position.x,
                                updatedCursorY,
                                Position.x + Size.x,
                                updatedCursorY + m_Style.BarHeight
                            );
                            barRects.push_back(barRect);
                            updatedCursorY += m_Style.BarHeight;
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
                RenderSplitterBar(barRects[i], MousePos, i);
            }
        }
    };
}