#pragma once
#include "ImPanelWidget.h"
#include "imgui_internal.h"
#include "ImEvent/ImDragEvent.h"

namespace ImGuiWidget
{
    struct ImVerticalSplitterStyle : public PropertyStruct
    {
        float BarHeight = 4.0f;
        ImU32 Color = IM_COL32(100, 100, 100, 255);
        ImU32 HoveredColor = IM_COL32(120, 120, 120, 255);
        ImU32 ActiveColor = IM_COL32(150, 150, 150, 255);
        float Rounding = 0.0f;

        virtual std::unordered_set<PropertyInfo, PropertyInfo::Hasher> GetProperties() override
        {
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

        // 拖拽状态
        bool m_IsDragSource = false; // 当前控件是否是拖拽源
        ImVec2 m_DragStartPosition; // 拖拽起始位置

        // 鼠标悬停状态
        int m_HoveredBarIndex = -1; // 当前悬停的分隔条索引
        bool m_IsMouseOver = false; // 鼠标是否在控件上方

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

        // 重写事件处理函数
        virtual void HandleEvent(ImEvent* event) override
        {
            if (event->IsHandled()) return;

            // 处理拖拽相关事件
            if (auto dragEvent = event->As<ImDragEvent>())
            {
                HandleDragEvent(dragEvent);
            }

            // 处理鼠标悬停事件
            if (auto mouseEvent = event->As<ImMouseEvent>())
            {
                HandleMouseEvent(mouseEvent);
            }
        }

    private:
        // 处理鼠标事件（悬停检测）
        void HandleMouseEvent(ImMouseEvent* mouseEvent)
        {
            const ImVec2 mousePos = mouseEvent->GetPosition();

            switch (mouseEvent->GetType())
            {
            case ImEventType::MouseEnter:
                HandleMouseEnter(mousePos, mouseEvent);
                break;
            case ImEventType::MouseLeave:
                HandleMouseLeave(mousePos, mouseEvent);
                break;
            case ImEventType::MouseMove:
                HandleMouseMove(mousePos, mouseEvent);
                break;
            default:
                break;
            }
        }

        // 鼠标进入控件
        void HandleMouseEnter(const ImVec2& mousePos, ImMouseEvent* mouseEvent)
        {
            m_IsMouseOver = true;
            UpdateHoveredBar(mousePos);
        }

        // 鼠标离开控件
        void HandleMouseLeave(const ImVec2& mousePos, ImMouseEvent* mouseEvent)
        {
            m_IsMouseOver = false;
            m_HoveredBarIndex = -1;
        }

        // 鼠标移动
        void HandleMouseMove(const ImVec2& mousePos, ImMouseEvent* mouseEvent)
        {
            if (m_IsMouseOver)
            {
                UpdateHoveredBar(mousePos);
            }
        }

        // 更新悬停的分隔条索引
        void UpdateHoveredBar(const ImVec2& mousePos)
        {
            int newHoveredIndex = -1;

            // 检查鼠标位置是否在某个分隔条上
            for (int i = 0; i < m_BarRects.size(); i++)
            {
                if (m_BarRects[i].Contains(mousePos))
                {
                    newHoveredIndex = i;
                    break;
                }
            }

            // 如果悬停状态发生变化
            if (newHoveredIndex != m_HoveredBarIndex)
            {
                // 发送离开事件给之前悬停的分隔条
                if (m_HoveredBarIndex >= 0)
                {
                    // 可以在这里处理分隔条悬停离开的视觉反馈
                }

                // 更新悬停索引
                m_HoveredBarIndex = newHoveredIndex;

                // 发送进入事件给新悬停的分隔条
                if (m_HoveredBarIndex >= 0)
                {
                    // 可以在这里处理分隔条悬停进入的视觉反馈
                }
            }
        }

        // 专门处理拖拽事件
        void HandleDragEvent(ImDragEvent* dragEvent)
        {
            const ImVec2 mousePos = dragEvent->GetPosition();

            switch (dragEvent->GetType())
            {
            case ImEventType::DragEnter:
                HandleDragEnter(mousePos, dragEvent);
                break;
            case ImEventType::DragOver:
                HandleDragOver(mousePos, dragEvent);
                break;
            case ImEventType::DragLeave:
                HandleDragLeave(mousePos, dragEvent);
                break;
            case ImEventType::Drop:
                HandleDrop(mousePos, dragEvent);
                break;
            case ImEventType::DragStart:
                HandleDragStart(mousePos, dragEvent);
                break;
            case ImEventType::DragUpdate:
                HandleDragUpdate(mousePos, dragEvent);
                break;
            case ImEventType::DragEnd:
                HandleDragEnd(mousePos, dragEvent);
                break;
            default:
                break;
            }
        }

        // 拖拽进入分隔条区域
        void HandleDragEnter(const ImVec2& mousePos, ImDragEvent* dragEvent)
        {
            // 检查拖拽源是否来自本控件内部的分隔条
            if (IsDragFromOwnBars(dragEvent))
            {
                // 设置拖拽光标样式
                dragEvent->StopPropagation();
            }
        }

        // 拖拽在分隔条上方移动
        void HandleDragOver(const ImVec2& mousePos, ImDragEvent* dragEvent)
        {
            // 只有来自本控件内部的分隔条拖拽才处理
            if (IsDragFromOwnBars(dragEvent))
            {
                dragEvent->StopPropagation();
            }
        }

        // 拖拽离开分隔条区域
        void HandleDragLeave(const ImVec2& mousePos, ImDragEvent* dragEvent)
        {
            if (IsDragFromOwnBars(dragEvent))
            {
                dragEvent->StopPropagation();
            }
        }

        // 拖拽放下（在分隔条上释放）
        void HandleDrop(const ImVec2& mousePos, ImDragEvent* dragEvent)
        {
            // 分隔条不接受外部拖拽放下，只处理内部拖拽
            if (IsDragFromOwnBars(dragEvent))
            {
                dragEvent->StopPropagation();
            }
        }

        // 拖拽开始（检查是否在分隔条上开始拖拽）
        void HandleDragStart(const ImVec2& mousePos, ImDragEvent* dragEvent)
        {
            // 检查鼠标位置是否在某个分隔条上
            for (int i = 0; i < m_BarRects.size(); i++)
            {
                if (m_BarRects[i].Contains(mousePos))
                {
                    m_DraggingIndex = i;
                    m_DragStartPos = mousePos.y;  // 垂直方向使用y坐标
                    m_DragStartSplitterPos = m_BarRects[i].GetCenter().y;
                    m_IsDragSource = true;
                    m_DragStartPosition = mousePos;

                    dragEvent->StopPropagation();
                    break;
                }
            }
        }

        // 拖拽更新
        void HandleDragUpdate(const ImVec2& mousePos, ImDragEvent* dragEvent)
        {
            // 只有当前控件是拖拽源时才处理
            if (m_IsDragSource && m_DraggingIndex >= 0)
            {
                float newSplitterPos = m_DragStartSplitterPos + (mousePos.y - m_DragStartPos);  // 垂直方向使用y坐标
                UpdateSplitterPosition(m_DraggingIndex, newSplitterPos);

                dragEvent->StopPropagation();
            }
        }

        // 拖拽结束
        void HandleDragEnd(const ImVec2& mousePos, ImDragEvent* dragEvent)
        {
            if (m_IsDragSource)
            {
                m_DraggingIndex = -1;
                m_IsDragSource = false;
                dragEvent->StopPropagation();
            }
        }

        // 检查拖拽是否来自本控件的分隔条
        bool IsDragFromOwnBars(ImDragEvent* dragEvent)
        {
            // 通过拖拽数据类型或拖拽源来判断
            return false;
        }

        // 更新分隔条位置
        void UpdateSplitterPosition(int barIndex, float newSplitterPos)
        {
            int topIndex = barIndex;
            int bottomIndex = barIndex + 1;

            if (auto* topSlot = dynamic_cast<ImVerticalSplitterSlot*>(GetSlotAt(topIndex)))
            {
                if (auto* bottomSlot = dynamic_cast<ImVerticalSplitterSlot*>(GetSlotAt(bottomIndex)))
                {
                    // 计算新高度
                    float newTopHeight = newSplitterPos - m_PartRects[topIndex].Min.y - m_Style.BarHeight * 0.5f;
                    float newBottomHeight = m_PartRects[bottomIndex].Max.y - newSplitterPos - m_Style.BarHeight * 0.5f;

                    // 应用最小尺寸约束
                    if (newTopHeight < topSlot->MinSize)
                    {
                        newTopHeight = topSlot->MinSize;
                        newBottomHeight = (m_PartRects[topIndex].GetHeight() +
                            m_PartRects[bottomIndex].GetHeight()) - newTopHeight;
                    }
                    if (newBottomHeight < bottomSlot->MinSize)
                    {
                        newBottomHeight = bottomSlot->MinSize;
                        newTopHeight = (m_PartRects[topIndex].GetHeight() +
                            m_PartRects[bottomIndex].GetHeight()) - newBottomHeight;
                    }

                    // 更新比例
                    float totalRatio = topSlot->Ratio + bottomSlot->Ratio;
                    topSlot->Ratio = totalRatio * newTopHeight / (newTopHeight + newBottomHeight);
                    bottomSlot->Ratio = totalRatio - topSlot->Ratio;

                    MarkLayoutDirty();
                }
            }
        }

        // 绘制分隔条（更新鼠标光标设置）
        void RenderSplitterBar(const ImRect& barRect, const ImVec2& MousePos, int barIndex)
        {
            ImGuiWindow* window = ImGui::GetCurrentWindow();
            ImU32 color;

            if (m_IsDragSource && m_DraggingIndex == barIndex)
            {
                color = m_Style.ActiveColor;
                // 设置拖拽光标
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
            }
            else if (barIndex == m_HoveredBarIndex)
            {
                color = m_Style.HoveredColor;
                // 设置悬停光标
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
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
            for (int i = 0; i < numParts; i++)
            {
                if (auto* slot = dynamic_cast<ImVerticalSplitterSlot*>(GetSlotAt(i)))
                {
                    totalMinHeight += slot->MinSize;
                }
            }

            // 如果可用空间小于最小总高度，按比例分配
            if (availableHeight < totalMinHeight)
            {
                for (int i = 0; i < numParts; i++)
                {
                    if (auto* slot = dynamic_cast<ImVerticalSplitterSlot*>(GetSlotAt(i)))
                    {
                        heights[i] = (slot->MinSize / totalMinHeight) * availableHeight;
                    }
                }
                return;
            }

            // 应用最小尺寸约束
            bool needsAdjustment;
            do
            {
                needsAdjustment = false;
                float remainingHeight = availableHeight;
                float totalDynamicRatio = 0.0f;

                // 设置所有部件的最小尺寸
                for (int i = 0; i < numParts; i++)
                {
                    if (auto* slot = dynamic_cast<ImVerticalSplitterSlot*>(GetSlotAt(i)))
                    {
                        if (heights[i] < slot->MinSize)
                        {
                            heights[i] = slot->MinSize;
                            needsAdjustment = true;
                        }
                        remainingHeight -= heights[i];
                    }
                }

                // 如果空间不足，需要重新分配
                if (remainingHeight < 0)
                {
                    // 计算动态部分的总比例
                    totalDynamicRatio = 0;
                    for (int i = 0; i < numParts; i++)
                    {
                        if (auto* slot = dynamic_cast<ImVerticalSplitterSlot*>(GetSlotAt(i)))
                        {
                            if (heights[i] > slot->MinSize)
                            {
                                totalDynamicRatio += slot->Ratio;
                            }
                        }
                    }

                    // 按比例调整动态部分
                    if (totalDynamicRatio > 0)
                    {
                        for (int i = 0; i < numParts; i++)
                        {
                            if (auto* slot = dynamic_cast<ImVerticalSplitterSlot*>(GetSlotAt(i)))
                            {
                                if (heights[i] > slot->MinSize)
                                {
                                    float reduction = (remainingHeight * slot->Ratio / totalDynamicRatio);
                                    heights[i] += reduction;
                                    if (heights[i] < slot->MinSize)
                                    {
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
            for (int i = 0; i < numParts; i++)
            {
                if (auto* slot = dynamic_cast<ImVerticalSplitterSlot*>(GetSlotAt(i)))
                {
                    totalRatio += slot->Ratio;
                }
            }

            // 计算初始高度
            heights.clear();
            heights.resize(numParts, 0.0f);
            for (int i = 0; i < numParts; i++)
            {
                if (auto* slot = dynamic_cast<ImVerticalSplitterSlot*>(GetSlotAt(i)))
                {
                    heights[i] = (slot->Ratio / totalRatio) * contentHeight;
                }
            }

            // 应用最小尺寸约束
            ApplyMinSizeConstraints(heights, contentHeight);
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
            for (int i = 0; i < numParts; i++)
            {
                // 部件区域
                m_PartRects.emplace_back(
                    Position.x,
                    cursorY,
                    Position.x + Size.x,
                    cursorY + heights[i]
                );
                cursorY += heights[i];

                // 如果不是最后一部分，添加分隔条
                if (i < numBars)
                {
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
            for (int i = 0; i < numParts; i++)
            {
                if (ImSlot* slot = GetSlotAt(i))
                {
                    if (slot->IsValid())
                    {
                        // 只需设置slot的位置和大小
                        slot->SetSlotPosition(m_PartRects[i].Min);
                        slot->SetSlotSize(m_PartRects[i].GetSize());
                        slot->ApplyLayout();
                    }
                }
            }
        }

    public:
        void SetSplitterStyle(const ImVerticalSplitterStyle& style)
        {
            m_Style = style;
        }

        ImVerticalSplitterSlot* AddPart(ImWidget* widget, float ratio = 1.0f)
        {
            auto* slot = AddChildInternal<ImVerticalSplitterSlot>(widget);
            slot->Ratio = ratio;
            return slot;
        }

        void SetMinSize(int index, float minSize)
        {
            if (ImSlot* slot = GetSlotAt(index))
            {
                if (auto* splitterSlot = dynamic_cast<ImVerticalSplitterSlot*>(slot))
                {
                    splitterSlot->MinSize = minSize;
                    MarkLayoutDirty();  // 标记需要重新布局
                }
            }
        }

        virtual ImSlot* AddChild(ImWidget* Child, ImVec2 RelativePosition = ImVec2(FLT_MIN, FLT_MIN))
        {
            // 如果没有指定相对位置，添加到末尾
            if (RelativePosition.y == FLT_MIN)
            {
                return AddPart(Child, 1.0f); // 使用AddPart添加并设置默认比例
            }

            // 计算在分割器中的相对位置
            float relativeY = RelativePosition.y;

            // 如果没有子项，直接添加到末尾
            if (GetSlotNum() == 0)
            {
                return AddPart(Child, 1.0f);
            }

            // 确保布局是最新的
            HandleLayout();

            // 遍历所有部分区域，寻找插入位置
            int insertIndex = GetSlotNum(); // 默认插入到最后
            float accumulatedHeight = 0.0f;

            for (int i = 0; i < GetSlotNum(); i++)
            {
                const ImRect& partRect = m_PartRects[i];
                float partHeight = partRect.GetHeight();

                // 如果位置在当前部分中点之前，插入在当前位置
                if (relativeY <= accumulatedHeight + partHeight * 0.5f)
                {
                    insertIndex = i;
                    break;
                }

                accumulatedHeight += partHeight;

                // 如果不是最后一部分，加上分隔条高度
                if (i < static_cast<int>(m_BarRects.size()))
                {
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

            // 只处理布局，不处理交互
            HandleLayout();

            // 渲染背景和子控件
            RenderBackGround();
            RenderChild();

            // 渲染所有分隔条（使用当前鼠标位置获取悬停状态）
            const ImVec2 mousePos = ImGui::GetIO().MousePos;
            for (int i = 0; i < m_BarRects.size(); i++)
            {
                RenderSplitterBar(m_BarRects[i], mousePos, i);
            }

            // 如果没有分隔条被悬停或拖拽，恢复默认光标
            if (m_HoveredBarIndex == -1 && !m_IsDragSource)
            {
                // 可以在这里恢复默认光标，如果需要的话
            }
        }

        // 修改HitTest，确保分隔条能够接收拖拽事件
        virtual ImWidget* ChildHitTest(ImVec2 Pos) override
        {
            // 首先检查是否在分隔条上
            for (int i = 0; i < m_BarRects.size(); i++)
            {
                if (m_BarRects[i].Contains(Pos))
                {
                    return this; // 分隔条事件由分割器本身处理
                }
            }

            // 如果不是分隔条，调用基类的HitTest
            return ImPanelWidget::ChildHitTest(Pos);
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