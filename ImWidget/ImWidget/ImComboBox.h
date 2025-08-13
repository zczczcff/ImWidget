#pragma once
#include "ImWidget.h"
#include "ImButton.h"
#include <vector>
#include <functional>

namespace ImGuiWidget
{
    class ImComboBox : public ImWidget
    {
    private:
        // 内部状态
        bool m_IsPopupOpen = false;
        int m_SelectedIndex = -1;
        std::vector<std::string> m_Items;

        // 按钮样式
        ButtonStateStyle m_NormalStyle;
        ButtonStateStyle m_HoveredStyle;
        ButtonStateStyle m_PressedStyle;
        ButtonStateStyle m_DropdownStyle;

        // 回调函数
        std::function<void(int)> OnSelectionChanged;
        std::function<void(void)> OnPopupOpened;
        std::function<void(void)> OnPopupClosed;

        // 尺寸参数
        float m_ArrowSize = 10.0f;
        float m_ItemHeight = 25.0f;
        float m_PopupRounding = 5.0f;
        ImVec2 m_PopupPadding = ImVec2(5, 5);

        // 样式颜色
        ImU32 m_PopupBgColor = IM_COL32(40, 40, 50, 255);
        ImU32 m_HighlightColor = IM_COL32(60, 60, 128, 255);
        ImU32 m_TextColor = IM_COL32(255, 255, 255, 255);

        // 状态跟踪
        bool m_WasPopupOpen = false;
        bool m_ShouldClosePopup = false;

    public:
        ImComboBox(const std::string& WidgetName)
            : ImWidget(WidgetName)
        {
            // 初始化默认样式
            m_NormalStyle = {
                IM_COL32(70, 70, 80, 255),
                5.0f,
                true,
                1.0f,
                IM_COL32(100, 100, 120, 255)
            };

            m_HoveredStyle = {
                IM_COL32(80, 80, 100, 255),
                5.0f,
                true,
                1.0f,
                IM_COL32(120, 120, 150, 255)
            };

            m_PressedStyle = {
                IM_COL32(60, 60, 80, 255),
                5.0f,
                true,
                1.0f,
                IM_COL32(100, 100, 120, 255)
            };

            m_DropdownStyle = m_NormalStyle;

        }

        virtual ~ImComboBox()
        {

        }

        // 设置选项
        void SetItems(const std::vector<std::string>& items)
        {
            m_Items = items;
            if (m_SelectedIndex >= static_cast<int>(m_Items.size()))
                m_SelectedIndex = -1;
        }

        // 添加单个选项
        void AddItem(const std::string& item)
        {
            m_Items.push_back(item);
        }

        // 清除所有选项
        void ClearItems()
        {
            m_Items.clear();
            m_SelectedIndex = -1;
        }

        // 设置当前选中项
        void SetSelectedIndex(int index)
        {
            if (index >= -1 && index < static_cast<int>(m_Items.size()))
                m_SelectedIndex = index;
        }

        void SetSelectedItem(const std::string& SelectedItem)
        {
            int i = 0;
            for (auto& item : m_Items)
            {
                if (item == SelectedItem)
                {
                    m_SelectedIndex = i;
                    return;
                }
                i++;
            }
        }

        // 获取当前选中项
        int GetSelectedIndex() const { return m_SelectedIndex; }

        // 获取当前选中文本
        std::string GetSelectedText() const
        {
            return (m_SelectedIndex >= 0 && m_SelectedIndex < static_cast<int>(m_Items.size())) ?
                m_Items[m_SelectedIndex] : "";
        }

        // 设置回调函数
        void SetOnSelectionChanged(std::function<void(int)> callback) { OnSelectionChanged = callback; }
        void SetOnPopupOpened(std::function<void(void)> callback) { OnPopupOpened = callback; }
        void SetOnPopupClosed(std::function<void(void)> callback) { OnPopupClosed = callback; }

        // 设置按钮样式
        void SetNormalStyle(const ButtonStateStyle& style) { m_NormalStyle = style; }
        void SetHoveredStyle(const ButtonStateStyle& style) { m_HoveredStyle = style; }
        void SetPressedStyle(const ButtonStateStyle& style) { m_PressedStyle = style; }

        // 设置下拉列表样式
        void SetPopupBackgroundColor(ImU32 color) { m_PopupBgColor = color; }
        void SetHighlightColor(ImU32 color) { m_HighlightColor = color; }
        void SetTextColor(ImU32 color) { m_TextColor = color; }
        void SetPopupRounding(float rounding) { m_PopupRounding = rounding; }
        void SetItemHeight(float height) { m_ItemHeight = height; }

        virtual ImVec2 GetMinSize() override
        {
            // 计算最小尺寸（基于最长文本）
            float width = 100.0f; // 最小宽度
            float height = m_ItemHeight;

            ImGuiContext& g = *GImGui;
            for (const auto& item : m_Items)
            {
                ImVec2 textSize = ImGui::CalcTextSize(item.c_str());
                width = ImMax(width, textSize.x);
            }

            // 加上箭头和边距的空间
            width += m_ArrowSize + m_PopupPadding.x * 4;
            height += m_PopupPadding.y * 2;

            return ImVec2(width, height);
        }

        virtual void Render() override
        {
            if (!Visibility) return;

            ImGuiWindow* window = ImGui::GetCurrentWindow();
            if (window->SkipItems) return;

            const ImGuiID id = window->GetID(m_WidgetID.c_str());
            ImRect total_bb(Position, Position + Size);

            // 处理组合框的按钮部分
            RenderDropdownButton(id, total_bb);

            // 处理弹出窗口
            HandlePopup(id, total_bb);

            // 处理全局点击（关闭弹出窗口）
            HandleGlobalClick();
        }

    private:
        void RenderDropdownButton(ImGuiID id, const ImRect& total_bb)
        {
            ImGuiContext& g = *GImGui;
            ImGuiWindow* window = g.CurrentWindow;

            // 确定按钮状态
            const bool is_hovered = total_bb.Contains(g.IO.MousePos);
            if (!ImGui::ItemAdd(total_bb, id)) return;
            const bool is_clicked = ImGui::ButtonBehavior(total_bb, id, nullptr, nullptr, ImGuiButtonFlags_AllowOverlap);

            if (is_clicked)
            {
                m_IsPopupOpen = !m_IsPopupOpen;
            }

            // 根据状态选择样式
            const ButtonStateStyle* currentStyle = &m_NormalStyle;
            if (m_IsPopupOpen)
            {
                currentStyle = &m_PressedStyle;
            }
            else if (is_hovered)
            {
                currentStyle = &m_HoveredStyle;
            }

            // 绘制按钮背景
            window->DrawList->AddRectFilled(
                total_bb.Min,
                total_bb.Max,
                currentStyle->BackgroundColor,
                currentStyle->Rounding
            );

            // 绘制边框
            if (currentStyle->HasBorder && currentStyle->BorderThickness > 0.0f)
            {
                window->DrawList->AddRect(
                    total_bb.Min,
                    total_bb.Max,
                    currentStyle->BorderColor,
                    currentStyle->Rounding,
                    0,
                    currentStyle->BorderThickness
                );
            }

            // 绘制文本
            std::string displayText = GetSelectedText();
            if (displayText.empty()) displayText = "Select...";

            ImVec2 textSize = ImGui::CalcTextSize(displayText.c_str());
            ImVec2 textPos = total_bb.Min + ImVec2(
                m_PopupPadding.x,
                (total_bb.GetHeight() - textSize.y) * 0.5f
            );

            window->DrawList->AddText(textPos, m_TextColor, displayText.c_str());

            // 绘制下拉箭头
            ImVec2 arrowMin = ImVec2(
                total_bb.Max.x - m_ArrowSize - m_PopupPadding.x,
                total_bb.Min.y + (total_bb.GetHeight() - m_ArrowSize) * 0.5f
            );

            ImVec2 arrowMax = arrowMin + ImVec2(m_ArrowSize, m_ArrowSize);

            window->DrawList->AddTriangleFilled(
                ImVec2(arrowMin.x, arrowMin.y),
                ImVec2(arrowMin.x + m_ArrowSize, arrowMin.y),
                ImVec2(arrowMin.x + m_ArrowSize * 0.5f, arrowMin.y + m_ArrowSize),
                m_TextColor
            );
        }

        void HandlePopup(ImGuiID id, const ImRect& total_bb)
        {
            ImGuiContext& g = *GImGui;

            // 检查是否需要打开弹出窗口
            bool wasPopupOpen = m_IsPopupOpen;
            //if (ImGui::IsItemActivated())
            //{
            //    m_IsPopupOpen = !m_IsPopupOpen;
            //}

            // 处理弹出窗口状态变化
            if (m_IsPopupOpen && !wasPopupOpen && OnPopupOpened)
            {
                OnPopupOpened();
            }
            else if (!m_IsPopupOpen && wasPopupOpen && OnPopupClosed)
            {
                OnPopupClosed();
            }

            // 显示弹出窗口
            if (m_IsPopupOpen)
            {
                // 计算弹出窗口位置和大小
                ImVec2 popupSize = ImVec2(
                    total_bb.GetWidth(),
                    (static_cast<float>(m_Items.size()) * m_ItemHeight + m_PopupPadding.y * 2
                    ));

                ImVec2 popupPos = ImVec2(
                    total_bb.Min.x,
                    total_bb.Max.y + 2.0f
                );

                // 确保弹出窗口在屏幕内
                ImRect popupRect(popupPos, popupPos + popupSize);
                ImRect viewportRect(ImGui::GetMainViewport()->Pos, ImGui::GetMainViewport()->Pos + ImGui::GetMainViewport()->Size);

                if (popupRect.Max.y > viewportRect.Max.y)
                {
                    popupPos.y = total_bb.Min.y - popupSize.y - 2.0f;
                    popupRect = ImRect(popupPos, popupPos + popupSize);
                }

                if (popupRect.Max.x > viewportRect.Max.x)
                {
                    popupPos.x = viewportRect.Max.x - popupSize.x;
                    popupRect = ImRect(popupPos, popupPos + popupSize);
                }

                // 设置弹出窗口标志
                ImGuiWindowFlags flags =
                    ImGuiWindowFlags_NoTitleBar |
                    ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoMove |
                    ImGuiWindowFlags_NoScrollbar |
                    ImGuiWindowFlags_NoSavedSettings |
                    ImGuiWindowFlags_NoFocusOnAppearing;

                // 开始弹出窗口
                ImGui::SetNextWindowPos(popupPos);
                ImGui::SetNextWindowSize(popupSize);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, m_PopupPadding);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, m_PopupRounding);
                ImGui::PushStyleColor(ImGuiCol_WindowBg, m_PopupBgColor);

                if (ImGui::Begin(m_WidgetID.c_str(), nullptr, flags))
                {
                    // 绘制弹出窗口背景
                    ImGuiWindow* popupWindow = ImGui::GetCurrentWindow();
                    popupWindow->DrawList->AddRectFilled(
                        popupWindow->Pos,
                        popupWindow->Pos + popupWindow->Size,
                        m_PopupBgColor,
                        m_PopupRounding
                    );

                    // 绘制选项
                    for (int i = 0; i < static_cast<int>(m_Items.size()); i++)
                    {
                        ImVec2 itemMin = ImVec2(
                            popupWindow->Pos.x + m_PopupPadding.x,
                            popupWindow->Pos.y + m_PopupPadding.y + i * m_ItemHeight
                        );

                        ImVec2 itemMax = ImVec2(
                            popupWindow->Pos.x + popupWindow->Size.x - m_PopupPadding.x,
                            popupWindow->Pos.y + m_PopupPadding.y + (i + 1) * m_ItemHeight
                        );

                        ImRect itemRect(itemMin, itemMax);
                        bool isHovered = itemRect.Contains(g.IO.MousePos);
                        bool isSelected = (i == m_SelectedIndex);

                        // 绘制项目背景
                        if (isSelected || isHovered)
                        {
                            popupWindow->DrawList->AddRectFilled(
                                itemMin,
                                itemMax,
                                isSelected ? m_HighlightColor : (m_HighlightColor & IM_COL32(255, 255, 255, 180)),
                                4.0f
                            );
                        }

                        // 绘制项目文本
                        ImVec2 textPos = ImVec2(
                            itemMin.x + 5.0f,
                            itemMin.y + (m_ItemHeight - ImGui::GetTextLineHeight()) * 0.5f
                        );

                        popupWindow->DrawList->AddText(textPos, m_TextColor, m_Items[i].c_str());

                        // 处理项目点击
                        if (ImGui::IsMouseClicked(0) ){
                            if (itemRect.Contains(g.IO.MousePos)) {
                                m_SelectedIndex = i;
                                    m_ShouldClosePopup = true;

                                    if (OnSelectionChanged) {
                                        OnSelectionChanged(i);
                                    }
                            }
                        }
                    }
                }

                ImGui::End();
                ImGui::PopStyleColor();
                ImGui::PopStyleVar(2);
            }
        }

        void HandleGlobalClick()
        {
            // 检查是否需要关闭弹出窗口
            if (m_ShouldClosePopup)
            {
                m_IsPopupOpen = false;
                m_ShouldClosePopup = false;
                return;
            }

            // 点击外部关闭弹出窗口
            if (m_IsPopupOpen && ImGui::IsMouseClicked(0))
            {
                ImVec2 mousePos = ImGui::GetMousePos();
                ImRect comboRect(Position, Position + Size);

                // 计算弹出窗口位置
                ImVec2 popupPos = ImVec2(Position.x, Position.y + Size.y + 2.0f);
                ImVec2 popupSize = ImVec2(
                    Size.x,
                    (static_cast<float>(m_Items.size()) * m_ItemHeight + m_PopupPadding.y * 2
                    ));

                ImRect popupRect(popupPos, popupPos + popupSize);

                // 如果点击在组合框和弹出窗口之外
                if (!comboRect.Contains(mousePos) && !popupRect.Contains(mousePos))
                {
                    m_IsPopupOpen = false;
                }
            }
        }


    public:
        virtual std::unordered_set<PropertyInfo, PropertyInfo::Hasher> GetProperties() override
        {
            auto baseProps = ImWidget::GetProperties();

            baseProps.insert(
                { "Items", PropertyType::StringArray, "Data",
                  [this](void* v) { m_Items = *static_cast<std::vector<std::string>*>(v); },
                  [this]() -> void* { return &m_Items; } }
            );

            baseProps.insert(
                { "SelectedIndex", PropertyType::Int, "Data",
                  [this](void* v) { m_SelectedIndex = *static_cast<int*>(v); },
                  [this]() -> void* { return &m_SelectedIndex; } }
            );

            baseProps.insert(
                { "ArrowSize", PropertyType::Float, "Appearance",
                  [this](void* v) { m_ArrowSize = *static_cast<float*>(v); },
                  [this]() -> void* { return &m_ArrowSize; } }
            );

            baseProps.insert(
                { "ItemHeight", PropertyType::Float, "Appearance",
                  [this](void* v) { m_ItemHeight = *static_cast<float*>(v); },
                  [this]() -> void* { return &m_ItemHeight; } }
            );

            baseProps.insert(
                { "PopupRounding", PropertyType::Float, "Appearance",
                  [this](void* v) { m_PopupRounding = *static_cast<float*>(v); },
                  [this]() -> void* { return &m_PopupRounding; } }
            );

            baseProps.insert(
                { "PopupPadding", PropertyType::Vec2, "Appearance",
                  [this](void* v) { m_PopupPadding = *static_cast<ImVec2*>(v); },
                  [this]() -> void* { return &m_PopupPadding; } }
            );

            baseProps.insert(
                { "PopupBgColor", PropertyType::Color, "Appearance",
                  [this](void* v) { m_PopupBgColor = *static_cast<ImU32*>(v); },
                  [this]() -> void* { return &m_PopupBgColor; } }
            );

            baseProps.insert(
                { "HighlightColor", PropertyType::Color, "Appearance",
                  [this](void* v) { m_HighlightColor = *static_cast<ImU32*>(v); },
                  [this]() -> void* { return &m_HighlightColor; } }
            );

            baseProps.insert(
                { "TextColor", PropertyType::Color, "Appearance",
                  [this](void* v) { m_TextColor = *static_cast<ImU32*>(v); },
                  [this]() -> void* { return &m_TextColor; } }
            );

            baseProps.insert(
                { "NormalStyle", PropertyType::Struct, "Style",
                  [this](void* v) { m_NormalStyle = *(ButtonStateStyle*)v; },
                  [this]() -> void* { return const_cast<ButtonStateStyle*>(&m_NormalStyle); } }
            );

            baseProps.insert(
                { "HoveredStyle", PropertyType::Struct, "Style",
                  [this](void* v) { m_HoveredStyle = *(ButtonStateStyle*)v; },
                  [this]() -> void* { return const_cast<ButtonStateStyle*>(&m_HoveredStyle); } }
            );

            baseProps.insert(
                { "PressedStyle", PropertyType::Struct, "Style",
                  [this](void* v) { m_PressedStyle = *(ButtonStateStyle*)v; },
                  [this]() -> void* { return const_cast<ButtonStateStyle*>(&m_PressedStyle); } }
            );

            baseProps.insert(
                { "DropdownStyle", PropertyType::Struct, "Style",
                  [this](void* v) { m_DropdownStyle = *(ButtonStateStyle*)v; },
                  [this]() -> void* { return const_cast<ButtonStateStyle*>(&m_DropdownStyle); } }
            );

            return baseProps;
        }

        virtual std::string GetRegisterTypeName()override { return "ImComboBox"; }
    };
}
