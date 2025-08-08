#pragma once

#include "ImWidget.h"
#include <imgui_internal.h>
#include <vector>
#include <string>
#include <algorithm>
#include <imgui.h>
#include <set>
#include <map>
#include <functional>

namespace ImGuiWidget
{
    class ImMultiLineTextBlock : public ImWidget
    {
        //friend class ImScrollingTextList;
    public:
        ImMultiLineTextBlock(const std::string& WidgetName)
            : ImWidget(WidgetName)
        {
        }
        virtual ~ImMultiLineTextBlock() = default;

        void SetText(const std::string& Text)
        {
            if (m_Text != Text)
            {
                m_Text = Text;
                m_TextDirty = true;
            }
        }
        void SetTextColor(ImU32 Color)
        {
            m_TextColor = Color;
        }
        void SetBackgroundColor(ImU32 Color)
        {
            m_BackgroundColor = Color;
        }
        void SetSelectionColor(ImU32 Color)
        {
            m_SelectionColor = Color;
        }
        void SetLineSpacing(float Spacing)
        {
            m_LineSpacing = Spacing;
        }

        virtual ImVec2 GetMinSize() override
        {
            if (m_TextDirty) WrapText();

            float maxWidth = 0.0f;
            for (float width : m_LineWidths)
            {
                maxWidth = ImMax(maxWidth, width);
            }

            float height = 0.0f;
            if (!m_Lines.empty())
            {
                height = (m_Lines.size() * ImGui::GetTextLineHeight() * m_LineSpacing) -
                    (ImGui::GetTextLineHeight() * (m_LineSpacing - 1.0f));
            }

            return ImVec2(ImMax(maxWidth, 16.f), ImMax(height, 16.f));
        }
        virtual void Render() override
        {
            if (m_TextDirty) WrapText();
            if (m_Lines.empty()) return;

            ImDrawList* drawList = ImGui::GetWindowDrawList();

            // 绘制背景
            if (m_BackgroundColor != 0)
            {
                drawList->AddRectFilled(Position, Position + Size, m_BackgroundColor);
            }

            // 计算行位置
            m_LinePositions.clear();
            float y = Position.y;
            const float lineHeight = ImGui::GetTextLineHeight() * m_LineSpacing;

            for (const auto& line : m_Lines)
            {
                m_LinePositions.emplace_back(Position.x, y);
                y += lineHeight;
            }

            // 处理文本选择
            HandleSelection();

            // 绘制选择区域
            if (m_SelectionStart >= 0 && m_SelectionEnd >= 0 && m_SelectionStart != m_SelectionEnd)
            {
                int start = ImMin(m_SelectionStart, m_SelectionEnd);
                int end = ImMax(m_SelectionStart, m_SelectionEnd);

                int lineStartIndex = 0;
                for (int i = 0; i < m_Lines.size(); i++)
                {
                    int lineEndIndex = lineStartIndex + static_cast<int>(m_Lines[i].size());

                    // 检查当前行是否在选择范围内
                    if (end > lineStartIndex && start < lineEndIndex)
                    {
                        // 计算当前行的选择范围
                        int lineSelStart = ImMax(start, lineStartIndex) - lineStartIndex;
                        int lineSelEnd = ImMin(end, lineEndIndex) - lineStartIndex;

                        // 计算选择区域的坐标
                        ImVec2 linePos = m_LinePositions[i];
                        ImFont* font = ImGui::GetFont();

                        // 计算选择区域的起始和结束X坐标
                        std::string beforeSel = m_Lines[i].substr(0, lineSelStart);
                        float selStartX = font->CalcTextSizeA(
                            ImGui::GetFontSize(),
                            FLT_MAX,
                            0.0f,
                            beforeSel.c_str()
                        ).x;

                        std::string selText = m_Lines[i].substr(0, lineSelEnd);
                        float selEndX = font->CalcTextSizeA(
                            ImGui::GetFontSize(),
                            FLT_MAX,
                            0.0f,
                            selText.c_str()
                        ).x;

                        // 绘制选择区域
                        ImVec2 selStart(linePos.x + selStartX, linePos.y);
                        ImVec2 selEnd(linePos.x + selEndX, linePos.y + lineHeight);

                        drawList->AddRectFilled(selStart, selEnd, m_SelectionColor);
                    }

                    lineStartIndex = lineEndIndex;
                }
            }

            // 绘制文本
            for (int i = 0; i < m_Lines.size(); i++)
            {
                drawList->AddText(
                    m_LinePositions[i],
                    m_TextColor,
                    m_Lines[i].c_str()
                );
            }
        }
        virtual void SetPosition(ImVec2 Pos)
        {
            if (Pos == Position) return;
            Position = Pos;
            m_TextDirty = true;
        }
        virtual void SetSize(ImVec2 size)
        {
            if (size == Size) return;
            Size = size;
            m_TextDirty = true;
        }

        void SetSelection(int startindex, int endindex)
        {
            m_SelectionStart = startindex;
            m_SelectionEnd = endindex;
            m_IsSelecting = true;
        }

        void ClearSelection()
        {
            m_SelectionStart = -1;
            m_SelectionEnd = -1;
            m_IsSelecting = false;
        }
        // 清空所有选中的控件
        static void ClearAllSelections()
        {
            s_SelectedWidgets.clear();
        }

        // 复制所有选中控件的文本
        static void CopyAllSelectedTexts()
        {
            if (s_SelectedWidgets.empty())
                return;

            // 按位置排序：先上后下，先左后右
            std::vector<ImMultiLineTextBlock*> sortedWidgets;
            for (auto widget : s_SelectedWidgets)
                sortedWidgets.push_back(widget);

            std::sort(sortedWidgets.begin(), sortedWidgets.end(),
                [](ImMultiLineTextBlock* a, ImMultiLineTextBlock* b) {
                    if (a->GetPosition().y != b->GetPosition().y)
                        return a->GetPosition().y < b->GetPosition().y;
                    return a->GetPosition().x < b->GetPosition().x;
                });

            // 拼接所有选中文本
            std::string combinedText;
            for (auto widget : sortedWidgets)
            {
                if (widget->m_SelectionStart < 0 || widget->m_SelectionEnd < 0)
                    continue;

                int start = ImMin(widget->m_SelectionStart, widget->m_SelectionEnd);
                int end = ImMax(widget->m_SelectionStart, widget->m_SelectionEnd);
                start = ImClamp(start, 0, static_cast<int>(widget->m_Text.size()));
                end = ImClamp(end, 0, static_cast<int>(widget->m_Text.size()));

                if (start < end)
                {
                    combinedText += widget->m_Text.substr(start, end - start);
                    combinedText += "\n";
                }
            }

            // 复制到剪切板
            if (!combinedText.empty())
                ImGui::SetClipboardText(combinedText.c_str());
        }

        // 全局拖拽状态
        struct GlobalDragState {
            bool IsDragging = false;
            ImVec2 StartPos;
            ImVec2 CurrentPos;
            bool CtrlPressed = false;
        };

        static GlobalDragState s_DragState;

    private:
        void WrapText()
        {
            m_Lines.clear();
            m_LineWidths.clear();
            m_LinePositions.clear();

            if (m_Text.empty() || Size.x <= 0)
            {
                m_TextDirty = false;
                return;
            }

            ImFont* font = ImGui::GetFont();
            const float lineHeight = ImGui::GetTextLineHeight();
            float wrapWidth = Size.x;

            // 手动换行实现
            const char* textStart = m_Text.c_str();
            const char* textEnd = textStart + m_Text.size();
            const char* lineStart = textStart;

            while (lineStart < textEnd)
            {
                // 查找下一个换行符
                const char* nextLine = lineStart;
                while (nextLine < textEnd && *nextLine != '\n' && *nextLine != '\r')
                    nextLine++;

                // 处理当前行
                const char* lineEnd = nextLine;
                bool hasContent = false;

                while (lineStart < lineEnd)
                {
                    hasContent = true;
                    const char* wrapPos = lineStart;
                    const char* lastSpace = nullptr;
                    float currentWidth = 0.0f;

                    // 手动计算换行位置
                    const char* wordStart = lineStart;
                    while (wordStart < lineEnd)
                    {
                        // 查找单词边界
                        const char* wordEnd = wordStart;
                        while (wordEnd < lineEnd && *wordEnd != ' ' && *wordEnd != '\t')
                            wordEnd++;

                        // 计算单词宽度
                        std::string word(wordStart, wordEnd);
                        ImVec2 wordSize = font->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.0f, word.c_str());

                        // 检查是否超出宽度
                        if (currentWidth + wordSize.x > wrapWidth)
                        {
                            // 如果已经有内容，则换行
                            if (wrapPos > lineStart || lastSpace)
                            {
                                if (lastSpace)
                                    wrapPos = lastSpace;
                                break;
                            }
                        }

                        // 添加单词到当前行
                        currentWidth += wordSize.x;
                        wrapPos = wordEnd;

                        // 添加空格
                        if (wordEnd < lineEnd && (*wordEnd == ' ' || *wordEnd == '\t'))
                        {
                            lastSpace = wordEnd;
                            float spaceWidth = font->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.0f, " ").x;
                            currentWidth += spaceWidth;
                            wrapPos++;
                        }

                        wordStart = wordEnd + 1;
                        if (wordStart >= lineEnd) break;
                    }

                    // 确保至少添加一个字符
                    if (wrapPos == lineStart)
                        wrapPos++;

                    // 添加新行
                    std::string line(lineStart, wrapPos);
                    m_Lines.push_back(line);
                    m_LineWidths.push_back(font->CalcTextSizeA(
                        ImGui::GetFontSize(),
                        FLT_MAX,
                        0.0f,
                        line.c_str()
                    ).x);

                    lineStart = wrapPos;
                }

                // 处理空行（只有换行符）
                if (!hasContent && (lineEnd < textEnd && (*lineEnd == '\n' || *lineEnd == '\r')))
                {
                    m_Lines.push_back("");
                    m_LineWidths.push_back(0.0f);
                }

                // 跳过换行符
                if (lineStart < textEnd && (*lineStart == '\n' || *lineStart == '\r'))
                {
                    lineStart++;
                    if (lineStart < textEnd && *lineStart == '\n' && *(lineStart - 1) == '\r')
                        lineStart++;
                }
            }

            m_TextDirty = false;
        }
        void HandleSelection()
        {
            ImGuiIO& io = ImGui::GetIO();
            const ImVec2& mousePos = io.MousePos;
            ImRect widgetRect(Position, Position + Size);
            bool isMouseInWidget = widgetRect.Contains(mousePos);
            s_DragState.CtrlPressed = io.KeyCtrl;
            // 处理鼠标左键点击
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                if (isMouseInWidget)
                {
                    s_DragState.StartPos = mousePos;
                }

                for (auto& widget : s_SelectedWidgets)
                {
                    widget->ClearSelection();
                }
                ClearAllSelections();
            }

            if (ImGui::IsMouseDragging(0))
            {
                if (isMouseInWidget)
                {
                    // 开始全局拖拽
                    s_DragState.IsDragging = true;
                    s_DragState.CurrentPos = mousePos;
                }
            }
            else
            {
                s_DragState.IsDragging = false;
            }

            // 处理拖拽
            if (s_DragState.IsDragging)
            {
                s_DragState.CurrentPos = mousePos;

                if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
                {
                    // 计算拖拽矩形
                    ImRect dragRect(
                        ImMin(s_DragState.StartPos, s_DragState.CurrentPos),
                        ImMax(s_DragState.StartPos, s_DragState.CurrentPos)
                    );

                    // 检查当前控件是否与拖拽矩形相交
                    if (widgetRect.Overlaps(dragRect))
                    {
                        // 设置选中状态
                        s_SelectedWidgets.insert(this);

                        // 设置文本选择区域
                        SetSelectionByRect();
                    }
                    else
                    {
                        m_SelectionStart = 0;
                        m_SelectionEnd = 0;
                    }
                }

                if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
                {
                    s_DragState.IsDragging = false;
                }
            }

            // 处理复制操作
            if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_C))
                CopyAllSelectedTexts(); // 全局复制
        }
        void CopySelection();
        int GetCharIndexAtPosition(ImVec2 pos) const
        {
            if (m_Lines.empty()) return -1;

            // 转换为相对于控件的坐标
            ImVec2 localPos = pos - Position;
            if (localPos.y < 0) return 0;

            ImFont* font = ImGui::GetFont();
            const float lineHeight = ImGui::GetTextLineHeight() * m_LineSpacing;

            // 计算行索引
            int lineIndex = static_cast<int>(localPos.y / lineHeight);
            if (lineIndex < 0)
            {
                return -1;
            }
            if (lineIndex >= static_cast<int>(m_Lines.size()))
            {
                return m_Text.size();
            }
            //if (lineIndex < 0 || lineIndex >= static_cast<int>(m_Lines.size()))
            //    return -1;

            // 计算字符索引
            const std::string& line = m_Lines[lineIndex];
            float xOffset = localPos.x;
            float currentPos = 0.0f;
            int charIndex = 0;

            // 查找字符位置
            for (; charIndex < line.size(); charIndex++)
            {
                // 计算字符宽度
                std::string substr(line.substr(0, charIndex + 1));
                float charWidth = font->CalcTextSizeA(
                    ImGui::GetFontSize(),
                    FLT_MAX,
                    0.0f,
                    substr.c_str()
                ).x;

                if (xOffset < charWidth)
                {
                    // 计算全局索引
                    int globalIndex = 0;
                    for (int i = 0; i < lineIndex; i++)
                        globalIndex += static_cast<int>(m_Lines[i].size());

                    return globalIndex + charIndex;
                }
            }

            // 返回行尾索引
            int globalIndex = 0;
            for (int i = 0; i <= lineIndex; i++)
                globalIndex += static_cast<int>(m_Lines[i].size());

            return globalIndex - 1;
        }

        // 根据矩形设置选中区域
        void SetSelectionByRect()
        {
            // 计算矩形覆盖的文本范围
            //int startChar = GetCharIndexAtPosition(OverLapRect.Min);
            //int endChar = GetCharIndexAtPosition(OverLapRect.Max);

            int startChar = GetCharIndexAtPosition(s_DragState.StartPos);
            int endChar = GetCharIndexAtPosition(s_DragState.CurrentPos);

            //if (s_DragState.StartPos.y <= Position.y && startChar > 0)
            //{
            //    startChar = 0;
            //}

            //if (s_DragState.CurrentPos.x < s_DragState.StartPos.x && s_DragState.CurrentPos.y <= Position.y)
            //{
            //    startChar = 0;
            //}

            //if (s_DragState.CurrentPos.x < s_DragState.StartPos.x && s_DragState.CurrentPos.y >= (Position.y + Size.y))
            //{
            //    startChar = m_SelectionEnd;
            //    endChar = m_Text.size();
            //}


            // 确保有效范围
            if (startChar >= 0 && endChar >= 0)
            {
                m_SelectionStart = ImMin(startChar, endChar);
                m_SelectionEnd = ImMax(startChar, endChar);
            }
        }

        std::string m_Text;
        ImU32 m_TextColor = IM_COL32(0, 0, 0, 255);
        ImU32 m_BackgroundColor = IM_COL32(0, 0, 0, 0);
        ImU32 m_SelectionColor = IM_COL32(100, 150, 250, 100);
        float m_LineSpacing = 1.0f;

        // 文本行数据
        std::vector<std::string> m_Lines;
        std::vector<float> m_LineWidths;
        std::vector<ImVec2> m_LinePositions;
        bool m_TextDirty = true;

        // 文本选择状态
        int m_SelectionStart = -1;
        int m_SelectionEnd = -1;
        bool m_IsSelecting = false;
        ImVec2 m_SelectionStartPos;

        // 静态成员：存储所有选中的控件
        static std::set<ImMultiLineTextBlock*> s_SelectedWidgets;

        virtual std::unordered_set<PropertyInfo, PropertyInfo::Hasher> GetProperties() override {
            auto props = ImWidget::GetProperties();
            props.insert({
                "Text", PropertyType::String, "Content",
                [this](void* v) { SetText(*static_cast<std::string*>(v)); },
                [this]() -> void* { return &m_Text; }
                });
            props.insert({
                "TextColor", PropertyType::Color, "Appearance",
                [this](void* v) { m_TextColor = *static_cast<ImU32*>(v); },
                [this]() -> void* { return &m_TextColor; }
                });
            // 添加其他属性...
            return props;
        }

        virtual std::string GetRegisterTypeName()override { return "ImMultiLineTextBlock"; }
    };

    // 静态成员定义

}

