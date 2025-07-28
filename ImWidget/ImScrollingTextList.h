#pragma once
#include "ImWidget.h"
#include <vector>
#include <string>
#include <algorithm>

namespace ImGuiWidget
{
    class ImScrollingTextList : public ImWidget
    {
    private:
        struct TextLine {
            std::string Text;
            ImVec2 Position;
            ImVec2 Size;
        };

        struct TextItem {
            std::string Original;
            std::vector<TextLine> Lines;
        };

        std::vector<TextItem> m_Items;
        float m_ScrollOffsetY = 0.0f;
        float m_TotalContentHeight = 0.0f;
        ImVec2 m_ContentRegion = { 0, 0 };
        bool m_RequireRebuild = true;
        ImU32 m_TextColor = IM_COL32(0, 0, 0, 255);
        bool m_IsDraggingScrollbar = false;
        float m_ScrollbarGrabDelta = 0.0f;

        // 使用ImFont底层API计算文本尺寸
        ImVec2 CalculateTextSize(const char* text_begin, const char* text_end = nullptr)
        {
            ImFont* font = ImGui::GetFont();
            float font_size = ImGui::GetFontSize();
            return font->CalcTextSizeA(font_size, FLT_MAX, -1.0f, text_begin, text_end);
        }

        // 重建布局（自动换行）
        void RebuildLayout()
        {
            m_TotalContentHeight = 0.0f;
            const float line_spacing = ImGui::GetTextLineHeightWithSpacing();
            const float available_width = m_ContentRegion.x;

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

    public:
        ImScrollingTextList(const std::string& WidgetName)
            : ImWidget(WidgetName)
        {
        }

        void SetTextColor(ImU32 color) {
            m_TextColor = color;
        }

        void AddItem(const std::string& text)
        {
            m_Items.push_back({ text, {} });
            m_RequireRebuild = true;
        }

        void Clear()
        {
            m_Items.clear();
            m_ScrollOffsetY = 0.0f;
            m_RequireRebuild = true;
        }

        void ModifyItem(size_t index, const std::string& newText)
        {
            if (index < m_Items.size())
            {
                m_Items[index].Original = newText;
                m_Items[index].Lines.clear();
                m_RequireRebuild = true;
            }
        }

        void RemoveItem(size_t index)
        {
            if (index < m_Items.size())
            {
                m_Items.erase(m_Items.begin() + index);
                m_RequireRebuild = true;
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

            // 开始裁剪区域
            window->DrawList->PushClipRect(widget_rect.Min, widget_rect.Max, true);

            // 处理滚轮事件
            ImGuiIO& io = ImGui::GetIO();
            bool is_hovered = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(widget_rect.Min, widget_rect.Max);

            if (is_hovered)
            {
                m_ScrollOffsetY -= io.MouseWheel * ImGui::GetTextLineHeight() * 3;

                // 修复：使用正确的高度差进行滚动限制
                float max_scroll = ImMax(0.0f, m_TotalContentHeight - m_ContentRegion.y);
                m_ScrollOffsetY = ImClamp(m_ScrollOffsetY, 0.0f, max_scroll);
            }

            // 计算可见区域
            const float visible_y1 = m_ScrollOffsetY;
            const float visible_y2 = visible_y1 + Size.y;

            // 绘制背景（可选）
            window->DrawList->AddRectFilled(
                widget_rect.Min, widget_rect.Max,
                ImGui::GetColorU32(ImGuiCol_ChildBg),
                ImGui::GetStyle().ChildRounding
            );

            // 渲染可见行
            for (const auto& item : m_Items)
            {
                for (const auto& line : item.Lines)
                {
                    const float line_y1 = line.Position.y;
                    const float line_y2 = line_y1 + line.Size.y;

                    if (line_y2 >= visible_y1 && line_y1 <= visible_y2)
                    {
                        ImVec2 screen_pos = widget_pos + ImVec2(0, line.Position.y - m_ScrollOffsetY);
                        window->DrawList->AddText(screen_pos, m_TextColor, line.Text.c_str());
                    }
                }
            }

            // 滚动条处理
            float max_scroll = ImMax(0.0f, m_TotalContentHeight - m_ContentRegion.y);

            if (max_scroll > 0.0f)
            {
                const float scrollbar_width = ImGui::GetStyle().ScrollbarSize;
                const ImRect scrollbar_rect(
                    widget_pos.x + Size.x - scrollbar_width,
                    widget_pos.y,
                    widget_pos.x + Size.x,
                    widget_pos.y + Size.y
                );

                // 计算滚动条滑块高度（至少20像素）
                const float scroll_thumb_height = ImMax(20.0f, (m_ContentRegion.y / m_TotalContentHeight) * Size.y);

                // 修复：使用正确的高度差计算滚动比例
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

                // 绘制滚动条背景
                window->DrawList->AddRectFilled(
                    scrollbar_rect.Min, scrollbar_rect.Max,
                    ImGui::GetColorU32(ImGuiCol_ScrollbarBg),
                    g.Style.ScrollbarRounding
                );

                // 绘制滑块
                window->DrawList->AddRectFilled(
                    thumb_rect.Min, thumb_rect.Max,
                    ImGui::GetColorU32(ImGuiCol_ScrollbarGrab),
                    g.Style.ScrollbarRounding
                );

                // 处理滚动条拖动
                ImGuiID scrollbar_id = window->GetID(m_WidgetName.c_str());
                bool hovered, held;
                ImGui::ButtonBehavior(scrollbar_rect, scrollbar_id, &hovered, &held,
                    ImGuiButtonFlags_AllowOverlap | ImGuiButtonFlags_PressedOnClick);

                // 处理拖动开始
                if (ImGui::IsMouseClicked(0) && hovered)
                {
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