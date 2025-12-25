#pragma once

#include "ImPanelWidget.h"
#include "imgui_internal.h"
#include <functional>
#include "ImEvent/ImMouseEvent.h"
#include "ImEvent/ImFocusEvent.h"
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

        std::unordered_set<PropertyInfo, PropertyInfo::Hasher> GetProperties()  override
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
        // 按钮状态
        bool m_IsHovered = false;
        bool m_IsPressed = false;

        std::string m_TooltipText;

        // 回调函数
        std::function<void(void)> OnPressed;
        std::function<void(void)> OnReleased;
        std::function<void(void)> OnHoverBegin;
        std::function<void(void)> OnHoverEnd;
        std::function<void(void)> OnFocusGained;
        std::function<void(void)> OnFocusLost;

        // 分别存储三种状态的样式
        ButtonStateStyle m_NormalStyle;
        ButtonStateStyle m_HoveredStyle;
        ButtonStateStyle m_PressedStyle;
        ButtonStateStyle m_FocusedStyle;

        void RenderButton()
        {
            ImGuiWindow* window = ImGui::GetCurrentWindow();

            // 根据当前状态选择样式
            const ButtonStateStyle* currentStyle = &m_NormalStyle;

            if (m_IsPressed)
            {
                currentStyle = &m_PressedStyle;
            }
            else if (m_IsHovered)
            {
                currentStyle = &m_HoveredStyle;
            }
            else if (m_hasFocus)
            {
                currentStyle = &m_FocusedStyle;
            }

            ImRect bb(Position, Position + Size);

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
                    0,
                    currentStyle->BorderThickness
                );
            }

            // 工具提示（如果悬停且设置了工具提示文本）
            if (m_IsHovered && !m_TooltipText.empty())
            {
                ImGui::SetTooltip("%s", m_TooltipText.c_str());
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

        virtual ImSlot* CreateSlot(ImWidget* Content) override
        {
            return new ImPaddingSlot(Content);
        }

        // 事件处理
        virtual void HandleEventInternal(ImEvent* event) override
        {
            if (event->IsHandled()) return;
            if (event->GetPhase() == ImEventPhase::Capture)return;
            switch (event->GetType())
            {
            case ImEventType::MouseEnter:
                HandleMouseEnter(event->As<ImMouseEnterEvent>());
                break;

            case ImEventType::MouseLeave:
                HandleMouseLeave(event->As<ImMouseLeaveEvent>());
                break;

            case ImEventType::MouseDown:
                HandleMouseDown(event->As<ImMouseDownEvent>());
                break;

            case ImEventType::MouseUp:
                HandleMouseUp(event->As<ImMouseUpEvent>());
                break;

            case ImEventType::FocusIn:
                HandleFocusIn(event->As<ImFocusInEvent>());
                break;

            case ImEventType::FocusOut:
                HandleFocusOut(event->As<ImFocusOutEvent>());
                break;

            case ImEventType::MouseClick:
                // 可以选择处理点击事件，或者继续冒泡
                break;

            case ImEventType::MouseDoubleClick:
                // 处理双击事件
                break;

            default:
                // 其他事件继续冒泡
                break;
            }
        }

        void HandleMouseEnter(ImMouseEnterEvent* event)
        {
            if (!m_IsHovered)
            {
                m_IsHovered = true;
                if (OnHoverBegin)
                {
                    OnHoverBegin();
                }
            }
            event->StopPropagation(); // 阻止事件继续冒泡
        }

        void HandleMouseLeave(ImMouseLeaveEvent* event)
        {
            if (m_IsHovered)
            {
                m_IsHovered = false;
                if (OnHoverEnd)
                {
                    OnHoverEnd();
                }
            }
            event->StopPropagation();
        }

        void HandleMouseDown(ImMouseDownEvent* event)
        {
            if (event->GetButton() == ImMouseButton::Left)
            {
                m_IsPressed = true;
                if (OnPressed)
                {
                    OnPressed();
                }

                // 请求焦点
                RequestFocus();
                event->StopPropagation();
            }
        }

        void HandleMouseUp(ImMouseUpEvent* event)
        {
            if (event->GetButton() == ImMouseButton::Left && m_IsPressed)
            {
                m_IsPressed = false;
                if (OnReleased)
                {
                    OnReleased();
                }
                event->StopPropagation();
            }
        }

        void HandleFocusIn(ImFocusInEvent* event)
        {
            if (!m_hasFocus)
            {
                m_hasFocus = true;
                if (OnFocusGained)
                {
                    OnFocusGained();
                }
            }
        }

        void HandleFocusOut(ImFocusOutEvent* event)
        {
            if (m_hasFocus)
            {
                m_hasFocus = false;
                m_IsPressed = false; // 失去焦点时释放按下状态
                if (OnFocusLost)
                {
                    OnFocusLost();
                }
            }
        }



    public:
        ImButton(const std::string& WidgetName) : 
            ImPanelWidget(WidgetName)
        {
            // 初始化默认样式
            m_NormalStyle = {
                IM_COL32(0, 102, 204, 255),
                4.0f,
                true,
                1.0f,
                IM_COL32(200, 200, 200, 255)
            };

            m_HoveredStyle = {
                IM_COL32(51, 153, 255, 255),
                4.0f,
                true,
                1.0f,
                IM_COL32(100, 150, 255, 255)
            };

            m_PressedStyle = {
                IM_COL32(0, 51, 153, 255),
                4.0f,
                true,
                2.0f,
                IM_COL32(50, 100, 200, 255)
            };

            m_FocusedStyle = {
                IM_COL32(0, 102, 204, 255),
                4.0f,
                true,
                2.0f,
                IM_COL32(255, 255, 100, 255)
            };

            bHaveBackGround = false;
            SetFocusable(true);
        }

        // 设置内容
        void SetContent(ImWidget* child)
        {
            SetChildAt(0, child);
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
        void SetOnFocusGained(std::function<void(void)> callback) { OnFocusGained = callback; }
        void SetOnFocusLost(std::function<void(void)> callback) { OnFocusLost = callback; }

        // 设置状态样式
        void SetNormalStyle(const ButtonStateStyle& style) { m_NormalStyle = style; }
        void SetHoveredStyle(const ButtonStateStyle& style) { m_HoveredStyle = style; }
        void SetPressedStyle(const ButtonStateStyle& style) { m_PressedStyle = style; }
        void SetFocusedStyle(const ButtonStateStyle& style) { m_FocusedStyle = style; }

        // 获取状态样式
        ButtonStateStyle& GetNormalStyle() { return m_NormalStyle; }
        ButtonStateStyle& GetHoveredStyle() { return m_HoveredStyle; }
        ButtonStateStyle& GetPressedStyle() { return m_PressedStyle; }
        ButtonStateStyle& GetFocusedStyle() { return m_FocusedStyle; }

        // 获取当前状态
        bool IsHovered() const { return m_IsHovered; }
        bool IsPressed() const { return m_IsPressed; }

        virtual ImVec2 GetMinSize()
        {
            auto content = GetChildAt(0);
            if (content)
            {
                ImVec2 ContentMinSize = content->GetMinSize();
                return ImVec2(ImMax(ContentMinSize.x, 30.f), ImMax(ContentMinSize.y, 10.f));
            }
            else
            {
                return ImVec2(30, 10);
            }
        }

        virtual void Render()
        {
            HandleLayout();
            RenderButton();        // 专注于渲染
            RenderBackGround();
            RenderChild();
        }

        virtual std::unordered_set<PropertyInfo, PropertyInfo::Hasher> GetProperties() override
        {
            auto baseProps = ImPanelWidget::GetProperties();

            // 添加按钮特有属性
            baseProps.insert(
                { "TooltipText", PropertyType::String, "Behavior",
                 [this](void* v) { m_TooltipText = *static_cast<std::string*>(v); },
                 [this]() -> void* { return &m_TooltipText; } }
            );

            // 添加样式结构体属性
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
                { "FocusedStyle", PropertyType::Struct, "Style",
                 [this](void* v) { m_FocusedStyle = *(ButtonStateStyle*)v; },
                 [this]() -> void* { return const_cast<ButtonStateStyle*>(&m_FocusedStyle); } }
            );

            return baseProps;
        }

        virtual std::string GetRegisterTypeName() override { return "ImButton"; }

        virtual ImWidget* CopyWidget() override
        {
            return new ImButton(*this);
        }
    };
}