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
        ImU32 m_FillColor = IM_COL32(255, 255, 255, 255); // 填充颜色
        bool m_HasBorder = false;               // 是否显示边框
        ImU32 m_BorderColor = IM_COL32(0, 0, 0, 255); // 边框颜色
        float m_BorderThickness = 1.0f;          // 边框粗细

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
        // 属性系统实现
        virtual std::unordered_set<PropertyInfo, PropertyInfo::Hasher> GetProperties() override
        {
            auto props = ImWidget::GetProperties();

            // 图形类型
            props.insert({
                "Type", PropertyType::Enum, "Appearance",
                [this](void* v) 
                {
                    std::string str = *static_cast<std::string*>(v);
                    if (str == "Triangle") m_Type = FigureType::TRIANGLE;
                    else if (str == "Square") m_Type = FigureType::SQUARE;
                    else if (str == "Circle") m_Type = FigureType::CIRCLE;
                },
                [this]() -> void* 
                {
                    static std::vector<std::string> options;
                    options = {"Triangle", "Square", "Circle"}; // 枚举选项

                    // 添加当前选中项
                    switch (m_Type) 
                    {
                        case FigureType::TRIANGLE: options.push_back("Triangle"); break;
                        case FigureType::SQUARE: options.push_back("Square"); break;
                        case FigureType::CIRCLE: options.push_back("Circle"); break;
                    }
                    return static_cast<void*>(&options);
                }
                });

            // 尺寸系数
            props.insert({
                "SizeRatio", PropertyType::Float, "Appearance",
                [this](void* v) {
                    this->m_SizeRatio = ImClamp(*static_cast<float*>(v), 0.0f, 1.0f);
                },
                [this]() -> void* { return &this->m_SizeRatio; }
                });

            // 填充颜色
            props.insert({
                "FillColor", PropertyType::Color, "Colors",
                [this](void* v) { this->m_FillColor = *static_cast<ImU32*>(v); },
                [this]() -> void* { return &this->m_FillColor; }
                });

            // 边框设置
            props.insert({
                "HasBorder", PropertyType::Bool, "Border",
                [this](void* v) { this->m_HasBorder = *static_cast<bool*>(v); },
                [this]() -> void* { return &this->m_HasBorder; }
                });

            props.insert({
                "BorderColor", PropertyType::Color, "Border",
                [this](void* v) { this->m_BorderColor = *static_cast<ImU32*>(v); },
                [this]() -> void* { return &this->m_BorderColor; }
                });

            props.insert({
                "BorderThickness", PropertyType::Float, "Border",
                [this](void* v) { this->m_BorderThickness = *static_cast<float*>(v); },
                [this]() -> void* { return &this->m_BorderThickness; }
                });

            return props;
        }

        virtual std::string GetRegisterTypeName() override { return "ImSimpleFigure"; }
    };
}