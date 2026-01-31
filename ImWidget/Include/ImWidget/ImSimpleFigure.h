#pragma once
#include "ImWidget.h"

namespace ImGuiWidget
{
    class ImSimpleFigure : public ImWidget
    {
    public:
        // 图形类型枚举
        enum class FigureType
        {
            TRIANGLE, // 等边三角形
            SQUARE,   // 正方形
            CIRCLE    // 圆
        };

    protected:
        FigureType m_Type = FigureType::SQUARE; // 当前图形类型
        float m_SizeRatio = 0.8f;               // 尺寸系数 (0.0~1.0)
        ImU32 m_FillColor = IM_COL32(180, 180, 180, 255); // 填充颜色
        bool m_HasBorder = false;               // 是否显示边框
        ImU32 m_BorderColor = IM_COL32(0, 0, 0, 255); // 边框颜色
        float m_BorderThickness = 1.0f;          // 边框粗细
        float m_MinSize = 10.f;
    public:
        ImSimpleFigure(const std::string& WidgetName) : ImWidget(WidgetName) {}

        virtual void Render() override
        {
            ImGuiWindow* window = ImGui::GetCurrentWindow();
            if (!window) return;

            const ImVec2 center(
                Position.x + Size.x * 0.5f,
                Position.y + Size.y * 0.5f
            );

            // 计算实际图形尺寸（基于控件较小边的百分比）
            const float minSize = ImMin(Size.x, Size.y);
            const float graphicSize = minSize * m_SizeRatio;
            const float radius = graphicSize * 0.5f;

            switch (m_Type)
            {
            case FigureType::TRIANGLE:
                RenderTriangle(window, center, radius);
                break;

            case FigureType::SQUARE:
                RenderSquare(window, center, radius);
                break;

            case FigureType::CIRCLE:
                RenderCircle(window, center, radius);
                break;
            }
        }

        virtual ImVec2 GetMinSize() { return ImVec2(m_MinSize, m_MinSize); }

        void SetFillColor(ImU32 NewColor)
        {
            m_FillColor = NewColor;
        }
    protected:
        // 渲染等边三角形
        void RenderTriangle(ImGuiWindow* window, const ImVec2& center, float radius)
        {
            // 计算三角形顶点（顶部、左下、右下）
            const float height = radius * ImSqrt(3.0f);
            const ImVec2 top(center.x, center.y - radius);
            const ImVec2 bottomLeft(center.x - radius, center.y + height * 0.5f);
            const ImVec2 bottomRight(center.x + radius, center.y + height * 0.5f);

            // 填充三角形
            window->DrawList->AddTriangleFilled(top, bottomLeft, bottomRight, m_FillColor);

            // 绘制边框
            if (m_HasBorder)
            {
                window->DrawList->AddTriangle(top, bottomLeft, bottomRight,
                    m_BorderColor, m_BorderThickness);
            }
        }

        // 渲染正方形
        void RenderSquare(ImGuiWindow* window, const ImVec2& center, float radius)
        {
            const ImVec2 halfSize(radius, radius);
            const ImRect rect(
                ImVec2(center.x - halfSize.x, center.y - halfSize.y),
                ImVec2(center.x + halfSize.x, center.y + halfSize.y)
            );

            // 填充正方形
            window->DrawList->AddRectFilled(rect.Min, rect.Max, m_FillColor);

            // 绘制边框
            if (m_HasBorder)
            {
                window->DrawList->AddRect(rect.Min, rect.Max,
                    m_BorderColor, 0.0f, 0, m_BorderThickness);
            }
        }

        // 渲染圆形
        void RenderCircle(ImGuiWindow* window, const ImVec2& center, float radius)
        {
            // 填充圆形
            window->DrawList->AddCircleFilled(center, radius, m_FillColor);

            // 绘制边框
            if (m_HasBorder)
            {
                window->DrawList->AddCircle(center, radius,
                    m_BorderColor, 0, m_BorderThickness);
            }
        }

    public:
        virtual std::string GetRegisterTypeName() override { return "ImSimpleFigure"; }

        virtual ImWidget* CopyWidget()
        {
            return new ImSimpleFigure(*this);
        }

        DECLARE_IMOBJECT(ImSimpleFigure, ImWidget)
        registrar
            .RegisterOptionalProperty(PropertyType::Enum, "Type", &ImSimpleFigure::m_Type,
                { "Triangle", "Square", "Circle" }, u8"图形类型")
            .RegisterProperty(PropertyType::Float, "SizeRatio", &ImSimpleFigure::m_SizeRatio, u8"尺寸系数 (0.0~1.0)")
            .RegisterProperty(PropertyType::Float, "MinSize", &ImSimpleFigure::m_MinSize, u8"最小尺寸")
            .RegisterProperty(PropertyType::Color, "FillColor", &ImSimpleFigure::m_FillColor, u8"填充颜色")
            .RegisterProperty(PropertyType::Bool, "HasBorder", &ImSimpleFigure::m_HasBorder, u8"是否显示边框")
            .RegisterProperty(PropertyType::Color, "BorderColor", &ImSimpleFigure::m_BorderColor, u8"边框颜色")
            .RegisterProperty(PropertyType::Float, "BorderThickness", &ImSimpleFigure::m_BorderThickness, u8"边框粗细");    
        END_DECLARE_IMOBJECT()
    };
}