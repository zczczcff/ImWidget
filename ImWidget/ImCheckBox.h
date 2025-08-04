#pragma once
#include "ImWidget.h"
#include <functional>

namespace ImGuiWidget
{
    class ImCheckBox : public ImWidget
    {
    private:
        bool m_Checked = false;
        ImU32 m_BoxColor = IM_COL32(100, 100, 100, 255);
        ImU32 m_CheckColor = IM_COL32(0, 0, 0, 255);
        ImU32 m_BackgroundColor = IM_COL32(255, 255, 255, 255);
        float m_BoxRounding = 3.0f;
        float m_BorderThickness = 1.5f;

        std::function<void(bool)> OnToggled;

    public:
        ImCheckBox(const std::string& WidgetName)
            : ImWidget(WidgetName)
        {}

        void SetChecked(bool checked) { m_Checked = checked; }
        bool IsChecked() const { return m_Checked; }

        void SetBoxColor(ImU32 color) { m_BoxColor = color; }
        void SetCheckColor(ImU32 color) { m_CheckColor = color; }
        void SetBackgroundColor(ImU32 color) { m_BackgroundColor = color; }
        void SetBoxRounding(float rounding) { m_BoxRounding = rounding; }
        void SetBorderThickness(float thickness) { m_BorderThickness = thickness; }

        void SetOnToggled(std::function<void(bool)> callback) {
            OnToggled = callback;
        }

        virtual ImVec2 GetMinSize() override {
            // 只需要考虑选框本身的尺寸
            return ImVec2(20.0f, 20.0f);
        }

        virtual void Render() override {
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            ImGuiIO& io = ImGui::GetIO();

            // 计算选框位置和大小 - 居中显示
            float boxSize = ImMin(Size.x, Size.y) * 0.8f;
            ImVec2 boxMin = Position + (Size - ImVec2(boxSize, boxSize)) * 0.5f;
            ImVec2 boxMax = boxMin + ImVec2(boxSize, boxSize);

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
                m_BoxColor,
                m_BoxRounding,
                0,
                m_BorderThickness
            );

            // 绘制勾选标记
            if (m_Checked) {
                float padding = boxSize * 0.2f;
                ImVec2 p1 = boxMin + ImVec2(padding, boxSize * 0.4f);
                ImVec2 p2 = boxMin + ImVec2(boxSize * 0.4f, boxSize - padding);
                ImVec2 p3(boxMax.x - padding, boxMin.y + padding);

                drawList->AddLine(p1, p2, m_CheckColor, 1.0f);
                drawList->AddLine(p2, p3, m_CheckColor, 1.0f);
            }

            // 处理点击事件
            bool clicked = ImGui::IsMouseClicked(0);
            bool isHovered = ImRect(Position, Position + Size).Contains(io.MousePos);

            if (clicked && isHovered) {
                m_Checked = !m_Checked;
                if (OnToggled) {
                    OnToggled(m_Checked);
                }
            }
        }

        virtual std::unordered_set<PropertyInfo, PropertyInfo::Hasher> GetProperties() override {
            std::unordered_set<PropertyInfo, PropertyInfo::Hasher> props = {
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
                {"BoxRounding", PropertyType::Float, "Style",
                    [this](void* v) { m_BoxRounding = *static_cast<float*>(v); },
                    [this]() -> void* { return &m_BoxRounding; }},
                {"BorderThickness", PropertyType::Float, "Style",
                    [this](void* v) { m_BorderThickness = *static_cast<float*>(v); },
                    [this]() -> void* { return &m_BorderThickness; }}
            };
            return props;
        }
        virtual std::string GetRegisterTypeName()override { return "ImCheckBox"; }
    };
}