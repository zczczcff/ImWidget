#pragma once
#include "ImWidget.h"
#include "ImEvent/ImMouseEvent.h"
#include "ImEvent/ImKeyEvent.h"
#include "ImEvent/ImDragEvent.h"
#include <vector>
#include <string>
#include <algorithm>
#include <set>

namespace ImGuiWidget
{
    class ImScrollingTextList : public ImWidget
    {
    private:
        struct TextLine
        {
            std::string Text;
            ImVec2 Position;
            ImVec2 Size;
            std::vector<float> CharOffsets;
            bool IsVisible = false; // 新增：可见性标记
        };

        struct TextItem
        {
            std::string Original;
            std::vector<TextLine> Lines;
            ImU32 Color = IM_COL32(0, 0, 0, 255);
            bool NeedsRebuild = true; // 新增：标记是否需要重建
        };

        std::vector<TextItem> m_Items;
        float m_ScrollOffsetY = 0.0f;
        float m_TotalContentHeight = 0.0f;
        ImVec2 m_ContentRegion = { 0, 0 };
        bool m_RequireRebuild = true;
        ImU32 m_TextColor = IM_COL32(0, 0, 0, 255);

        // 新增：可见性缓存
        struct VisibleRange
        {
            int StartItem = -1;
            int StartLine = -1;
            int EndItem = -1;
            int EndLine = -1;
            float CachedScrollOffsetY = -1.0f; // 缓存的滚动位置

            bool IsValid() const { return StartItem >= 0 && EndItem >= 0; }
            void Reset() { StartItem = EndItem = -1; CachedScrollOffsetY = -1.0f; }
        };

        VisibleRange m_VisibleRange;
        float m_LastScrollOffsetY = -1.0f; // 上次的滚动位置

        // 滚动条相关（保持不变）
        float m_ScrollbarThickness = 16.0f;
        ImU32 m_ScrollbarBgColor = IM_COL32(225, 225, 225, 255);
        ImU32 m_ScrollbarGrabColor = IM_COL32(100, 100, 100, 255);
        ImU32 m_ScrollbarGrabHoveredColor = IM_COL32(120, 120, 120, 255);
        ImU32 m_ScrollbarGrabActiveColor = IM_COL32(150, 150, 150, 255);

        // 滚动条交互状态（保持不变）
        bool m_IsDraggingScrollbar = false;
        int m_DraggingScrollbar = 0;
        ImVec2 m_DragStartPos;
        float m_ScrollbarGrabDelta = 0.0f;
        int m_HoveredScrollbar = 0;
        ImRect m_VerticalScrollbarRect;
        ImRect m_VerticalGrabRect;

        // 选中状态（保持不变）
        struct SelectionPoint
        {
            int ItemIndex = -1;
            int LineIndex = -1;
            int CharIndex = -1;

            bool operator==(const SelectionPoint& other) const
            {
                return ItemIndex == other.ItemIndex &&
                    LineIndex == other.LineIndex &&
                    CharIndex == other.CharIndex;
            }

            bool isValid() const { return ItemIndex >= 0 && LineIndex >= 0 && CharIndex >= 0; }
        };

        SelectionPoint m_SelectionStart;
        SelectionPoint m_SelectionEnd;
        bool m_IsSelecting = false;
        bool m_HasSelection = false;

        // 自动滚动（保持不变）
        float m_AutoScrollSpeed = 0.0f;
        float m_AutoScrollAccumulator = 0.0f;

        // 行间距（保持不变）
        float m_LineSpacing = 5.0f;

        // 事件处理状态（保持不变）
        bool m_IsMouseOver = false;

    public:
        ImScrollingTextList(const std::string& WidgetName)
            : ImWidget(WidgetName)
        {
            SetFocusable(true);
        }

        // 文本管理方法 - 添加标记需要重建
        void AddItem(const std::string& text, ImU32 color = IM_COL32(0, 0, 0, 255))
        {
            m_Items.push_back({ text, {}, color, true });
            m_RequireRebuild = true;
            m_VisibleRange.Reset(); // 重置可见范围
        }

        void Clear()
        {
            m_Items.clear();
            m_ScrollOffsetY = 0.0f;
            m_RequireRebuild = true;
            m_HasSelection = false;
            m_IsSelecting = false;
            m_VisibleRange.Reset();
        }

        void ModifyItem(size_t index, const std::string& newText, ImU32 newColor = IM_COL32(0, 0, 0, 255))
        {
            if (index < m_Items.size())
            {
                m_Items[index].Original = newText;
                m_Items[index].Color = newColor;
                m_Items[index].NeedsRebuild = true;
                m_RequireRebuild = true;
                m_HasSelection = false;
                m_VisibleRange.Reset();
            }
        }

        void RemoveItem(size_t index)
        {
            if (index < m_Items.size())
            {
                m_Items.erase(m_Items.begin() + index);
                m_RequireRebuild = true;
                m_HasSelection = false;
                m_VisibleRange.Reset();
            }
        }

        // 样式设置（保持不变）
        void SetTextColor(ImU32 color) { m_TextColor = color; }

        void SetItemColor(size_t index, ImU32 color)
        {
            if (index < m_Items.size())
            {
                m_Items[index].Color = color;
            }
        }

        void SetAllItemsColor(ImU32 color)
        {
            for (auto& item : m_Items)
            {
                item.Color = color;
            }
        }

        void SetLineSpacing(float spacing)
        {
            if (spacing != m_LineSpacing)
            {
                m_LineSpacing = spacing;
                m_RequireRebuild = true;
                m_VisibleRange.Reset();
            }
        }

        virtual void SetSize(ImVec2 size) override
        {
            if (size == Size) return;
            ImWidget::SetSize(size);
            m_ContentRegion = size - ImVec2(m_ScrollbarThickness, 0);
            m_RequireRebuild = true;
            m_VisibleRange.Reset();
        }

    protected:
        void CalculateVisibleRange()
        {
            // 如果滚动位置没有变化且可见范围有效，则直接返回
            if (m_LastScrollOffsetY == m_ScrollOffsetY && m_VisibleRange.IsValid())
                return;

            m_LastScrollOffsetY = m_ScrollOffsetY;
            m_VisibleRange.Reset();

            if (m_Items.empty()) return;

            const float visible_y1 = m_ScrollOffsetY;
            const float visible_y2 = visible_y1 + m_ContentRegion.y;
            const float line_spacing = (m_LineSpacing > 0) ? m_LineSpacing : ImGui::GetTextLineHeightWithSpacing();

            bool foundStart = false;

            // 遍历所有项和行，找到可见范围
            for (int itemIdx = 0; itemIdx < m_Items.size(); ++itemIdx)
            {
                auto& item = m_Items[itemIdx];

                // 如果该项需要重建，先重建
                if (item.NeedsRebuild)
                {
                    RebuildItem(item);
                }

                for (int lineIdx = 0; lineIdx < item.Lines.size(); ++lineIdx)
                {
                    auto& line = item.Lines[lineIdx];
                    float line_y1 = line.Position.y;
                    float line_y2 = line_y1 + line.Size.y + line_spacing;

                    // 检查是否可见
                    bool isVisible = (line_y2 >= visible_y1 && line_y1 <= visible_y2);
                    line.IsVisible = isVisible;

                    if (isVisible)
                    {
                        if (!foundStart)
                        {
                            m_VisibleRange.StartItem = itemIdx;
                            m_VisibleRange.StartLine = lineIdx;
                            foundStart = true;
                        }
                        m_VisibleRange.EndItem = itemIdx;
                        m_VisibleRange.EndLine = lineIdx;
                    }
                    else if (foundStart && line_y1 > visible_y2)
                    {
                        // 已经超过可见区域，提前结束
                        m_VisibleRange.CachedScrollOffsetY = m_ScrollOffsetY;
                        return;
                    }
                }
            }

            m_VisibleRange.CachedScrollOffsetY = m_ScrollOffsetY;
        }

        // 新增：重建单个文本项
        void RebuildItem(TextItem& item)
        {
            item.Lines.clear();
            const char* text_ptr = item.Original.c_str();
            const char* text_end = text_ptr + item.Original.size();
            const float available_width = m_ContentRegion.x - ImGui::GetStyle().FramePadding.x * 2;
            const float line_spacing = (m_LineSpacing > 0) ? m_LineSpacing : ImGui::GetTextLineHeightWithSpacing();

            while (text_ptr < text_end)
            {
                const char* line_end = text_ptr;
                while (line_end < text_end && *line_end != '\n' && *line_end != '\r')
                    ++line_end;

                const char* next_line = line_end;
                if (line_end < text_end && *line_end == '\r') ++next_line;
                if (line_end < text_end && *line_end == '\n') ++next_line;

                TextLine line;
                line.Text = std::string(text_ptr, line_end);
                line.Size = CalculateTextSize(line.Text.c_str());
                line.CharOffsets.clear();

                // 计算字符偏移
                if (!line.Text.empty())
                {
                    line.CharOffsets.push_back(0.0f);
                    for (size_t i = 0; i < line.Text.size(); ++i)
                    {
                        float charWidth = CalculateTextSize(line.Text.substr(0, i + 1).c_str()).x;
                        line.CharOffsets.push_back(charWidth);
                    }
                }

                // 自动换行处理
                if (line.Size.x > available_width)
                {
                    const char* wrap_ptr = text_ptr;
                    const char* last_good = text_ptr;

                    while (wrap_ptr < line_end)
                    {
                        if (*wrap_ptr == ' ' || *wrap_ptr == ',' || *wrap_ptr == '.')
                            last_good = wrap_ptr;

                        const std::string temp_str(text_ptr, wrap_ptr);
                        ImVec2 temp_size = CalculateTextSize(temp_str.c_str());

                        if (temp_size.x > available_width)
                        {
                            if (last_good > text_ptr)
                            {
                                line.Text = std::string(text_ptr, last_good);
                                line.Size = CalculateTextSize(line.Text.c_str());
                                wrap_ptr = last_good + 1;
                            }
                            else
                            {
                                line.Text = std::string(text_ptr, wrap_ptr);
                                line.Size = temp_size;
                            }
                            break;
                        }
                        ++wrap_ptr;
                    }
                    next_line = wrap_ptr;
                }

                line.Position = ImVec2(0.0f, m_TotalContentHeight);
                m_TotalContentHeight += line.Size.y + line_spacing;
                item.Lines.push_back(line);

                text_ptr = next_line;
            }

            item.NeedsRebuild = false;
        }

        // 修改后的布局重建方法
        void RebuildLayout()
        {
            m_TotalContentHeight = 0.0f;

            // 只重建需要重建的项
            for (auto& item : m_Items)
            {
                if (item.NeedsRebuild || m_RequireRebuild)
                {
                    RebuildItem(item);
                }
                else
                {
                    // 更新位置
                    for (auto& line : item.Lines)
                    {
                        line.Position.y = m_TotalContentHeight;
                        m_TotalContentHeight += line.Size.y + m_LineSpacing;
                    }
                }
            }

            m_RequireRebuild = false;
            ClampScrollPosition();
            m_VisibleRange.Reset(); // 重置可见范围
        }

        ImVec2 CalculateTextSize(const char* text_begin, const char* text_end = nullptr)
        {
            ImFont* font = ImGui::GetFont();
            float font_size = ImGui::GetFontSize();
            return font->CalcTextSizeA(font_size, FLT_MAX, -1.0f, text_begin, text_end);
        }

        void ClampScrollPosition()
        {
            float max_scroll = ImMax(0.0f, m_TotalContentHeight - m_ContentRegion.y);
            m_ScrollOffsetY = ImClamp(m_ScrollOffsetY, 0.0f, max_scroll);
        }

        // 选中相关方法
        SelectionPoint FindCharPosition(ImVec2 mousePosLocal) const
        {
            mousePosLocal.y += m_ScrollOffsetY;

            if (mousePosLocal.y < 0)
            {
                if (!m_Items.empty() && !m_Items[0].Lines.empty())
                {
                    return { 0, 0, 0 };
                }
                return { -1, -1, -1 };
            }

            const float totalHeight = m_TotalContentHeight;
            if (mousePosLocal.y > totalHeight)
            {
                if (!m_Items.empty())
                {
                    const auto& lastItem = m_Items.back();
                    if (!lastItem.Lines.empty())
                    {
                        const auto& lastLine = lastItem.Lines.back();
                        return { static_cast<int>(m_Items.size() - 1), static_cast<int>(lastItem.Lines.size() - 1), static_cast<int>(lastLine.Text.size()) };
                    }
                }
                return { -1, -1, -1 };
            }

            const float line_spacing = (m_LineSpacing > 0) ? m_LineSpacing : ImGui::GetTextLineHeightWithSpacing();
            for (int itemIdx = 0; itemIdx < m_Items.size(); ++itemIdx)
            {
                const auto& item = m_Items[itemIdx];
                for (int lineIdx = 0; lineIdx < item.Lines.size(); ++lineIdx)
                {
                    const auto& line = item.Lines[lineIdx];
                    float lineTop = line.Position.y;
                    float lineBottom = lineTop + line.Size.y;
                    float nextLineTop = lineBottom + line_spacing;

                    if (mousePosLocal.y >= lineTop && mousePosLocal.y < nextLineTop)
                    {
                        if (mousePosLocal.y <= lineBottom)
                        {
                            float mouseX = mousePosLocal.x;
                            const auto& offsets = line.CharOffsets;

                            if (offsets.empty()) return { itemIdx, lineIdx, 0 };

                            auto it = std::upper_bound(offsets.begin(), offsets.end(), mouseX);
                            if (it != offsets.begin())
                            {
                                int charIdx = std::distance(offsets.begin(), it) - 1;
                                return { itemIdx, lineIdx, charIdx };
                            }
                            return { itemIdx, lineIdx, 0 };
                        }
                        else
                        {
                            return { itemIdx, lineIdx, static_cast<int>(line.Text.size()) };
                        }
                    }
                }
            }
            return { -1, -1, -1 };
        }

        void NormalizeSelection(SelectionPoint& start, SelectionPoint& end) const
        {
            if (!start.isValid() || !end.isValid()) return;

            bool swapNeeded = false;
            if (start.ItemIndex > end.ItemIndex)
            {
                swapNeeded = true;
            }
            else if (start.ItemIndex == end.ItemIndex)
            {
                if (start.LineIndex > end.LineIndex)
                {
                    swapNeeded = true;
                }
                else if (start.LineIndex == end.LineIndex)
                {
                    if (start.CharIndex > end.CharIndex)
                    {
                        swapNeeded = true;
                    }
                }
            }
            if (swapNeeded) std::swap(start, end);
        }

        void CopySelectionToClipboard() const
        {
            if (!m_HasSelection) return;

            SelectionPoint start = m_SelectionStart;
            SelectionPoint end = m_SelectionEnd;
            NormalizeSelection(start, end);

            if (!start.isValid() || !end.isValid()) return;

            std::string selectedText;
            bool isFirstLine = true;

            for (int itemIdx = start.ItemIndex; itemIdx <= end.ItemIndex; ++itemIdx)
            {
                if (itemIdx < 0 || itemIdx >= m_Items.size()) continue;

                const auto& item = m_Items[itemIdx];
                int startLineIdx = (itemIdx == start.ItemIndex) ? start.LineIndex : 0;
                int endLineIdx = (itemIdx == end.ItemIndex) ? end.LineIndex : (item.Lines.size() - 1);

                for (int lineIdx = startLineIdx; lineIdx <= endLineIdx; ++lineIdx)
                {
                    if (lineIdx < 0 || lineIdx >= item.Lines.size()) continue;

                    const auto& line = item.Lines[lineIdx];
                    int startChar = (itemIdx == start.ItemIndex && lineIdx == start.LineIndex) ? start.CharIndex : 0;
                    int endChar = (itemIdx == end.ItemIndex && lineIdx == end.LineIndex) ? end.CharIndex : line.Text.size();

                    if (endChar > startChar)
                    {
                        if (!isFirstLine) selectedText += "\n";
                        selectedText += line.Text.substr(startChar, endChar - startChar);
                        isFirstLine = false;
                    }
                }
            }

            if (!selectedText.empty())
            {
                ImGui::SetClipboardText(selectedText.c_str());
            }
        }

        // 事件处理
        virtual void HandleEventInternal(ImEvent* event) override
        {
            if (event->IsHandled()) return;

            // 根据事件类型分发处理
            if (auto mouseEvent = event->As<ImMouseEvent>())
            {
                HandleMouseEvent(*mouseEvent);
            }
            else if (auto keyEvent = event->As<ImKeyEvent>())
            {
                HandleKeyEvent(*keyEvent);
            }
            else if (auto dragEvent = event->As<ImDragEvent>())
            {
                HandleDragEvent(*dragEvent);
            }
        }

        bool HandleMouseEvent(ImMouseEvent& event)
        {
            ImVec2 mousePos = event.GetPosition();
            ImRect widgetRect(Position, Position + Size);

            switch (event.GetType())
            {
            case ImEventType::MouseEnter:
                m_IsMouseOver = true;
                UpdateHoveredScrollbar(mousePos);
                return true;

            case ImEventType::MouseLeave:
                m_IsMouseOver = false;
                m_HoveredScrollbar = 0;
                return true;

            case ImEventType::MouseMove:
                if (m_IsMouseOver)
                {
                    UpdateHoveredScrollbar(mousePos);

                    // 处理文本选中拖动
                    if (m_IsSelecting)
                    {
                        HandleSelecting(mousePos);
                    }
                }
                return true;

            case ImEventType::MouseDown:
                if (event.GetButton() == ImMouseButton::Left)
                {
                    return HandleMouseDown(mousePos);
                }
                break;

            case ImEventType::MouseUp:
                if (event.GetButton() == ImMouseButton::Left)
                {
                    return HandleMouseUp();
                }
                break;

            case ImEventType::MouseWheel:
                if (m_IsMouseOver)
                {
                    ImVec2 wheelDelta = event.As<ImMouseWheelEvent>()->GetWheelDelta();
                    m_ScrollOffsetY -= wheelDelta.y * ImGui::GetTextLineHeight() * 3;
                    ClampScrollPosition();
                    return true;
                }
                break;

            default:
                break;
            }
            return false;
        }

        bool HandleKeyEvent(const ImKeyEvent& event)
        {
            if (!m_hasFocus) return false;

            if (event.GetType() == ImEventType::KeyDown)
            {
                if (event.IsCtrlPressed() && event.GetKeyCode() == ImGuiKey_C)
                {
                    CopySelectionToClipboard();
                    return true;
                }
            }
            return false;
        }

        bool HandleDragEvent(const ImDragEvent& event)
        {
            if (m_DraggingScrollbar != 0)
            {
                ImVec2 mousePos = event.GetPosition();

                if (m_DraggingScrollbar == 1)
                { // 垂直滚动条
                    float mouse_rel_y = mousePos.y - m_VerticalScrollbarRect.Min.y;
                    float new_scroll_ratio = (mouse_rel_y - m_ScrollbarGrabDelta) /
                        (m_VerticalScrollbarRect.GetHeight() - m_VerticalGrabRect.GetHeight());
                    new_scroll_ratio = ImClamp(new_scroll_ratio, 0.0f, 1.0f);

                    float max_scroll = ImMax(0.0f, m_TotalContentHeight - m_ContentRegion.y);
                    m_ScrollOffsetY = new_scroll_ratio * max_scroll;
                    ClampScrollPosition();
                }
                return true;
            }
            return false;
        }

        bool HandleMouseDown(const ImVec2& mousePos)
        {
            // 检查是否点击滚动条
            if (m_HoveredScrollbar == 1 && m_VerticalGrabRect.Contains(mousePos))
            {
                m_DraggingScrollbar = 1;
                m_DragStartPos = mousePos;
                m_ScrollbarGrabDelta = mousePos.y - m_VerticalGrabRect.Min.y;
                return true;
            }

            // 开始文本选中
            m_IsSelecting = true;
            m_HasSelection = true;

            ImVec2 mousePosLocal = mousePos - Position;
            m_SelectionStart = FindCharPosition(mousePosLocal);
            m_SelectionEnd = m_SelectionStart;

            RequestFocus();
            return true;
        }

        bool HandleMouseUp()
        {
            if (m_DraggingScrollbar != 0)
            {
                m_DraggingScrollbar = 0;
                return true;
            }

            if (m_IsSelecting)
            {
                m_IsSelecting = false;
                return true;
            }
            return false;
        }

        void HandleSelecting(const ImVec2& mousePos)
        {
            ImVec2 mousePosLocal = mousePos - Position;
            SelectionPoint newEnd = FindCharPosition(mousePosLocal);

            if (newEnd.isValid())
            {
                m_SelectionEnd = newEnd;
            }

            // 自动滚动处理
            const float edgeThreshold = 30.0f;
            float scrollDelta = 0.0f;
            float mouseYRelative = mousePosLocal.y;
            const float scrollSpeed = 0.5f;

            if (mouseYRelative < edgeThreshold)
            {
                scrollDelta = (mouseYRelative - edgeThreshold) * scrollSpeed;
            }
            else if (mouseYRelative > Size.y - edgeThreshold)
            {
                scrollDelta = (mouseYRelative - (Size.y - edgeThreshold)) * scrollSpeed;
            }

            if (scrollDelta != 0.0f)
            {
                m_ScrollOffsetY += scrollDelta;
                m_AutoScrollSpeed = scrollDelta;
            }
        }

        void UpdateHoveredScrollbar(const ImVec2& mousePos)
        {
            m_HoveredScrollbar = 0;
            if (m_VerticalScrollbarRect.Contains(mousePos) && m_VerticalGrabRect.Contains(mousePos))
            {
                m_HoveredScrollbar = 1;
            }
        }

        // 渲染方法
        virtual void Render() override
        {
            if (m_RequireRebuild)
                RebuildLayout();

            // 计算可见范围
            CalculateVisibleRange();

            ImDrawList* drawList = ImGui::GetWindowDrawList();
            ImRect widgetRect(Position, Position + Size);

            // 绘制背景
            drawList->AddRectFilled(widgetRect.Min, widgetRect.Max,
                ImGui::GetColorU32(ImGuiCol_ChildBg));

            // 计算可见区域
            const float visible_y1 = m_ScrollOffsetY;
            const float visible_y2 = visible_y1 + Size.y;

            ImGuiID id = ImGui::GetID(m_WidgetID.c_str());
            ImVec2 ContentSize(Size.x, Size.y - m_ScrollbarThickness);

            // 创建子窗口用于裁剪
            ImGui::SetNextWindowPos(Position);
            ImGui::BeginChild(id, ContentSize, ImGuiChildFlags_None,
                ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

            ImDrawList* drawListContent = ImGui::GetWindowDrawList();

            // 绘制选中背景（只绘制可见部分）
            if (m_HasSelection && m_SelectionStart.isValid() && m_SelectionEnd.isValid())
            {
                DrawSelectionBackground(drawListContent, visible_y1, visible_y2);
            }

            // 只渲染可见文本
            DrawVisibleTextContent(drawListContent, visible_y1, visible_y2);

            ImGui::EndChild();

            // 渲染滚动条
            DrawScrollbars(drawList);

            // 绘制边框
            drawList->AddRect(widgetRect.Min, widgetRect.Max,
                ImGui::GetColorU32(ImGuiCol_Border));
        }

        // 新增：只绘制可见文本内容
        void DrawVisibleTextContent(ImDrawList* drawList, float visible_y1, float visible_y2)
        {
            if (!m_VisibleRange.IsValid()) return;

            for (int itemIdx = m_VisibleRange.StartItem; itemIdx <= m_VisibleRange.EndItem; ++itemIdx)
            {
                if (itemIdx < 0 || itemIdx >= m_Items.size()) continue;

                const auto& item = m_Items[itemIdx];
                int startLine = (itemIdx == m_VisibleRange.StartItem) ? m_VisibleRange.StartLine : 0;
                int endLine = (itemIdx == m_VisibleRange.EndItem) ? m_VisibleRange.EndLine : (item.Lines.size() - 1);

                for (int lineIdx = startLine; lineIdx <= endLine; ++lineIdx)
                {
                    if (lineIdx < 0 || lineIdx >= item.Lines.size()) continue;

                    const auto& line = item.Lines[lineIdx];
                    if (!line.IsVisible) continue;

                    ImVec2 screen_pos = Position +
                        ImVec2(ImGui::GetStyle().FramePadding.x, line.Position.y - m_ScrollOffsetY);

                    drawList->AddText(screen_pos, item.Color, line.Text.c_str());
                }
            }
        }

        // 修改选中背景绘制，只处理可见部分
        void DrawSelectionBackground(ImDrawList* drawList, float visible_y1, float visible_y2)
        {
            SelectionPoint start = m_SelectionStart;
            SelectionPoint end = m_SelectionEnd;
            NormalizeSelection(start, end);

            if (!m_VisibleRange.IsValid()) return;

            // 限制在可见范围内处理
            int startItem = ImMax(start.ItemIndex, m_VisibleRange.StartItem);
            int endItem = ImMin(end.ItemIndex, m_VisibleRange.EndItem);

            for (int itemIdx = startItem; itemIdx <= endItem; ++itemIdx)
            {
                if (itemIdx < 0 || itemIdx >= m_Items.size()) continue;

                const auto& item = m_Items[itemIdx];
                int startLineIdx = (itemIdx == start.ItemIndex) ? start.LineIndex : 0;
                int endLineIdx = (itemIdx == end.ItemIndex) ? end.LineIndex : (item.Lines.size() - 1);

                // 限制在可见行范围内
                startLineIdx = ImMax(startLineIdx, (itemIdx == m_VisibleRange.StartItem) ? m_VisibleRange.StartLine : 0);
                endLineIdx = ImMin(endLineIdx, (itemIdx == m_VisibleRange.EndItem) ? m_VisibleRange.EndLine : ((int)item.Lines.size() - 1));

                for (int lineIdx = startLineIdx; lineIdx <= endLineIdx; ++lineIdx)
                {
                    if (lineIdx < 0 || lineIdx >= item.Lines.size()) continue;

                    const auto& line = item.Lines[lineIdx];
                    if (!line.IsVisible) continue;

                    ImVec2 screen_pos = Position + ImVec2(ImGui::GetStyle().FramePadding.x,
                        line.Position.y - m_ScrollOffsetY);

                    int startChar = (itemIdx == start.ItemIndex && lineIdx == start.LineIndex) ? start.CharIndex : 0;
                    int endChar = (itemIdx == end.ItemIndex && lineIdx == end.LineIndex) ? end.CharIndex : line.Text.size();

                    if (endChar > startChar)
                    {
                        float startX = screen_pos.x + (startChar > 0 ? line.CharOffsets[startChar] : 0);
                        float endX = screen_pos.x + (endChar < line.CharOffsets.size() ? line.CharOffsets[endChar] : line.Size.x);

                        drawList->AddRectFilled(
                            ImVec2(startX, screen_pos.y),
                            ImVec2(endX, screen_pos.y + line.Size.y),
                            ImGui::GetColorU32(ImGuiCol_TextSelectedBg)
                        );
                    }
                }
            }
        }

        void DrawScrollbars(ImDrawList* drawList)
        {
            float max_scroll = ImMax(0.0f, m_TotalContentHeight - m_ContentRegion.y);
            if (max_scroll <= 0.0f) return;

            // 垂直滚动条
            m_VerticalScrollbarRect = ImRect(
                ImVec2(Position.x + Size.x - m_ScrollbarThickness, Position.y),
                ImVec2(Position.x + Size.x, Position.y + Size.y)
            );

            float grabHeight = ImMax(m_ScrollbarThickness,
                (m_ContentRegion.y / m_TotalContentHeight) * Size.y);
            float scroll_ratio = m_ScrollOffsetY / max_scroll;
            scroll_ratio = ImClamp(scroll_ratio, 0.0f, 1.0f);

            float grabPosY = m_VerticalScrollbarRect.Min.y +
                (m_VerticalScrollbarRect.GetHeight() - grabHeight) * scroll_ratio;

            m_VerticalGrabRect = ImRect(
                ImVec2(m_VerticalScrollbarRect.Min.x, grabPosY),
                ImVec2(m_VerticalScrollbarRect.Max.x, grabPosY + grabHeight)
            );

            // 绘制滚动条背景
            drawList->AddRectFilled(m_VerticalScrollbarRect.Min, m_VerticalScrollbarRect.Max,
                m_ScrollbarBgColor);

            // 绘制滑块
            bool isHovered = (m_HoveredScrollbar == 1);
            bool isActive = (m_DraggingScrollbar == 1);
            ImU32 grabColor = isActive ? m_ScrollbarGrabActiveColor :
                isHovered ? m_ScrollbarGrabHoveredColor :
                m_ScrollbarGrabColor;

            drawList->AddRectFilled(m_VerticalGrabRect.Min, m_VerticalGrabRect.Max, grabColor);
        }

    public:
        virtual std::unordered_set<PropertyInfo, PropertyInfo::Hasher> GetProperties() override
        {
            auto props = ImWidget::GetProperties();

            props.insert({
                "TextColor", PropertyType::Color, "Appearance",
                [this](void* v) { this->m_TextColor = *static_cast<ImU32*>(v); },
                [this]() -> void* { return &this->m_TextColor; }
                });

            props.insert({
                "LineSpacing", PropertyType::Float, "Layout",
                [this](void* v)
 {
this->m_LineSpacing = *static_cast<float*>(v);
this->m_RequireRebuild = true;
},
[this]() -> void* { return &this->m_LineSpacing; }
                });

            // 滚动条样式属性
            props.insert({
                "ScrollbarThickness", PropertyType::Float, "Scrollbar",
                [this](void* v) { this->m_ScrollbarThickness = *static_cast<float*>(v); },
                [this]() -> void* { return &this->m_ScrollbarThickness; }
                });

            return props;
        }

        virtual std::string GetRegisterTypeName()override { return "ImScrollingTextList"; }

        virtual ImWidget* CopyWidget()
        {
            return new ImScrollingTextList(*this);
        }
    };
}