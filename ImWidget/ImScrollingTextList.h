#pragma once
#include "ImWidget.h"
#include <vector>
#include <string>
#include <algorithm>
#include <set>

namespace ImGuiWidget
{
    class ImScrollingTextList : public ImWidget
    {
    private:

        // 新增：行间距控制
        float m_LineSpacing = -1.0f; // 负值表示使用默认值

        // 新增：滚动条样式
        struct ScrollbarStyle {
            ImU32 BackgroundColor = IM_COL32(200, 200, 200, 255);
            ImU32 GrabColor = IM_COL32(120, 120, 120, 255);
            float Width = ImGui::GetStyle().ScrollbarSize;
            float Rounding = 0.0f;
            float GrabMinHeight = 20.0f;
        } m_ScrollbarStyle;


        struct TextLine {
            std::string Text;
            ImVec2 Position;
            ImVec2 Size;
            std::vector<float> CharOffsets; // 每个字符的x偏移量
        };

        struct TextItem {
            std::string Original;
            std::vector<TextLine> Lines;
            ImU32 Color = IM_COL32(0, 0, 0, 255); // 默认黑色
        };

        std::vector<TextItem> m_Items;
        float m_ScrollOffsetY = 0.0f;
        float m_TotalContentHeight = 0.0f;
        ImVec2 m_ContentRegion = { 0, 0 };
        bool m_RequireRebuild = true;
        ImU32 m_TextColor = IM_COL32(0, 0, 0, 255);
        bool m_IsDraggingScrollbar = false;
        float m_ScrollbarGrabDelta = 0.0f;

        // 选中状态
        struct SelectionPoint {
            int ItemIndex = -1;
            int LineIndex = -1;
            int CharIndex = -1;
        };

        SelectionPoint m_SelectionStart;
        SelectionPoint m_SelectionEnd;
        bool m_IsSelecting = false;
        bool m_HasSelection = false;
        float m_AutoScrollSpeed = 0.0f;
        float m_AutoScrollAccumulator = 0.0f;



        // 设置滚动条样式
        void SetScrollbarStyle(
            ImU32 bgColor = IM_COL32(200, 200, 200, 255),
            ImU32 grabColor = IM_COL32(120, 120, 120, 255),
            float width = -1.0f,   // 负值表示使用默认值
            float rounding = 0.0f,
            float grabMinHeight = 20.0f)
        {
            m_ScrollbarStyle.BackgroundColor = bgColor;
            m_ScrollbarStyle.GrabColor = grabColor;
            if (width >= 0) m_ScrollbarStyle.Width = width;
            m_ScrollbarStyle.Rounding = rounding;
            m_ScrollbarStyle.GrabMinHeight = grabMinHeight;
        }

        // 使用ImFont底层API计算文本尺寸
        ImVec2 CalculateTextSize(const char* text_begin, const char* text_end = nullptr)
        {
            ImFont* font = ImGui::GetFont();
            float font_size = ImGui::GetFontSize();
            return font->CalcTextSizeA(font_size, FLT_MAX, -1.0f, text_begin, text_end);
        }

        // 重建布局（自动换行）并计算字符偏移
        void RebuildLayout()
        {
            m_TotalContentHeight = 0.0f;

            // 计算行间距：优先使用自定义值，否则使用默认值
            const float line_spacing = (m_LineSpacing > 0) ? m_LineSpacing :
                ImGui::GetTextLineHeightWithSpacing();

            const float available_width = m_ContentRegion.x - ImGui::GetStyle().FramePadding.x * 2;

            for (auto& item : m_Items)
            {
                item.Lines.clear();
                const char* text_ptr = item.Original.c_str();
                const char* text_end = text_ptr + item.Original.size();

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

                    // 计算每个字符的偏移量
                    if (!line.Text.empty()) {
                        line.CharOffsets.push_back(0.0f);
                        for (size_t i = 0; i < line.Text.size(); ++i) {
                            float charWidth = CalculateTextSize(line.Text.substr(0, i + 1).c_str()).x;
                            line.CharOffsets.push_back(charWidth);
                        }
                    }

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
            }
            m_RequireRebuild = false;

            // 修复：使用正确的高度差进行滚动限制
            float max_scroll = ImMax(0.0f, m_TotalContentHeight - m_ContentRegion.y);
            m_ScrollOffsetY = ImClamp(m_ScrollOffsetY, 0.0f, max_scroll);
        }

        SelectionPoint FindCharPosition(ImVec2 mousePosLocal) const
        {
            mousePosLocal.y += m_ScrollOffsetY; // 转换为内容空间坐标

            // 顶部边界处理：选择第一个字符
            if (mousePosLocal.y < 0) {
                if (!m_Items.empty() && !m_Items[0].Lines.empty()) {
                    return { 0, 0, 0 };
                }
                return { -1, -1, -1 };
            }

            // 底部边界处理：选择最后一个字符
            const float totalHeight = m_TotalContentHeight;
            if (mousePosLocal.y > totalHeight) {
                if (!m_Items.empty()) {
                    const auto& lastItem = m_Items.back();
                    if (!lastItem.Lines.empty()) {
                        const auto& lastLine = lastItem.Lines.back();
                        return {
                            static_cast<int>(m_Items.size() - 1),
                            static_cast<int>(lastItem.Lines.size() - 1),
                            static_cast<int>(lastLine.Text.size())
                        };
                    }
                }
                return { -1, -1, -1 };
            }

            float accumulatedHeight = 0.0f;
            const float line_spacing = ImGui::GetTextLineHeightWithSpacing();

            for (int itemIdx = 0; itemIdx < m_Items.size(); ++itemIdx) {
                const auto& item = m_Items[itemIdx];
                for (int lineIdx = 0; lineIdx < item.Lines.size(); ++lineIdx) {
                    const auto& line = item.Lines[lineIdx];
                    float lineTop = line.Position.y;
                    float lineBottom = lineTop + line.Size.y;
                    float nextLineTop = lineBottom + line_spacing;

                    // 核心修改：处理行间空白区域
                    // 当鼠标位于当前行和下一行之间时，视为当前行的末尾
                    if (mousePosLocal.y >= lineTop && mousePosLocal.y < nextLineTop) {
                        // 在文本行内
                        if (mousePosLocal.y <= lineBottom) {
                            float mouseX = mousePosLocal.x;
                            const auto& offsets = line.CharOffsets;

                            if (offsets.empty()) {
                                return { itemIdx, lineIdx, 0 };
                            }

                            // 找到第一个大于鼠标x位置的偏移量
                            auto it = std::upper_bound(offsets.begin(), offsets.end(), mouseX);
                            if (it != offsets.begin()) {
                                int charIdx = std::distance(offsets.begin(), it) - 1;
                                return { itemIdx, lineIdx, charIdx };
                            }

                            return { itemIdx, lineIdx, 0 };
                        }
                        // 在行间空白区域
                        else {
                            // 视为当前行的末尾
                            return { itemIdx, lineIdx, static_cast<int>(line.Text.size()) };
                        }
                    }
                    accumulatedHeight = nextLineTop;
                }
            }

            // 默认返回无效位置
            return { -1, -1, -1 };
        }

        // 规范化选中范围（确保start在end之前）
        void NormalizeSelection(SelectionPoint& start, SelectionPoint& end) const
        {
            if (!IsValidPoint(start) || !IsValidPoint(end)) return;

            // 比较顺序：先按项目索引，再按行索引，最后按字符索引
            bool swapNeeded = false;

            if (start.ItemIndex > end.ItemIndex) {
                swapNeeded = true;
            }
            else if (start.ItemIndex == end.ItemIndex) {
                if (start.LineIndex > end.LineIndex) {
                    swapNeeded = true;
                }
                else if (start.LineIndex == end.LineIndex) {
                    if (start.CharIndex > end.CharIndex) {
                        swapNeeded = true;
                    }
                }
            }

            if (swapNeeded) {
                std::swap(start, end);
            }
        }

        // 检查点是否有效（确保索引在合理范围内）
        bool IsValidPoint(const SelectionPoint& point) const
        {
            if (point.ItemIndex < 0 || point.ItemIndex >= m_Items.size())
                return false;

            const auto& item = m_Items[point.ItemIndex];
            if (point.LineIndex < 0 || point.LineIndex >= item.Lines.size())
                return false;

            const auto& line = item.Lines[point.LineIndex];
            if (point.CharIndex < 0 || point.CharIndex > static_cast<int>(line.Text.size()))
                return false;

            return true;
        }

        // 复制选中的文本到剪贴板
        void CopySelectionToClipboard() const
        {
            if (!m_HasSelection) return;

            SelectionPoint start = m_SelectionStart;
            SelectionPoint end = m_SelectionEnd;
            NormalizeSelection(start, end);

            if (!IsValidPoint(start) || !IsValidPoint(end)) return;

            std::string selectedText;
            bool isFirstLine = true;

            for (int itemIdx = start.ItemIndex; itemIdx <= end.ItemIndex; ++itemIdx) {
                if (itemIdx < 0 || itemIdx >= m_Items.size()) continue;

                const auto& item = m_Items[itemIdx];
                int startLineIdx = (itemIdx == start.ItemIndex) ? start.LineIndex : 0;
                int endLineIdx = (itemIdx == end.ItemIndex) ? end.LineIndex : (item.Lines.size() - 1);

                for (int lineIdx = startLineIdx; lineIdx <= endLineIdx; ++lineIdx) {
                    if (lineIdx < 0 || lineIdx >= item.Lines.size()) continue;

                    const auto& line = item.Lines[lineIdx];
                    int startChar = (itemIdx == start.ItemIndex && lineIdx == start.LineIndex) ? start.CharIndex : 0;
                    int endChar = (itemIdx == end.ItemIndex && lineIdx == end.LineIndex) ? end.CharIndex : line.Text.size();

                    if (endChar > startChar) {
                        if (!isFirstLine) {
                            selectedText += "\n";
                        }
                        selectedText += line.Text.substr(startChar, endChar - startChar);
                        isFirstLine = false;
                    }
                }
            }

            if (!selectedText.empty()) {
                ImGui::SetClipboardText(selectedText.c_str());
            }
        }

    public:
        ImScrollingTextList(const std::string& WidgetName)
            : ImWidget(WidgetName)
        {
        }

        void SetTextColor(ImU32 color) {
            m_TextColor = color;
        }

        // 设置单行文本颜色
        void SetItemColor(size_t index, ImU32 color)
        {
            if (index < m_Items.size()) {
                m_Items[index].Color = color;
            }
        }

        // 设置所有文本颜色
        void SetAllItemsColor(ImU32 color)
        {
            for (auto& item : m_Items) {
                item.Color = color;
            }
        }

        // 设置行间距（负值表示使用默认值）
        void SetLineSpacing(float spacing)
        {
            if (spacing != m_LineSpacing) {
                m_LineSpacing = spacing;
                m_RequireRebuild = true;
            }
        }

        void AddItem(const std::string& text, ImU32 color = IM_COL32(0, 0, 0, 255))
        {
            m_Items.push_back({ text, {}, color });
            m_RequireRebuild = true;
        }

        void Clear()
        {
            m_Items.clear();
            m_ScrollOffsetY = 0.0f;
            m_RequireRebuild = true;
            m_HasSelection = false;
            m_IsSelecting = false;
        }

        void ModifyItem(size_t index, const std::string& newText, ImU32 newColor = IM_COL32(0, 0, 0, 255))
        {
            if (index < m_Items.size())
            {
                m_Items[index].Original = newText;
                m_Items[index].Color = newColor;
                m_Items[index].Lines.clear();
                m_RequireRebuild = true;
                m_HasSelection = false;
            }
        }

        void RemoveItem(size_t index)
        {
            if (index < m_Items.size())
            {
                m_Items.erase(m_Items.begin() + index);
                m_RequireRebuild = true;
                m_HasSelection = false;
            }
        }

        virtual void SetSize(ImVec2 size) override
        {
            ImWidget::SetSize(size);
            m_ContentRegion = size - ImVec2(ImGui::GetStyle().ScrollbarSize, 0);
            m_RequireRebuild = true;
        }

        virtual void Render() override
        {
            if (m_RequireRebuild)
                RebuildLayout();

            ImGuiContext& g = *GImGui;
            ImGuiWindow* window = ImGui::GetCurrentWindow();
            if (window->SkipItems)
                return;

            // 计算控件在屏幕上的位置
            ImVec2 widget_pos = Position;
            ImRect widget_rect(widget_pos, widget_pos + Size);

            ImVec2 mousepos = ImGui::GetMousePos();
            ImRect WidgetRect(Position, Position + Size);
            // 开始裁剪区域
            window->DrawList->PushClipRect(widget_rect.Min, widget_rect.Max, true);

            // 新增：全局鼠标点击检查（用于取消选中）
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                // 检查点击是否发生在控件外
                if (!WidgetRect.Contains(mousepos))
                {
                    m_HasSelection = false;
                    m_IsSelecting = false;
                }
            }

            // 处理鼠标和键盘事件
            ImGuiIO& io = ImGui::GetIO();
            bool is_hovered = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(widget_rect.Min, widget_rect.Max);
            bool is_active = ImGui::IsWindowFocused() && is_hovered;

            // 修改：添加滚动条拖动检查，避免同时处理文本选中
            if (is_hovered && !m_IsDraggingScrollbar)  // 新增条件
            {
                // 鼠标滚轮滚动
                m_ScrollOffsetY -= io.MouseWheel * ImGui::GetTextLineHeight() * 3;

                // 开始选中（鼠标左键按下）
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                {
                    m_IsSelecting = true;
                    m_HasSelection = true;

                    // 计算鼠标位置对应的字符位置
                    ImVec2 mousePos = io.MousePos;
                    ImVec2 mousePosLocal = mousePos - widget_pos;
                    m_SelectionStart = FindCharPosition(mousePosLocal);
                    m_SelectionEnd = m_SelectionStart;
                }

                // 结束选中（鼠标左键释放）
                if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                    m_IsSelecting = false;
                }

                // 处理Ctrl+C复制
                if (is_active && io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_C)) {
                    CopySelectionToClipboard();
                }
            }

            // 处理选中拖动 - 核心修改部分
            if (!m_IsDraggingScrollbar &&m_IsSelecting && (ImGui::IsMouseDragging(ImGuiMouseButton_Left) || ImGui::IsMouseDown(ImGuiMouseButton_Left))) {
                ImVec2 mousePos = io.MousePos;
                ImVec2 mousePosLocal = mousePos - widget_pos;

                // 即使鼠标在控件外也持续更新选中结束点
                SelectionPoint newEnd = FindCharPosition(mousePosLocal);
                if (IsValidPoint(newEnd)) {
                    m_SelectionEnd = newEnd;
                }

                // 自动滚动处理 - 边界区域扩大
                const float edgeThreshold = 30.0f; // 边界触发区域
                float scrollDelta = 0.0f;
                float mouseYRelative = mousePosLocal.y;
                const float scrollSpeed = 0.5f; // 滚动速度系数

                if (mouseYRelative < edgeThreshold) {
                    // 鼠标在控件上方区域
                    scrollDelta = (mouseYRelative - edgeThreshold) * scrollSpeed;
                }
                else if (mouseYRelative > Size.y - edgeThreshold) {
                    // 鼠标在控件下方区域
                    scrollDelta = (mouseYRelative - (Size.y - edgeThreshold)) * scrollSpeed;
                }

                // 应用自动滚动
                if (scrollDelta != 0.0f) {
                    m_ScrollOffsetY += scrollDelta;
                    m_AutoScrollSpeed = scrollDelta;
                }
                else {
                    // 没有边界滚动时，保持当前滚动速度
                    if (m_AutoScrollSpeed != 0.0f) {
                        m_ScrollOffsetY += m_AutoScrollSpeed;
                        m_AutoScrollSpeed *= 0.95f; // 平滑减速

                        if (std::abs(m_AutoScrollSpeed) < 0.1f) {
                            m_AutoScrollSpeed = 0.0f;
                        }
                    }
                }
            }
            else {
                // 应用惯性滚动
                if (m_AutoScrollSpeed != 0.0f) {
                    m_ScrollOffsetY += m_AutoScrollSpeed;
                    m_AutoScrollSpeed *= 0.9f; // 减速

                    if (std::abs(m_AutoScrollSpeed) < 0.1f) {
                        m_AutoScrollSpeed = 0.0f;
                    }
                }
            }

            // 修复：使用正确的高度差进行滚动限制
            float max_scroll = ImMax(0.0f, m_TotalContentHeight - m_ContentRegion.y);
            m_ScrollOffsetY = ImClamp(m_ScrollOffsetY, 0.0f, max_scroll);

            // 计算可见区域
            const float visible_y1 = m_ScrollOffsetY;
            const float visible_y2 = visible_y1 + Size.y;

            // 绘制背景（可选）
            window->DrawList->AddRectFilled(
                widget_rect.Min, widget_rect.Max,
                ImGui::GetColorU32(ImGuiCol_ChildBg),
                ImGui::GetStyle().ChildRounding
            );

            // 绘制选中背景
            if (m_HasSelection && IsValidPoint(m_SelectionStart) && IsValidPoint(m_SelectionEnd)) {
                SelectionPoint start = m_SelectionStart;
                SelectionPoint end = m_SelectionEnd;
                NormalizeSelection(start, end);

                for (int itemIdx = start.ItemIndex; itemIdx <= end.ItemIndex; ++itemIdx) {
                    if (itemIdx < 0 || itemIdx >= m_Items.size()) continue;

                    const auto& item = m_Items[itemIdx];
                    int startLineIdx = (itemIdx == start.ItemIndex) ? start.LineIndex : 0;
                    int endLineIdx = (itemIdx == end.ItemIndex) ? end.LineIndex : (item.Lines.size() - 1);

                    for (int lineIdx = startLineIdx; lineIdx <= endLineIdx; ++lineIdx) {
                        if (lineIdx < 0 || lineIdx >= item.Lines.size()) continue;

                        const auto& line = item.Lines[lineIdx];
                        float line_y1 = line.Position.y;
                        float line_y2 = line_y1 + line.Size.y;

                        // 只绘制可见行
                        if (line_y2 >= visible_y1 && line_y1 <= visible_y2) {
                            ImVec2 screen_pos = widget_pos + ImVec2(0, line.Position.y - m_ScrollOffsetY);

                            // 确定当前行的选中范围
                            int startChar = (itemIdx == start.ItemIndex && lineIdx == start.LineIndex) ? start.CharIndex : 0;
                            int endChar = (itemIdx == end.ItemIndex && lineIdx == end.LineIndex) ? end.CharIndex : line.Text.size();

                            if (endChar > startChar) {
                                // 计算选中区域的起始和结束位置
                                float startX = screen_pos.x + (startChar > 0 ? line.CharOffsets[startChar] : 0);
                                float endX = screen_pos.x + (endChar < line.CharOffsets.size() ? line.CharOffsets[endChar] : line.Size.x);
                                float startY = screen_pos.y;
                                float endY = startY + line.Size.y;

                                // 绘制选中背景
                                window->DrawList->AddRectFilled(
                                    ImVec2(startX, startY),
                                    ImVec2(endX, endY),
                                    ImGui::GetColorU32(ImGuiCol_TextSelectedBg)
                                );
                            }
                        }
                    }
                }
            }

            // 渲染可见行 - 支持每行不同颜色
            for (const auto& item : m_Items)
            {
                for (const auto& line : item.Lines)
                {
                    const float line_y1 = line.Position.y;
                    const float line_y2 = line_y1 + line.Size.y;

                    if (line_y2 >= visible_y1 && line_y1 <= visible_y2)
                    {
                        ImVec2 screen_pos = widget_pos +
                            ImVec2(ImGui::GetStyle().FramePadding.x,
                                line.Position.y - m_ScrollOffsetY);

                        // 使用该项的颜色设置
                        window->DrawList->AddText(screen_pos, item.Color, line.Text.c_str());
                    }
                }
            }


            // 滚动条处理 - 使用自定义样式
            if (max_scroll > 0.0f)
            {
                const float scrollbar_width = m_ScrollbarStyle.Width;
                const ImRect scrollbar_rect(
                    widget_pos.x + Size.x - scrollbar_width,
                    widget_pos.y,
                    widget_pos.x + Size.x,
                    widget_pos.y + Size.y
                );

                // 计算滚动条滑块高度
                const float scroll_thumb_height = ImMax(
                    m_ScrollbarStyle.GrabMinHeight,
                    (m_ContentRegion.y / m_TotalContentHeight) * Size.y
                );

                float scroll_ratio = (m_ScrollOffsetY / max_scroll);
                scroll_ratio = ImClamp(scroll_ratio, 0.0f, 1.0f);

                // 计算滑块位置
                const float scroll_thumb_y = scrollbar_rect.Min.y +
                    (scrollbar_rect.GetHeight() - scroll_thumb_height) * scroll_ratio;

                ImRect thumb_rect(
                    scrollbar_rect.Min.x,
                    scroll_thumb_y,
                    scrollbar_rect.Max.x,
                    scroll_thumb_y + scroll_thumb_height
                );

                // 使用自定义颜色绘制滚动条背景
                window->DrawList->AddRectFilled(
                    scrollbar_rect.Min, scrollbar_rect.Max,
                    m_ScrollbarStyle.BackgroundColor,
                    m_ScrollbarStyle.Rounding
                );

                // 使用自定义颜色绘制滑块
                window->DrawList->AddRectFilled(
                    thumb_rect.Min, thumb_rect.Max,
                    m_ScrollbarStyle.GrabColor,
                    m_ScrollbarStyle.Rounding
                );

                // 处理滚动条拖动 - 新增：设置拖动标志
                ImGuiID scrollbar_id = window->GetID(m_WidgetName.c_str());
                bool hovered, held;
                ImGui::ButtonBehavior(scrollbar_rect, scrollbar_id, &hovered, &held,
                    ImGuiButtonFlags_AllowOverlap | ImGuiButtonFlags_PressedOnClick);

                // 处理拖动开始
                if (ImGui::IsMouseClicked(0) && hovered)
                {
                    m_IsDraggingScrollbar = true;  // 设置滚动条拖动标志
                    m_IsDraggingScrollbar = true;
                    if (ImGui::IsMouseHoveringRect(thumb_rect.Min, thumb_rect.Max))
                    {
                        // 计算鼠标在滑块内的相对位置
                        m_ScrollbarGrabDelta = io.MousePos.y - thumb_rect.Min.y;
                    }
                    else
                    {
                        // 点击轨道时直接跳转
                        m_ScrollbarGrabDelta = scroll_thumb_height * 0.5f;
                        float mouse_rel_y = io.MousePos.y - scrollbar_rect.Min.y;
                        float new_scroll_ratio = (mouse_rel_y - m_ScrollbarGrabDelta) /
                            (scrollbar_rect.GetHeight() - scroll_thumb_height);
                        new_scroll_ratio = ImClamp(new_scroll_ratio, 0.0f, 1.0f);

                        // 修复：使用正确的高度差计算新偏移
                        m_ScrollOffsetY = new_scroll_ratio * max_scroll;
                    }
                }

                // 处理拖动结束
                if (m_IsDraggingScrollbar && ImGui::IsMouseReleased(0))
                {
                    m_IsDraggingScrollbar = false;
                }

                // 处理拖动中
                if (m_IsDraggingScrollbar)
                {
                    float mouse_rel_y = io.MousePos.y - scrollbar_rect.Min.y;
                    float new_scroll_ratio = (mouse_rel_y - m_ScrollbarGrabDelta) /
                        (scrollbar_rect.GetHeight() - scroll_thumb_height);
                    new_scroll_ratio = ImClamp(new_scroll_ratio, 0.0f, 1.0f);

                    // 修复：使用正确的高度差计算新偏移
                    m_ScrollOffsetY = new_scroll_ratio * max_scroll;
                }
            }

            // 结束裁剪区域
            window->DrawList->PopClipRect();

            // 绘制边框（可选）
            window->DrawList->AddRect(
                widget_rect.Min, widget_rect.Max,
                ImGui::GetColorU32(ImGuiCol_Border),
                ImGui::GetStyle().ChildRounding
            );
        }
    };
}