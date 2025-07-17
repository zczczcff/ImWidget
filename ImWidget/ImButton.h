#pragma once

#include "ImPanelWidget.h"
#include "imgui_internal.h"
#include <functional>

namespace ImGuiWidget
{
    // 按钮基础样式结构
    struct ButtonStateStyle
    {
        ImU32 BackgroundColor;   // 背景颜色
        float Rounding;          // 四边圆角
        bool HasBorder;          // 有无边框
        float BorderThickness;   // 边框厚度
        ImU32 BorderColor;       // 边框颜色

        // 构造函数设置默认值
        ButtonStateStyle(ImU32 bgColor = IM_COL32(0, 0, 0, 0),
            float rounding = 0.0f,
            bool hasBorder = false,
            float borderThickness = 1.0f,
            ImU32 borderColor = IM_COL32(0, 0, 0, 0))
            : BackgroundColor(bgColor)
            , Rounding(rounding)
            , HasBorder(hasBorder)
            , BorderThickness(borderThickness)
            , BorderColor(borderColor)
        {}
    };

    class ImButton : public ImPanelWidget
    {
    protected:
        // 回调函数
        std::function<void(void)> OnPressed;
        std::function<void(void)> OnReleased;
        std::function<void(void)> OnHoverBegin;
        std::function<void(void)> OnHoverEnd;

        ImGuiButtonFlags m_ButtonFlag = 0;

        // 分别存储三种状态的样式
        ButtonStateStyle m_NormalStyle;
        ButtonStateStyle m_HoveredStyle;
        ButtonStateStyle m_PressedStyle;

        bool m_WasHovered = false;  // 上一帧悬停状态

    public:
        ImButton(const std::string& WidgetName) : ImPanelWidget(WidgetName)
        {
            // 使用ImGui默认样式初始化
            //const ImGuiStyle& style = ImGui::GetStyle();

            m_NormalStyle = {
                IM_COL32(0, 102, 204, 255),
                0.f,
                false,
                1.0f,
                IM_COL32(255, 255, 255, 255)
            };

            m_HoveredStyle = {
                IM_COL32(51, 153, 255, 255),
                0.f,
                false,
                1.0f,
                IM_COL32(255, 255, 255, 255)
            };

            m_PressedStyle = {
                IM_COL32(0, 51, 153, 255),
                0.f,
                false,
                1.0f,
                IM_COL32(255, 255, 255, 255)
            };
        }

        // 设置内容
        void SetContent(ImWidget* child) 
        {
            ImSlot* newslot = new ImSlot(child);
            if (m_Slots.size() == 1)
            {
                delete m_Slots[0];
                m_Slots[0] = newslot;
            }
            else
            {
                m_Slots.push_back(newslot);
            }
        }

        // 设置回调函数
        void SetOnPressed(std::function<void(void)> callback) { OnPressed = callback; }
        void SetOnReleased(std::function<void(void)> callback) { OnReleased = callback; }
        void SetOnHoverBegin(std::function<void(void)> callback) { OnHoverBegin = callback; }
        void SetOnHoverEnd(std::function<void(void)> callback) { OnHoverEnd = callback; }

        // 设置状态样式 - 提供独立设置每种状态的方法
        void SetNormalStyle(const ButtonStateStyle& style) { m_NormalStyle = style; }
        void SetHoveredStyle(const ButtonStateStyle& style) { m_HoveredStyle = style; }
        void SetPressedStyle(const ButtonStateStyle& style) { m_PressedStyle = style; }

        // 获取状态样式
        ButtonStateStyle& GetNormalStyle() { return m_NormalStyle; }
        ButtonStateStyle& GetHoveredStyle() { return m_HoveredStyle; }
        ButtonStateStyle& GetPressedStyle() { return m_PressedStyle; }

        virtual ImVec2 GetMinSize() 
        {
            if (m_Slots.size() > 0 && m_Slots[0])
            {
                ImVec2 ContentMinSize = m_Slots[0]->GetContent()->GetMinSize();
                return ImVec2(max(ContentMinSize.x, 30.f), max(ContentMinSize.y, 10.f));
            }
            else
            {
                return ImVec2(30, 10);
            }
        }

        virtual void Render()
        {
            ImGuiWindow* window = ImGui::GetCurrentWindow();
            ImGuiContext& g = *GImGui;
            const ImGuiID id = window->GetID(m_WidgetName.c_str());
            ImRect bb(Position, Position + Size);

            ImGui::ItemSize(Size, g.Style.FramePadding.y);
            if (!ImGui::ItemAdd(bb, id)) return;

            // 检测按钮行为
            bool hovered, held;
            bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, m_ButtonFlag);

            // 处理悬停状态变化
            if (hovered && !m_WasHovered && OnHoverBegin) 
            {
                OnHoverBegin();
            }
            if (!hovered && m_WasHovered && OnHoverEnd) 
            {
                OnHoverEnd();
            }
            m_WasHovered = hovered;

            // 处理按下/松开事件
            if (held && !m_LastFrameHeld && OnPressed) 
            {
                OnPressed();
            }
            if (!held && m_LastFrameHeld && OnReleased) 
            {
                OnReleased();
            }
            m_LastFrameHeld = held;

            // 根据当前状态选择样式
            const ButtonStateStyle* currentStyle = &m_NormalStyle;
            if (held) 
            {
                currentStyle = &m_PressedStyle;
            }
            else if (hovered) 
            {
                currentStyle = &m_HoveredStyle;
            }

            // 绘制按钮背景
            window->DrawList->AddRectFilled(
                bb.Min,
                bb.Max,
                currentStyle->BackgroundColor,
                currentStyle->Rounding,
                0
            );

            // 绘制边框
            if (currentStyle->HasBorder && currentStyle->BorderThickness > 0.0f)
            {
                window->DrawList->AddRect(
                    bb.Min,
                    bb.Max,
                    currentStyle->BorderColor,
                    currentStyle->Rounding,
                    0, // 默认全部圆角
                    currentStyle->BorderThickness
                );
            }

            // 渲染子内容
            if (m_Slots.size() == 1)
            {
                m_Slots[0]->GetContent()->SetPosition(Position);
                m_Slots[0]->GetContent()->SetSize(Size);
                m_Slots[0]->GetContent()->Render();
            }

        }

    private:
        bool m_LastFrameHeld = false; // 跟踪上一帧的按下状态
    };
}