#pragma once
#include "ImScrollBox.h"
#include "ImMultiLineTextBlock.h"
#include <vector>
#include <algorithm>
#include <set>

namespace ImGuiWidget
{
    class ImScrollingTextList : public ImScrollBox
    {
    private:
        struct TextItem {
            std::string content;
            float height = 0.0f;
            ImMultiLineTextBlock* block = nullptr;
        };

        std::vector<TextItem> m_Items;
        float m_TotalHeight = 0.0f;
        float m_ItemSpacing = 5.0f;
        float m_ItemHeight = 20.0f; // 默认项高度
        ImU32 m_SelectionColor = IM_COL32(100, 150, 250, 100);

        // 字符级选择状态
        struct SelectionState {
            int startItem = -1;      // 起始条目索引
            int startChar = -1;      // 起始字符索引
            int endItem = -1;        // 结束条目索引
            int endChar = -1;        // 结束字符索引
            bool isSelecting = false; // 是否正在选择
        };

        SelectionState m_Selection;
        std::set<int> m_SelectedItems;

    public:
        ImScrollingTextList(const std::string& WidgetName)
            : ImScrollBox(WidgetName)
        {
            EnableVerticalScroll(true);
            ShowVerticalScrollbar(true);
        }

        virtual ~ImScrollingTextList()
        {
            for (auto& item : m_Items) {
                delete item.block;
            }
        }

        // 添加文本条目
        void AddItem(const std::string& text)
        {
            TextItem item;
            item.content = text;
            item.block = new ImMultiLineTextBlock("TextItem_" + std::to_string(m_Items.size()));
            item.block->SetText(text);
            item.block->SetSelectionColor(m_SelectionColor);
            item.block->SetSize(ImVec2(Size.x - m_ScrollbarThickness, 0)); // 宽度适应

            // 计算项高度
            ImVec2 minSize = item.block->GetMinSize();
            item.height = minSize.y + m_ItemSpacing;
            m_ItemHeight = item.height; // 更新默认高度

            m_Items.push_back(item);
            m_TotalHeight += item.height;

            // 更新内容大小
            m_ContentSize = ImVec2(Size.x, m_TotalHeight);
        }

        // 删除指定索引的条目
        void RemoveItem(int index)
        {
            if (index < 0 || index >= static_cast<int>(m_Items.size()))
                return;

            m_TotalHeight -= m_Items[index].height;
            delete m_Items[index].block;
            m_Items.erase(m_Items.begin() + index);
            m_ContentSize.y = m_TotalHeight;

            // 更新选择状态
            if (m_Selection.startItem == index) m_Selection.startItem = -1;
            if (m_Selection.endItem == index) m_Selection.endItem = -1;
        }

        // 清空所有条目
        void ClearAllItems()
        {
            for (auto& item : m_Items) {
                delete item.block;
            }
            m_Items.clear();
            m_TotalHeight = 0.0f;
            m_ContentSize = ImVec2(0, 0);
            ClearSelection();
        }

        // 获取条目数量
        int GetItemCount() const { return static_cast<int>(m_Items.size()); }

        // 更新条目内容
        void UpdateItem(int index, const std::string& newText)
        {
            if (index < 0 || index >= static_cast<int>(m_Items.size()))
                return;

            // 保存旧高度
            float oldHeight = m_Items[index].height;

            m_Items[index].content = newText;
            m_Items[index].block->SetText(newText);

            // 更新高度和总高度
            ImVec2 minSize = m_Items[index].block->GetMinSize();
            m_Items[index].height = minSize.y + m_ItemSpacing;
            m_TotalHeight += (m_Items[index].height - oldHeight);
            m_ContentSize.y = m_TotalHeight;
        }

        // 设置选择颜色
        void SetSelectionColor(ImU32 color)
        {
            m_SelectionColor = color;
            for (auto& item : m_Items) {
                if (item.block) {
                    item.block->SetSelectionColor(color);
                }
            }
        }

        // 清除选择状态
        void ClearSelection()
        {
            m_Selection = SelectionState();
            for (auto& item : m_Items) {
                if (item.block) {
                    item.block->ClearSelection();
                }
            }
        }

        virtual void Render() override
        {
            if (!Visibility) return;

            // 处理自动滚动
            HandleAutoScroll();

            // 渲染背景
            RenderBackGround();

            // 计算内容区域（排除滚动条）
            ImVec2 contentAvail = Size;
            if (m_ShowVerticalScrollbar && m_VerticalScrollEnabled)
                contentAvail.x -= m_ScrollbarThickness;

            // 计算可见项范围
            float startY = m_ScrollPosition.y;
            float endY = startY + contentAvail.y;

            int firstVisible = static_cast<int>(startY / m_ItemHeight);
            int lastVisible = static_cast<int>(endY / m_ItemHeight) + 1;
            firstVisible = ImClamp(firstVisible, 0, static_cast<int>(m_Items.size()) - 1);
            lastVisible = ImClamp(lastVisible, firstVisible, static_cast<int>(m_Items.size()) - 1);

            // 渲染可见项
            ImVec2 currentPos(Position.x, Position.y - m_ScrollPosition.y);
            for (int i = 0; i < firstVisible; ++i) {
                currentPos.y += m_Items[i].height;
            }

            for (int i = firstVisible; i <= lastVisible; ++i) {
                if (m_Items[i].block) {
                    m_Items[i].block->SetPosition(currentPos);
                    m_Items[i].block->SetSize(ImVec2(contentAvail.x, m_Items[i].height));
                    m_Items[i].block->Render();
                }
                currentPos.y += m_Items[i].height;
            }

            // 处理文本选择
            HandleSelection();

            // 渲染滚动条
            RenderCustomScrollbars();

            // 更新选择状态显示
            UpdateSelectionDisplay();
        }

    private:
        void HandleAutoScroll()
        {
            ImGuiIO& io = ImGui::GetIO();
            ImVec2 mousePos = io.MousePos;

            // 只在选择过程中启用自动滚动
            if (!m_Selection.isSelecting) return;

            // 检查是否需要垂直滚动
            const float scrollSpeed = 30.0f;
            const float scrollThreshold = 30.0f;

            // 检查鼠标是否在控件顶部/底部附近
            if (mousePos.y <= Position.y) {
                m_ScrollPosition.y -= scrollSpeed * io.DeltaTime;
            }
            else if (mousePos.y >= Position.y + Size.y) {
                m_ScrollPosition.y += scrollSpeed * io.DeltaTime;
            }

            // 确保滚动位置在有效范围内
            ClampScrollPosition();
        }

        void HandleSelection()
        {
            ImGuiIO& io = ImGui::GetIO();
            ImVec2 mousePos = io.MousePos;
            bool isMouseInWidget = ImRect(Position, Position + Size).Contains(mousePos);

            // 开始选择
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && isMouseInWidget) {
                m_Selection.isSelecting = true;

                // 查找起始条目
                for (int i = 0; i < static_cast<int>(m_Items.size()); ++i) {
                    ImRect itemRect(
                        m_Items[i].block->GetPosition(),
                        m_Items[i].block->GetPosition() + m_Items[i].block->GetSize()
                    );

                    if (itemRect.Contains(mousePos)) {
                        // 计算起始字符索引
                        ImVec2 localPos = mousePos - m_Items[i].block->GetPosition();
                        int charIndex = m_Items[i].block->GetCharIndexAtPosition(localPos);

                        if (charIndex >= 0) {
                            m_Selection.startItem = i;
                            m_Selection.startChar = charIndex;
                            m_Selection.endItem = i;
                            m_Selection.endChar = charIndex;
                        }
                        break;
                    }
                }
            }

            // 更新选择结束位置
            if (m_Selection.isSelecting && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                // 查找结束条目
                for (int i = 0; i < static_cast<int>(m_Items.size()); ++i) {
                    ImRect itemRect(
                        m_Items[i].block->GetPosition(),
                        m_Items[i].block->GetPosition() + m_Items[i].block->GetSize()
                    );

                    if (itemRect.Contains(mousePos)) {
                        // 计算结束字符索引
                        ImVec2 localPos = mousePos - m_Items[i].block->GetPosition();
                        int charIndex = m_Items[i].block->GetCharIndexAtPosition(localPos);

                        if (charIndex >= 0) {
                            m_Selection.endItem = i;
                            m_Selection.endChar = charIndex;
                        }
                        break;
                    }
                }
            }

            // 结束选择
            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                m_Selection.isSelecting = false;
            }

            // 处理复制操作
            if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_C)) {
                CopySelectedText();
            }
        }

        void UpdateSelectionDisplay()
        {
            // 清除所有条目的选择状态
            for (auto& item : m_Items) {
                if (item.block) {
                    item.block->ClearSelection();
                }
            }

            // 如果选择无效，直接返回
            if (m_Selection.startItem < 0 || m_Selection.endItem < 0)
                return;

            // 确保选择方向正确（从开始到结束）
            int startItem = m_Selection.startItem;
            int endItem = m_Selection.endItem;
            int startChar = m_Selection.startChar;
            int endChar = m_Selection.endChar;

            if (startItem > endItem || (startItem == endItem && startChar > endChar)) {
                std::swap(startItem, endItem);
                std::swap(startChar, endChar);
            }

            // 更新选择显示
            if (startItem == endItem) {
                // 同一条目内的选择
                if (m_Items[startItem].block) {
                    m_Items[startItem].block->SetSelection(startChar, endChar);
                }
            }
            else {
                // 跨条目选择
                // 起始条目：从起始字符到末尾
                if (m_Items[startItem].block) {
                    m_Items[startItem].block->SetSelection(startChar, m_Items[startItem].content.size());
                }

                // 中间条目：全选
                for (int i = startItem + 1; i < endItem; ++i) {
                    if (m_Items[i].block) {
                        m_Items[i].block->SetSelection(0, m_Items[i].content.size());
                    }
                }

                // 结束条目：从开始到结束字符
                if (m_Items[endItem].block) {
                    m_Items[endItem].block->SetSelection(0, endChar);
                }
            }
        }

        void CopySelectedText()
        {
            if (m_Selection.startItem < 0 || m_Selection.endItem < 0)
                return;

            // 确保选择方向正确
            int startItem = m_Selection.startItem;
            int endItem = m_Selection.endItem;
            int startChar = m_Selection.startChar;
            int endChar = m_Selection.endChar;

            if (startItem > endItem || (startItem == endItem && startChar > endChar)) {
                std::swap(startItem, endItem);
                std::swap(startChar, endChar);
            }

            // 构建复制文本
            std::string combinedText;

            if (startItem == endItem) {
                // 同一条目内的选择
                int start = ImClamp(startChar, 0, static_cast<int>(m_Items[startItem].content.size()));
                int end = ImClamp(endChar, 0, static_cast<int>(m_Items[startItem].content.size()));

                if (start < end) {
                    combinedText = m_Items[startItem].content.substr(start, end - start);
                }
            }
            else {
                // 跨条目选择
                // 起始条目
                if (startChar < static_cast<int>(m_Items[startItem].content.size())) {
                    combinedText += m_Items[startItem].content.substr(startChar);
                }
                combinedText += "\n";

                // 中间条目
                for (int i = startItem + 1; i < endItem; ++i) {
                    combinedText += m_Items[i].content;
                    combinedText += "\n";
                }

                // 结束条目
                if (endChar > 0) {
                    combinedText += m_Items[endItem].content.substr(0, endChar);
                }
            }

            // 复制到剪贴板
            if (!combinedText.empty()) {
                ImGui::SetClipboardText(combinedText.c_str());
            }
        }
    };
}