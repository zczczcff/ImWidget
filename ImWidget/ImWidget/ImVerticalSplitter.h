#pragma once
#include "ImPanelWidget.h"
#include "imgui_internal.h"

namespace ImGuiWidget
{
    struct ImVerticalSplitterStyle : public PropertyStruct {
        float BarHeight = 4.0f;
        ImU32 Color = IM_COL32(100, 100, 100, 255);
        ImU32 HoveredColor = IM_COL32(120, 120, 120, 255);
        ImU32 ActiveColor = IM_COL32(150, 150, 150, 255);
        float Rounding = 0.0f;

        virtual std::unordered_set<PropertyInfo, PropertyInfo::Hasher> GetProperties() override {
            std::unordered_set<PropertyInfo, PropertyInfo::Hasher> props;

            props.insert({
                "BarHeight",
                PropertyType::Float,
                "Appearance",
                [this](void* val) { this->BarHeight = *static_cast<float*>(val); },
                [this]() { return static_cast<void*>(&this->BarHeight); }
                });

            props.insert({
                "Color",
                PropertyType::Color,
                "Appearance",
                [this](void* val) { this->Color = *static_cast<ImU32*>(val); },
                [this]() { return static_cast<void*>(&this->Color); }
                });

            props.insert({
                "HoveredColor",
                PropertyType::Color,
                "Appearance",
                [this](void* val) { this->HoveredColor = *static_cast<ImU32*>(val); },
                [this]() { return static_cast<void*>(&this->HoveredColor); }
                });

            props.insert({
                "ActiveColor",
                PropertyType::Color,
                "Appearance",
                [this](void* val) { this->ActiveColor = *static_cast<ImU32*>(val); },
                [this]() { return static_cast<void*>(&this->ActiveColor); }
                });

            props.insert({
                "Rounding",
                PropertyType::Float,
                "Appearance",
                [this](void* val) { this->Rounding = *static_cast<float*>(val); },
                [this]() { return static_cast<void*>(&this->Rounding); }
                });

            return props;
        }
    };

    class ImVerticalSplitterSlot : public ImPaddingSlot
    {
    public:
        float Ratio = 1.0f;
        float MinSize = 30.0f;

        ImVerticalSplitterSlot(ImWidget* Content)
            : ImPaddingSlot(Content)
        {
        }

        virtual std::unordered_set<PropertyInfo, PropertyInfo::Hasher> GetProperties()
        {
            auto props = ImPaddingSlot::GetProperties();

            props.insert({
                "Ratio",
                PropertyType::Float,
                "Layout",
                [this](void* val) { this->Ratio = *static_cast<float*>(val); },
                [this]() { return static_cast<void*>(&this->Ratio); }
                });

            props.insert({
                "MinSize",
                PropertyType::Float,
                "Layout",
                [this](void* val) { this->MinSize = *static_cast<float*>(val); },
                [this]() { return static_cast<void*>(&this->MinSize); }
                });

            return props;
        }

        virtual ImSlot* CopySlot() override
        {
            return new ImVerticalSplitterSlot(*this);
        }
    };

    class ImVerticalSplitter : public ImPanelWidget
    {
    private:
        ImVerticalSplitterStyle m_Style;
        int m_DraggingIndex = -1;
        float m_DragStartPos = 0.0f;
        float m_DragStartSplitterPos = 0.0f;
        std::vector<ImRect> m_PartRects;  // 缓存部分区域
        std::vector<ImRect> m_BarRects;    // 缓存分隔条区域
    protected:
        virtual ImSlot* CreateSlot(ImWidget* Content)override
        {
            return new ImVerticalSplitterSlot(Content);
        }
    public:
        ImVerticalSplitter(const std::string& WidgetName)
            : ImPanelWidget(WidgetName)
        {
        }

        void SetSplitterStyle(const ImVerticalSplitterStyle& style) {
            m_Style = style;
        }

        ImVerticalSplitterSlot* AddPart(ImWidget* widget, float ratio = 1.0f) 
        {
            //auto* slot = new ImVerticalSplitterSlot(widget);
            //slot->Ratio = ratio;
            //AddChild(slot->GetContent());  // 使用基类AddChild
            //return slot;
            auto* slot = AddChildInternal<ImVerticalSplitterSlot>(widget);
            slot->Ratio = ratio;
            return slot;
        }

        void SetMinSize(int index, float minSize) {
            if (ImSlot* slot = GetSlotAt(index)) {
                if (auto* splitterSlot = dynamic_cast<ImVerticalSplitterSlot*>(slot)) {
                    splitterSlot->MinSize = minSize;
                    MarkLayoutDirty();  // 标记需要重新布局
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
        void ApplyMinSizeConstraints(std::vector<float>& heights, float availableHeight)
        {
            const int numParts = GetSlotNum();
            if (numParts == 0) return;

            // 计算总的最小高度
            float totalMinHeight = 0;
            for (int i = 0; i < numParts; i++) {
                if (auto* slot = dynamic_cast<ImVerticalSplitterSlot*>(GetSlotAt(i))) {
                    totalMinHeight += slot->MinSize;
                }
            }

            // 如果可用空间小于最小总高度，按比例分配
            if (availableHeight < totalMinHeight) {
                for (int i = 0; i < numParts; i++) {
                    if (auto* slot = dynamic_cast<ImVerticalSplitterSlot*>(GetSlotAt(i))) {
                        heights[i] = (slot->MinSize / totalMinHeight) * availableHeight;
                    }
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
                    if (auto* slot = dynamic_cast<ImVerticalSplitterSlot*>(GetSlotAt(i))) {
                        if (heights[i] < slot->MinSize) {
                            heights[i] = slot->MinSize;
                            needsAdjustment = true;
                        }
                        remainingHeight -= heights[i];
                    }
                }

                // 如果空间不足，需要重新分配
                if (remainingHeight < 0) {
                    // 计算动态部分的总比例
                    totalDynamicRatio = 0;
                    for (int i = 0; i < numParts; i++) {
                        if (auto* slot = dynamic_cast<ImVerticalSplitterSlot*>(GetSlotAt(i))) {
                            if (heights[i] > slot->MinSize) {
                                totalDynamicRatio += slot->Ratio;
                            }
                        }
                    }

                    // 按比例调整动态部分
                    if (totalDynamicRatio > 0) {
                        for (int i = 0; i < numParts; i++) {
                            if (auto* slot = dynamic_cast<ImVerticalSplitterSlot*>(GetSlotAt(i))) {
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
                }
            } while (needsAdjustment);
        }

        // 计算实际高度
        void CalculateHeights(std::vector<float>& heights, float contentHeight)
        {
            const int numParts = GetSlotNum();
            if (numParts == 0) return;

            // 计算总比例
            float totalRatio = 0.0f;
            for (int i = 0; i < numParts; i++) {
                if (auto* slot = dynamic_cast<ImVerticalSplitterSlot*>(GetSlotAt(i))) {
                    totalRatio += slot->Ratio;
                }
            }

            // 计算初始高度
            heights.clear();
            heights.resize(numParts, 0.0f);
            for (int i = 0; i < numParts; i++) {
                if (auto* slot = dynamic_cast<ImVerticalSplitterSlot*>(GetSlotAt(i))) {
                    heights[i] = (slot->Ratio / totalRatio) * contentHeight;
                }
            }

            // 应用最小尺寸约束
            ApplyMinSizeConstraints(heights, contentHeight);
        }

        // 处理拖动事件
        void ProcessDragEvents(const ImVec2& mousePos)
        {
            ImGuiWindow* window = ImGui::GetCurrentWindow();
            const int numBars = m_BarRects.size();

            for (int i = 0; i < numBars; i++) {
                ImRect& barRect = m_BarRects[i];
                ImGuiID barId = window->GetID((m_WidgetID + "_Bar" + std::to_string(i)).c_str());

                // 注册交互区域
                ImGui::ItemAdd(barRect, barId);
                ImGui::SetItemAllowOverlap();

                // 鼠标悬停效果
                if (ImGui::IsItemHovered()) {
                    ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
                }

                // 开始拖动
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsItemHovered()) {
                    m_DraggingIndex = i;
                    m_DragStartPos = mousePos.y;
                    m_DragStartSplitterPos = barRect.GetCenter().y;
                }
            }

            // 处理拖动中的更新
            if (m_DraggingIndex >= 0 && m_DraggingIndex < numBars) {
                if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                    float newSplitterPos = m_DragStartSplitterPos + (mousePos.y - m_DragStartPos);

                    int topIndex = m_DraggingIndex;
                    int bottomIndex = m_DraggingIndex + 1;

                    if (auto* topSlot = dynamic_cast<ImVerticalSplitterSlot*>(GetSlotAt(topIndex))) {
                        if (auto* bottomSlot = dynamic_cast<ImVerticalSplitterSlot*>(GetSlotAt(bottomIndex))) {
                            // 计算新高度
                            float newTopHeight = newSplitterPos - m_PartRects[topIndex].Min.y - m_Style.BarHeight * 0.5f;
                            float newBottomHeight = m_PartRects[bottomIndex].Max.y - newSplitterPos - m_Style.BarHeight * 0.5f;

                            // 应用最小尺寸约束
                            if (newTopHeight < topSlot->MinSize) {
                                newTopHeight = topSlot->MinSize;
                                newBottomHeight = (m_PartRects[topIndex].GetHeight() +
                                    m_PartRects[bottomIndex].GetHeight()) - newTopHeight;
                            }
                            if (newBottomHeight < bottomSlot->MinSize) {
                                newBottomHeight = bottomSlot->MinSize;
                                newTopHeight = (m_PartRects[topIndex].GetHeight() +
                                    m_PartRects[bottomIndex].GetHeight()) - newBottomHeight;
                            }

                            // 更新比例
                            float totalRatio = topSlot->Ratio + bottomSlot->Ratio;
                            topSlot->Ratio = totalRatio * newTopHeight / (newTopHeight + newBottomHeight);
                            bottomSlot->Ratio = totalRatio - topSlot->Ratio;

                            MarkLayoutDirty(); // 标记需要重新布局
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
            const float totalBarHeight = numBars * m_Style.BarHeight;
            const float contentHeight = Size.y - totalBarHeight;

            // 计算实际高度
            std::vector<float> heights;
            CalculateHeights(heights, contentHeight);

            // 计算位置
            float cursorY = Position.y;
            m_PartRects.clear();
            m_BarRects.clear();

            // 计算每个部分的矩形区域
            for (int i = 0; i < numParts; i++) {
                // 部件区域
                m_PartRects.emplace_back(
                    Position.x,
                    cursorY,
                    Position.x + Size.x,
                    cursorY + heights[i]
                );
                cursorY += heights[i];

                // 如果不是最后一部分，添加分隔条
                if (i < numBars) {
                    m_BarRects.emplace_back(
                        Position.x,
                        cursorY,
                        Position.x + Size.x,
                        cursorY + m_Style.BarHeight
                    );
                    cursorY += m_Style.BarHeight;
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
        virtual ImSlot* AddChild(ImWidget* Child, ImVec2 RelativePosition = ImVec2(FLT_MIN, FLT_MIN))
        {
            // 如果没有指定相对位置，添加到末尾
            if (RelativePosition.y == FLT_MIN) {
                return AddPart(Child, 1.0f); // 使用AddPart添加并设置默认比例
            }

            // 计算在分割器中的相对位置
            float relativeY = RelativePosition.y;

            // 如果没有子项，直接添加到末尾
            if (GetSlotNum() == 0) {
                return AddPart(Child, 1.0f);
            }

            // 确保布局是最新的
            HandleLayout();

            // 遍历所有部分区域，寻找插入位置
            int insertIndex = GetSlotNum(); // 默认插入到最后
            float accumulatedHeight = 0.0f;

            for (int i = 0; i < GetSlotNum(); i++) {
                const ImRect& partRect = m_PartRects[i];
                float partHeight = partRect.GetHeight();

                // 如果位置在当前部分中点之前，插入在当前位置
                if (relativeY <= accumulatedHeight + partHeight * 0.5f) {
                    insertIndex = i;
                    break;
                }

                accumulatedHeight += partHeight;

                // 如果不是最后一部分，加上分隔条高度
                if (i < static_cast<int>(m_BarRects.size())) {
                    accumulatedHeight += m_BarRects[i].GetHeight();
                }
            }

            // 使用ImPanelWidget提供的API插入子项
            ImSlot* newSlot = InsertChildAt(insertIndex, Child);
            if (!newSlot) return nullptr;
            // 标记需要重新布局
            MarkLayoutDirty();

            return newSlot;
        }

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

        virtual std::unordered_set<PropertyInfo, PropertyInfo::Hasher> GetProperties() override
        {
            auto props = ImPanelWidget::GetProperties();

            // 分隔条样式
            props.insert({
                "SplitterStyle",
                PropertyType::Struct,
                "Appearance",
                [this](void* val) { this->m_Style = *static_cast<ImVerticalSplitterStyle*>(val); },
                [this]() { return static_cast<void*>(&this->m_Style); }
                });

            return props;
        }

        virtual std::string GetRegisterTypeName()override { return "ImVerticalSplitter"; }

        virtual ImWidget* CopyWidget()
        {
            return new ImVerticalSplitter(*this);
        }
    };
}