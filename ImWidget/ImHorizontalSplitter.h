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

    class ImHorizontalSplitterSlot : public ImPaddingSlot
    {
    public:
        float Ratio = 1.0f;
        float MinSize = 30.0f;

        ImHorizontalSplitterSlot(ImWidget* Content)
            : ImPaddingSlot(Content)
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
        std::vector<ImRect> m_PartRects;  // 缓存部分区域
        std::vector<ImRect> m_BarRects;    // 缓存分隔条区域

    public:
        ImHorizontalSplitter(const std::string& WidgetName)
            : ImPanelWidget(WidgetName)
        {
        }

        void SetSplitterStyle(const ImHorizontalSplitterStyle& style) {
            m_Style = style;
        }

        ImHorizontalSplitterSlot* AddPart(ImWidget* widget, float ratio = 1.0f) 
        {
            auto* slot = AddChildInternal<ImHorizontalSplitterSlot>(widget);
            slot->Ratio = ratio;
            return slot;
        }

        void SetMinSize(int index, float minSize) {
            if (ImSlot* slot = GetSlotAt(index)) {
                if (auto* splitterSlot = dynamic_cast<ImHorizontalSplitterSlot*>(slot)) {
                    splitterSlot->MinSize = minSize;
                    SetLayoutDirty();  // 标记需要重新布局
                }
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
        void ApplyMinSizeConstraints(std::vector<float>& widths, float availableWidth)
        {
            const int numParts = GetSlotNum();
            if (numParts == 0) return;

            // 计算总的最小宽度
            float totalMinWidth = 0;
            for (int i = 0; i < numParts; i++) {
                if (auto* slot = dynamic_cast<ImHorizontalSplitterSlot*>(GetSlotAt(i))) {
                    totalMinWidth += slot->MinSize;
                }
            }

            // 如果可用空间小于最小总宽度，按比例分配
            if (availableWidth < totalMinWidth) {
                for (int i = 0; i < numParts; i++) {
                    if (auto* slot = dynamic_cast<ImHorizontalSplitterSlot*>(GetSlotAt(i))) {
                        widths[i] = (slot->MinSize / totalMinWidth) * availableWidth;
                    }
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
                    if (auto* slot = dynamic_cast<ImHorizontalSplitterSlot*>(GetSlotAt(i))) {
                        if (widths[i] < slot->MinSize) {
                            widths[i] = slot->MinSize;
                            needsAdjustment = true;
                        }
                        remainingWidth -= widths[i];
                    }
                }

                // 如果空间不足，需要重新分配
                if (remainingWidth < 0) {
                    // 计算动态部分的总比例
                    totalDynamicRatio = 0;
                    for (int i = 0; i < numParts; i++) {
                        if (auto* slot = dynamic_cast<ImHorizontalSplitterSlot*>(GetSlotAt(i))) {
                            if (widths[i] > slot->MinSize) {
                                totalDynamicRatio += slot->Ratio;
                            }
                        }
                    }

                    // 按比例调整动态部分
                    if (totalDynamicRatio > 0) {
                        for (int i = 0; i < numParts; i++) {
                            if (auto* slot = dynamic_cast<ImHorizontalSplitterSlot*>(GetSlotAt(i))) {
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
                }
            } while (needsAdjustment);
        }

        // 计算实际宽度
        void CalculateWidths(std::vector<float>& widths, float contentWidth)
        {
            const int numParts = GetSlotNum();
            if (numParts == 0) return;

            // 计算总比例
            float totalRatio = 0.0f;
            for (int i = 0; i < numParts; i++) {
                if (auto* slot = dynamic_cast<ImHorizontalSplitterSlot*>(GetSlotAt(i))) {
                    totalRatio += slot->Ratio;
                }
            }

            // 计算初始宽度
            widths.clear();
            widths.resize(numParts, 0.0f);
            for (int i = 0; i < numParts; i++) {
                if (auto* slot = dynamic_cast<ImHorizontalSplitterSlot*>(GetSlotAt(i))) {
                    widths[i] = (slot->Ratio / totalRatio) * contentWidth;
                }
            }

            // 应用最小尺寸约束
            ApplyMinSizeConstraints(widths, contentWidth);
        }

        // 处理拖动事件
        void ProcessDragEvents(const ImVec2& mousePos)
        {
            ImGuiWindow* window = ImGui::GetCurrentWindow();
            const int numBars = m_BarRects.size();

            for (int i = 0; i < numBars; i++) {
                ImRect& barRect = m_BarRects[i];
                ImGuiID barId = window->GetID((m_WidgetName + "_Bar" + std::to_string(i)).c_str());

                // 注册交互区域
                ImGui::ItemAdd(barRect, barId);
                ImGui::SetItemAllowOverlap();

                // 鼠标悬停效果
                if (ImGui::IsItemHovered()) {
                    ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
                }

                // 开始拖动
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsItemHovered()) {
                    m_DraggingIndex = i;
                    m_DragStartPos = mousePos.x;
                    m_DragStartSplitterPos = barRect.GetCenter().x;
                }
            }

            // 处理拖动中的更新
            if (m_DraggingIndex >= 0 && m_DraggingIndex < numBars) {
                if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                    float newSplitterPos = m_DragStartSplitterPos + (mousePos.x - m_DragStartPos);

                    int leftIndex = m_DraggingIndex;
                    int rightIndex = m_DraggingIndex + 1;

                    if (auto* leftSlot = dynamic_cast<ImHorizontalSplitterSlot*>(GetSlotAt(leftIndex))) {
                        if (auto* rightSlot = dynamic_cast<ImHorizontalSplitterSlot*>(GetSlotAt(rightIndex))) {
                            // 计算新宽度
                            float newLeftWidth = newSplitterPos - m_PartRects[leftIndex].Min.x - m_Style.BarWidth * 0.5f;
                            float newRightWidth = m_PartRects[rightIndex].Max.x - newSplitterPos - m_Style.BarWidth * 0.5f;

                            // 应用最小尺寸约束
                            if (newLeftWidth < leftSlot->MinSize) {
                                newLeftWidth = leftSlot->MinSize;
                                newRightWidth = (m_PartRects[leftIndex].GetWidth() +
                                    m_PartRects[rightIndex].GetWidth()) - newLeftWidth;
                            }
                            if (newRightWidth < rightSlot->MinSize) {
                                newRightWidth = rightSlot->MinSize;
                                newLeftWidth = (m_PartRects[leftIndex].GetWidth() +
                                    m_PartRects[rightIndex].GetWidth()) - newRightWidth;
                            }

                            // 更新比例
                            float totalRatio = leftSlot->Ratio + rightSlot->Ratio;
                            leftSlot->Ratio = totalRatio * newLeftWidth / (newLeftWidth + newRightWidth);
                            rightSlot->Ratio = totalRatio - leftSlot->Ratio;

                            SetLayoutDirty(); // 标记需要重新布局
                        }
                    }
                }
                else if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                    m_DraggingIndex = -1; // 结束拖动
                }
            }
        }

        // 重写布局计算
        virtual void Relayout() override
        {
            ImGuiWindow* window = ImGui::GetCurrentWindow();
            if (window->SkipItems) return;

            const int numParts = GetSlotNum();
            if (numParts == 0) return;

            const int numBars = numParts - 1;
            const float totalBarWidth = numBars * m_Style.BarWidth;
            const float contentWidth = Size.x - totalBarWidth;

            // 计算实际宽度
            std::vector<float> widths;
            CalculateWidths(widths, contentWidth);

            // 计算位置
            float cursorX = Position.x;
            m_PartRects.clear();
            m_BarRects.clear();

            // 计算每个部分的矩形区域
            for (int i = 0; i < numParts; i++) {
                // 部件区域
                m_PartRects.emplace_back(
                    cursorX,
                    Position.y,
                    cursorX + widths[i],
                    Position.y + Size.y
                );
                cursorX += widths[i];

                // 如果不是最后一部分，添加分隔条
                if (i < numBars) {
                    m_BarRects.emplace_back(
                        cursorX,
                        Position.y,
                        cursorX + m_Style.BarWidth,
                        Position.y + Size.y
                    );
                    cursorX += m_Style.BarWidth;
                }
            }

            // 设置子控件的位置和大小
            for (int i = 0; i < numParts; i++) {
                if (ImSlot* slot = GetSlotAt(i)) {
                    if (slot->IsValid()) {
                        // 只需设置slot的位置和大小
                        slot->SetSlotPosition(m_PartRects[i].Min);
                        slot->SetSlotSize(m_PartRects[i].GetSize());
                        slot->ApplyLayout();
                    }
                }
            }
        }

    public:
        virtual void Render() override
        {
            ImGuiWindow* window = ImGui::GetCurrentWindow();

            const ImVec2 mousePos = ImGui::GetIO().MousePos;
            // 处理拖动事件
            ProcessDragEvents(mousePos);
            HandleLayout(); // 确保布局是最新的

            // 渲染背景和子控件（使用基类提供的接口）
            RenderBackGround();
            RenderChild();

            // 渲染所有分隔条
            for (int i = 0; i < m_BarRects.size(); i++) {
                RenderSplitterBar(m_BarRects[i], mousePos, i);
            }
        }
    };
}