#pragma once
#include "ImWidget.h"
#include <functional>
#include "ImEvent/ImMouseEvent.h"
#include "ImEvent/ImFocusEvent.h"

namespace ImGuiWidget
{
    class ImCheckBox : public ImWidget
    {
    private:
        bool m_Checked = false;
        bool m_IsHovered = false;
        bool m_IsPressed = false;

        ImU32 m_BoxColor = IM_COL32(100, 100, 100, 255);
        ImU32 m_CheckColor = IM_COL32(0, 0, 0, 255);
        ImU32 m_BackgroundColor = IM_COL32(255, 255, 255, 255);
        ImU32 m_HoveredBoxColor = IM_COL32(150, 150, 150, 255);
        ImU32 m_PressedBoxColor = IM_COL32(50, 50, 50, 255);

        float m_BoxRounding = 3.0f;
        float m_BorderThickness = 1.5f;

        std::function<void(bool)> OnToggled;
        std::function<void(void)> OnHoverBegin;
        std::function<void(void)> OnHoverEnd;

    protected:
        virtual void HandleEventInternal(ImEvent* event) override
        {
            if (event->IsHandled()) return;
            if (event->GetPhase() == ImEventPhase::Capture) return;

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

            case ImEventType::MouseClick:
                HandleMouseClick(event->As<ImMouseClickEvent>());
                break;

            default:
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
            event->StopPropagation();
        }

        void HandleMouseLeave(ImMouseLeaveEvent* event)
        {
            if (m_IsHovered)
            {
                m_IsHovered = false;
                m_IsPressed = false; // 离开时释放按下状态
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
                event->StopPropagation();
            }
        }

        void HandleMouseUp(ImMouseUpEvent* event)
        {
            if (event->GetButton() == ImMouseButton::Left && m_IsPressed)
            {
                m_IsPressed = false;
                event->StopPropagation();
            }
        }

        void HandleMouseClick(ImMouseClickEvent* event)
        {
            if (event->GetButton() == ImMouseButton::Left)
            {
                m_Checked = !m_Checked;
                if (OnToggled)
                {
                    OnToggled(m_Checked);
                }
                event->StopPropagation();
            }
        }

    public:
        ImCheckBox(const std::string& WidgetName)
            : ImWidget(WidgetName)
        {
            SetFocusable(true);
        }

        void SetChecked(bool checked) { m_Checked = checked; }
        bool IsChecked() const { return m_Checked; }

        void SetBoxColor(ImU32 color) { m_BoxColor = color; }
        void SetCheckColor(ImU32 color) { m_CheckColor = color; }
        void SetBackgroundColor(ImU32 color) { m_BackgroundColor = color; }
        void SetHoveredBoxColor(ImU32 color) { m_HoveredBoxColor = color; }
        void SetPressedBoxColor(ImU32 color) { m_PressedBoxColor = color; }
        void SetBoxRounding(float rounding) { m_BoxRounding = rounding; }
        void SetBorderThickness(float thickness) { m_BorderThickness = thickness; }

        void SetOnToggled(std::function<void(bool)> callback)
        {
            OnToggled = callback;
        }

        void SetOnHoverBegin(std::function<void(void)> callback) { OnHoverBegin = callback; }
        void SetOnHoverEnd(std::function<void(void)> callback) { OnHoverEnd = callback; }

        bool IsHovered() const { return m_IsHovered; }
        bool IsPressed() const { return m_IsPressed; }

        virtual ImVec2 GetMinSize() override
        {
            return ImVec2(20.0f, 20.0f);
        }

        virtual void Render() override
        {
            ImDrawList* drawList = ImGui::GetWindowDrawList();

            // 计算选框位置和大小 - 居中显示
            float boxSize = ImMin(Size.x, Size.y) * 0.8f;
            ImVec2 boxMin = Position + (Size - ImVec2(boxSize, boxSize)) * 0.5f;
            ImVec2 boxMax = boxMin + ImVec2(boxSize, boxSize);

            // 选择边框颜色
            ImU32 currentBoxColor = m_BoxColor;
            if (m_IsPressed)
            {
                currentBoxColor = m_PressedBoxColor;
            }
            else if (m_IsHovered)
            {
                currentBoxColor = m_HoveredBoxColor;
            }

            // 绘制背景
            drawList->AddRectFilled(
                boxMin,
                boxMax,
                m_BackgroundColor,
                m_BoxRounding
            );

            // 绘制边框
            drawList->AddRect(
                boxMin,
                boxMax,
                currentBoxColor,
                m_BoxRounding,
                0,
                m_BorderThickness
            );

            // 绘制勾选标记
            if (m_Checked)
            {
                float padding = boxSize * 0.2f;
                ImVec2 p1 = boxMin + ImVec2(padding, boxSize * 0.4f);
                ImVec2 p2 = boxMin + ImVec2(boxSize * 0.4f, boxSize - padding);
                ImVec2 p3(boxMax.x - padding, boxMin.y + padding);

                drawList->AddLine(p1, p2, m_CheckColor, 2.0f);
                drawList->AddLine(p2, p3, m_CheckColor, 2.0f);
            }
        }

        virtual std::unordered_set<PropertyInfo, PropertyInfo::Hasher> GetProperties() override
        {
            auto props = ImWidget::GetProperties();

            props.insert({
                {"Checked", PropertyType::Bool, "Data",
                    [this](void* v) { m_Checked = *static_cast<bool*>(v); },
                    [this]() -> void* { return &m_Checked; }},
                {"BoxColor", PropertyType::Color, "Style",
                    [this](void* v) { m_BoxColor = *static_cast<ImU32*>(v); },
                    [this]() -> void* { return &m_BoxColor; }},
                {"CheckColor", PropertyType::Color, "Style",
                    [this](void* v) { m_CheckColor = *static_cast<ImU32*>(v); },
                    [this]() -> void* { return &m_CheckColor; }},
                {"BackgroundColor", PropertyType::Color, "Style",
                    [this](void* v) { m_BackgroundColor = *static_cast<ImU32*>(v); },
                    [this]() -> void* { return &m_BackgroundColor; }},
                {"HoveredBoxColor", PropertyType::Color, "Style",
                    [this](void* v) { m_HoveredBoxColor = *static_cast<ImU32*>(v); },
                    [this]() -> void* { return &m_HoveredBoxColor; }},
                {"PressedBoxColor", PropertyType::Color, "Style",
                    [this](void* v) { m_PressedBoxColor = *static_cast<ImU32*>(v); },
                    [this]() -> void* { return &m_PressedBoxColor; }},
                {"BoxRounding", PropertyType::Float, "Style",
                    [this](void* v) { m_BoxRounding = *static_cast<float*>(v); },
                    [this]() -> void* { return &m_BoxRounding; }},
                {"BorderThickness", PropertyType::Float, "Style",
                    [this](void* v) { m_BorderThickness = *static_cast<float*>(v); },
                    [this]() -> void* { return &m_BorderThickness; }}
                });

            return props;
        }

        virtual std::string GetRegisterTypeName() override { return "ImCheckBox"; }

        virtual ImWidget* CopyWidget() override
        {
            ImCheckBox* copy = new ImCheckBox(*this);
            copy->m_Checked = m_Checked;
            copy->m_IsHovered = false;
            copy->m_IsPressed = false;
            copy->m_BoxColor = m_BoxColor;
            copy->m_CheckColor = m_CheckColor;
            copy->m_BackgroundColor = m_BackgroundColor;
            copy->m_HoveredBoxColor = m_HoveredBoxColor;
            copy->m_PressedBoxColor = m_PressedBoxColor;
            copy->m_BoxRounding = m_BoxRounding;
            copy->m_BorderThickness = m_BorderThickness;
            return copy;
        }
    };
}