#pragma once

#include "ImPanelWidget.h"
#include "imgui_internal.h"
#include <functional>

namespace ImGuiWidget
{
    class ButtonStateStyle : public PropertyStruct 
    {
    public:
        ImU32 BackgroundColor = IM_COL32(0, 0, 0, 0);
        float Rounding = 0.0f;
        bool HasBorder = false;
        float BorderThickness = 1.0f;
        ImU32 BorderColor = IM_COL32(0, 0, 0, 0);

        std::vector<PropertyInfo> GetProperties()  override 
        {
            return 
            {
                {"BackgroundColor", PropertyType::Color, "Style",
                    [this](void* v) { BackgroundColor = *static_cast<ImU32*>(v); },
                    [this]() -> void* { return &BackgroundColor; }},

                {"Rounding", PropertyType::Float, "Style",
                    [this](void* v) { Rounding = *static_cast<float*>(v); },
                    [this]() -> void* { return &Rounding; }},

                {"HasBorder", PropertyType::Bool, "Border",
                    [this](void* v) { HasBorder = *static_cast<bool*>(v); },
                    [this]() -> void* { return &HasBorder; }},

                {"BorderThickness", PropertyType::Float, "Border",
                    [this](void* v) { BorderThickness = *static_cast<float*>(v); },
                    [this]() -> void* { return &BorderThickness; }},

                {"BorderColor", PropertyType::Color, "Border",
                    [this](void* v) { BorderColor = *static_cast<ImU32*>(v); },
                    [this]() -> void* { return &BorderColor; }}
            };
        }
        ButtonStateStyle(){}
        ButtonStateStyle(ImU32 BackgroundColor, float Rounding, bool HasBorder, float BorderThickness, ImU32 BorderColor):
            BackgroundColor(BackgroundColor),
            Rounding(Rounding),
            HasBorder(HasBorder),
            BorderThickness(BorderThickness),
            BorderColor(BorderColor)
        {}
        //void operator=(const ButtonStateStyle& From) {};
    };

    class ImButton : public ImPanelWidget
    {
    protected:
        bool m_LastFrameHeld = false; // 跟踪上一帧的按下状态
        std::string m_TooltipText;
        // 回调函数
        std::function<void(void)> OnPressed;
        std::function<void(void)> OnReleased;
        std::function<void(void)> OnHoverBegin;
        std::function<void(void)> OnHoverEnd;

        ImGuiButtonFlags m_ButtonFlag = ImGuiButtonFlags_AllowOverlap;

        // 分别存储三种状态的样式
        ButtonStateStyle m_NormalStyle;
        ButtonStateStyle m_HoveredStyle;
        ButtonStateStyle m_PressedStyle;

        bool m_WasHovered = false;  // 上一帧悬停状态
        void RenderButton()
        {
            ImGuiWindow* window = ImGui::GetCurrentWindow();
            ImGuiContext& g = *GImGui;
            const ImGuiID id = window->GetID(m_WidgetName.c_str());
            ImRect bb(Position, Position + Size);

            //ImGui::ItemSize(Size, g.Style.FramePadding.y);
            if (!ImGui::ItemAdd(bb, id)) {}

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
                if (m_WidgetName == "CloseButton_test")
                {
                    printf("test");
                }
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

            if (hovered && !m_TooltipText.empty() &&
                ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
            {
                ImGui::BeginTooltip();
                ImGui::TextUnformatted(m_TooltipText.c_str());
                ImGui::EndTooltip();
            }
        }
        virtual void Relayout() override
        {
            auto slot = GetSlotAt(0);
            if (slot)
            {
                slot->SetSlotPosition(Position);
                slot->SetSlotSize(Size);
                slot->ApplyLayout();
            }
        }
        virtual ImSlot* CreateSlot(ImWidget* Content)override
        {
            return new ImPaddingSlot(Content);
        }
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
            bHaveBackGround = false;
            
        }

        // 设置内容
        void SetContent(ImWidget* child) 
        {
            SetChildAt(0, child);
            //ImSlot* newslot = new ImSlot(child);
            //if (m_Slots.size() == 1)
            //{
            //    delete m_Slots[0];
            //    m_Slots[0] = newslot;
            //}
            //else
            //{
            //    m_Slots.push_back(newslot);
            //}
        }
        ImPaddingSlot* GetContentSlot()
        {
            return (ImPaddingSlot*)GetSlotAt(0);
        }
        // 设置工具提示文本
        void SetTooltipText(const std::string& text) { m_TooltipText = text; }
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
            auto content = GetChildAt(0);
            if (content)
            {
                ImVec2 ContentMinSize = content->GetMinSize();
                return ImVec2(max(ContentMinSize.x, 30.f), max(ContentMinSize.y, 10.f));
            }
            //if (m_Slots.size() > 0 && m_Slots[0])
            //{
            //    ImVec2 ContentMinSize = m_Slots[0]->GetContent()->GetMinSize();
            //    return ImVec2(max(ContentMinSize.x, 30.f), max(ContentMinSize.y, 10.f));
            //}
            else
            {
                return ImVec2(30, 10);
            }
        }

        virtual void Render()
        {
            HandleLayout();
            RenderButton();
            RenderBackGround();
            RenderChild();
        }


        std::vector<PropertyInfo> GetProperties() override 
        {
            auto baseProps = ImPanelWidget::GetProperties();

            // 添加按钮特有属性
            baseProps.push_back(
                { "TooltipText", PropertyType::String, "Behavior",
                 [this](void* v) { m_TooltipText = *static_cast<std::string*>(v); },
                 [this]() -> void* { return &m_TooltipText; } }
            );

            // 添加样式结构体属性
            baseProps.push_back(
                { "NormalStyle", PropertyType::Struct, "Style",
                 [this](void* v) { m_NormalStyle = *(ButtonStateStyle*)v; },
                 [this]() -> void* { return const_cast<ButtonStateStyle*>(&m_NormalStyle); } }
            );

            baseProps.push_back(
                { "HoveredStyle", PropertyType::Struct, "Style",
                 [this](void* v) { m_HoveredStyle = *(ButtonStateStyle*)v; },
                 [this]() -> void* { return const_cast<ButtonStateStyle*>(&m_HoveredStyle); } }
            );

            baseProps.push_back(
                { "PressedStyle", PropertyType::Struct, "Style",
                 [this](void* v) { m_PressedStyle = *(ButtonStateStyle*)v; },
                 [this]() -> void* { return const_cast<ButtonStateStyle*>(&m_PressedStyle); } }
            );

            return baseProps;
        }
    };
}