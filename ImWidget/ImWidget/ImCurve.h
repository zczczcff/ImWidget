#pragma once

#include "ImWidget.h"
#include <vector>
#include <functional>
#include <algorithm>
#include <unordered_map>

namespace ImGuiWidget
{
    class ImCurve : public ImWidget
    {
    public:
        struct CurveData
        {
            std::vector<ImVec2> points;     // 曲线上的点
            ImU32 color;                    // 曲线颜色
            float thickness;                // 曲线粗细
            std::string label;              // 曲线标签
            size_t dataHash = 0;            // 数据哈希值，用于检测变化

            CurveData() : color(IM_COL32(255, 0, 0, 255)), thickness(1.0f) {}

            // 计算数据哈希值
            size_t CalculateHash() const
            {
                size_t h = std::hash<ImU32>()(color);
                h ^= std::hash<float>()(thickness) + 0x9e3779b9 + (h << 6) + (h >> 2);
                for (const auto& p : points)
                {
                    h ^= std::hash<float>()(p.x) + 0x9e3779b9 + (h << 6) + (h >> 2);
                    h ^= std::hash<float>()(p.y) + 0x9e3779b9 + (h << 6) + (h >> 2);
                }
                return h;
            }
        };

        struct MappedCurve
        {
            std::vector<ImVec2> mappedPoints;   // 映射后的点
            size_t dataHash = 0;                 // 原始数据的哈希值
            ImRect bounds;                       // 映射后点的边界
        };

    protected:
        // 坐标轴设置
        float m_XMin = 0.0f;
        float m_XMax = 10.0f;
        float m_YMin = -1.0f;
        float m_YMax = 1.0f;

        // 坐标轴原点位置
        float m_OriginX = 0.0f;
        float m_OriginY = 0.0f;

        // 坐标轴名称
        std::string m_XAxisName = "X";
        std::string m_YAxisName = "Y";

        // 新增：坐标轴名称颜色
        ImU32 m_XAxisNameColor = IM_COL32(200, 200, 200, 255);
        ImU32 m_YAxisNameColor = IM_COL32(200, 200, 200, 255);

        // 新增：坐标轴名称偏移
        ImVec2 m_XAxisNameOffset = ImVec2(0.0f, 0.0f);
        ImVec2 m_YAxisNameOffset = ImVec2(0.0f, 0.0f);

        // 坐标轴刻度设置
        float m_XTickInterval = 1.0f;
        float m_YTickInterval = 0.5f;

        // 外观设置
        ImU32 m_AxisColor = IM_COL32(200, 200, 200, 255);
        ImU32 m_GridColor = IM_COL32(100, 100, 100, 100);
        ImU32 m_BackgroundColor = IM_COL32(40, 40, 40, 255);
        float m_AxisThickness = 2.0f;
        float m_GridThickness = 1.0f;

        bool m_ShowGrid = true;
        bool m_ShowLabels = true;

        // 曲线数据
        std::vector<CurveData> m_Curves;

        // 映射缓存
        std::unordered_map<size_t, MappedCurve> m_MappedCurvesCache;
        ImVec2 m_LastSize = ImVec2(0, 0);
        ImVec2 m_LastPosition = ImVec2(0, 0);
        float m_LastXMin = 0.0f;
        float m_LastXMax = 0.0f;
        float m_LastYMin = 0.0f;
        float m_LastYMax = 0.0f;
        float m_LastOriginX = 0.0f;
        float m_LastOriginY = 0.0f;
        bool m_CacheDirty = true;  // 缓存是否过期

        // 计算当前状态的哈希值，用于检测变化
        size_t CalculateStateHash() const
        {
            size_t h = std::hash<float>()(m_XMin);
            h ^= std::hash<float>()(m_XMax) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<float>()(m_YMin) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<float>()(m_YMax) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<float>()(Position.x) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<float>()(Position.y) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<float>()(Size.x) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<float>()(Size.y) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<float>()(m_OriginX) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<float>()(m_OriginY) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }

        // 将数据坐标映射到屏幕坐标（已包含控件位置）
        ImVec2 MapToPixel(float x, float y) const
        {
            // 计算归一化坐标 (0.0-1.0范围)
            float normX = (x - m_XMin) / (m_XMax - m_XMin);
            float normY = (y - m_YMin) / (m_YMax - m_YMin);

            // 转换为屏幕坐标（已包含控件位置）
            return ImVec2(
                Position.x + normX * Size.x,
                Position.y + Size.y - normY * Size.y   // 翻转y轴
            );
        }

        // 检查是否需要更新缓存
        bool IsCacheDirty() const
        {
            return m_CacheDirty ||
                Size.x != m_LastSize.x ||
                Size.y != m_LastSize.y ||
                Position.x != m_LastPosition.x ||
                Position.y != m_LastPosition.y ||
                m_XMin != m_LastXMin ||
                m_XMax != m_LastXMax ||
                m_YMin != m_LastYMin ||
                m_YMax != m_LastYMax ||
                m_OriginX != m_LastOriginX ||
                m_OriginY != m_LastOriginY;
        }

        // 更新映射缓存
        void UpdateMappedCurves()
        {
            m_MappedCurvesCache.clear();

            for (const auto& curve : m_Curves)
            {
                MappedCurve mappedCurve;
                mappedCurve.dataHash = curve.CalculateHash();

                // 计算映射点
                for (const auto& point : curve.points)
                {
                    mappedCurve.mappedPoints.push_back(MapToPixel(point.x, point.y));
                }

                // 计算边界
                if (!mappedCurve.mappedPoints.empty())
                {
                    ImVec2 min = mappedCurve.mappedPoints[0];
                    ImVec2 max = mappedCurve.mappedPoints[0];

                    for (const auto& p : mappedCurve.mappedPoints)
                    {
                        min.x = std::min(min.x, p.x);
                        min.y = std::min(min.y, p.y);
                        max.x = std::max(max.x, p.x);
                        max.y = std::max(max.y, p.y);
                    }

                    mappedCurve.bounds = ImRect(min, max);
                }

                m_MappedCurvesCache[curve.CalculateHash()] = mappedCurve;
            }

            // 更新最后状态
            m_LastSize = Size;
            m_LastPosition = Position;
            m_LastXMin = m_XMin;
            m_LastXMax = m_XMax;
            m_LastYMin = m_YMin;
            m_LastYMax = m_YMax;
            m_LastOriginX = m_OriginX;
            m_LastOriginY = m_OriginY;
            m_CacheDirty = false;
        }

        // 获取映射后的曲线（如果缓存存在）
        const MappedCurve* GetMappedCurve(const CurveData& curve) const
        {
            size_t hash = curve.CalculateHash();
            auto it = m_MappedCurvesCache.find(hash);
            if (it != m_MappedCurvesCache.end() && it->second.dataHash == hash)
            {
                return &it->second;
            }
            return nullptr;
        }

        // 绘制坐标轴和网格
        void DrawAxesAndGrid(ImDrawList* drawList)
        {
            // 绘制背景
            drawList->AddRectFilled(Position, Position + Size, m_BackgroundColor);

            // 网格线（先画网格，再画坐标轴，这样坐标轴在最上面）
            if (m_ShowGrid) {
                // 水平网格线（对应y轴刻度）
                for (float y = m_YMin; y <= m_YMax; y += m_YTickInterval)
                {
                    ImVec2 start = MapToPixel(m_XMin, y);
                    ImVec2 end = MapToPixel(m_XMax, y);
                    drawList->AddLine(start, end, m_GridColor, m_GridThickness);
                }

                // 垂直网格线（对应x轴刻度）
                for (float x = m_XMin; x <= m_XMax; x += m_XTickInterval)
                {
                    ImVec2 start = MapToPixel(x, m_YMin);
                    ImVec2 end = MapToPixel(x, m_YMax);
                    drawList->AddLine(start, end, m_GridColor, m_GridThickness);
                }
            }

            // 坐标轴
            // X轴（使用原点Y坐标）
            ImVec2 xAxisStart = MapToPixel(m_XMin, m_OriginY);
            ImVec2 xAxisEnd = MapToPixel(m_XMax, m_OriginY);
            drawList->AddLine(xAxisStart, xAxisEnd, m_AxisColor, m_AxisThickness);

            // Y轴（使用原点X坐标）
            ImVec2 yAxisStart = MapToPixel(m_OriginX, m_YMin);
            ImVec2 yAxisEnd = MapToPixel(m_OriginX, m_YMax);
            drawList->AddLine(yAxisStart, yAxisEnd, m_AxisColor, m_AxisThickness);

            // 绘制刻度
            if (m_ShowLabels)
            {
                ImGuiContext& g = *GImGui;
                ImFont* font = g.Font;
                float fontSize = g.FontSize;

                // X轴刻度
                for (float x = m_XMin; x <= m_XMax; x += m_XTickInterval)
                {
                    ImVec2 tickPos = MapToPixel(x, m_OriginY);

                    // 刻度线
                    drawList->AddLine(ImVec2(tickPos.x, tickPos.y - 5),
                        ImVec2(tickPos.x, tickPos.y + 5),
                        m_AxisColor, 1.0f);

                    // 刻度值（确保在控件范围内）
                    char label[32];
                    sprintf(label, "%.1f", x);
                    ImVec2 labelSize = ImGui::CalcTextSize(label);

                    // 计算标签位置（在轴上方或下方）
                    float labelY = (m_OriginY > (m_YMin + m_YMax) * 0.5f) ?
                        tickPos.y - 10 - labelSize.y : // 在轴上方
                        tickPos.y + 10;                // 在轴下方

                    // 确保标签在控件范围内
                    labelY = std::clamp(labelY,
                        Position.y,
                        Position.y + Size.y - labelSize.y);

                    // 水平居中
                    ImVec2 labelPos = ImVec2(
                        tickPos.x - labelSize.x * 0.5f,
                        labelY
                    );

                    // 确保水平位置在控件范围内
                    labelPos.x = std::clamp(labelPos.x,
                        Position.x,
                        Position.x + Size.x - labelSize.x);

                    drawList->AddText(labelPos, m_AxisColor, label);
                }

                // Y轴刻度
                for (float y = m_YMin; y <= m_YMax; y += m_YTickInterval)
                {
                    ImVec2 tickPos = MapToPixel(m_OriginX, y);

                    // 刻度线
                    drawList->AddLine(ImVec2(tickPos.x - 5, tickPos.y),
                        ImVec2(tickPos.x + 5, tickPos.y),
                        m_AxisColor, 1.0f);

                    // 刻度值（确保在控件范围内）
                    char label[32];
                    sprintf(label, "%.1f", y);
                    ImVec2 labelSize = ImGui::CalcTextSize(label);

                    // 计算标签位置（在轴左侧或右侧）
                    float labelX = (m_OriginX > (m_XMin + m_XMax) * 0.5f) ?
                        tickPos.x + 10 :               // 在轴右侧
                        tickPos.x - 10 - labelSize.x;   // 在轴左侧

                    // 确保标签在控件范围内
                    labelX = std::clamp(labelX,
                        Position.x,
                        Position.x + Size.x - labelSize.x);

                    // 垂直居中
                    ImVec2 labelPos = ImVec2(
                        labelX,
                        tickPos.y - labelSize.y * 0.5f
                    );

                    // 确保垂直位置在控件范围内
                    labelPos.y = std::clamp(labelPos.y,
                        Position.y,
                        Position.y + Size.y - labelSize.y);

                    drawList->AddText(labelPos, m_AxisColor, label);
                }
            }

            // 绘制坐标轴名称
            if (!m_XAxisName.empty()) {
                // X轴名称（在X轴末端）
                ImVec2 namePos = MapToPixel(m_XMax, m_OriginY);
                ImVec2 nameSize = ImGui::CalcTextSize(m_XAxisName.c_str());

                // 调整位置（在轴下方）
                namePos.y += 20;
                namePos.x -= nameSize.x * 0.5f;

                // 确保在控件范围内
                namePos.x = std::clamp(namePos.x,
                    Position.x,
                    Position.x + Size.x - nameSize.x);
                namePos.y = std::clamp(namePos.y,
                    Position.y,
                    Position.y + Size.y - nameSize.y);

                // 应用偏移
                namePos += m_XAxisNameOffset;

                drawList->AddText(namePos, m_XAxisNameColor, m_XAxisName.c_str());
            }

            if (!m_YAxisName.empty()) {
                ImVec2 namePos = MapToPixel(m_OriginX, m_YMax);
                ImVec2 nameSize = ImGui::CalcTextSize(m_YAxisName.c_str());

                // 调整位置（在轴左侧）
                namePos.x -= nameSize.x + 20;
                namePos.y -= nameSize.y * 0.5f;

                // 确保在控件范围内
                namePos.x = std::clamp(namePos.x,
                    Position.x,
                    Position.x + Size.x - nameSize.x);
                namePos.y = std::clamp(namePos.y,
                    Position.y,
                    Position.y + Size.y - nameSize.y);

                // 应用偏移
                namePos += m_YAxisNameOffset;

                drawList->AddText(namePos, m_YAxisNameColor, m_YAxisName.c_str());
            }
        }

        // 绘制图例
        void DrawLegend(ImDrawList* drawList)
        {
            if (m_Curves.empty()) return;

            // 图例位置：右上角
            const float padding = 10.0f;
            const float lineHeight = 20.0f;
            const float sampleWidth = 40.0f;
            const float sampleLineHeight = 2.0f;

            ImVec2 legendPos = Position + ImVec2(Size.x - padding, padding);
            ImVec2 currentPos = legendPos;

            for (const auto& curve : m_Curves)
            {
                // 绘制曲线颜色示例
                ImVec2 lineStart = currentPos - ImVec2(sampleWidth, -lineHeight / 2 + sampleLineHeight / 2);
                ImVec2 lineEnd = lineStart + ImVec2(sampleWidth, 0);
                drawList->AddLine(lineStart, lineEnd, curve.color, curve.thickness);

                // 绘制标签
                ImVec2 textPos = currentPos - ImVec2(sampleWidth + padding, -lineHeight / 2) -
                    ImVec2(0, ImGui::GetFontSize() / 2);
                drawList->AddText(textPos, IM_COL32_WHITE, curve.label.c_str());

                currentPos.y += lineHeight;
            }
        }

    public:
        ImCurve(const std::string& WidgetName) : ImWidget(WidgetName)
        {
            // 默认范围
            SetAxisRange(0.0f, 10.0f, -1.0f, 1.0f);
            // 默认名称颜色与坐标轴颜色相同
            m_XAxisNameColor = m_AxisColor;
            m_YAxisNameColor = m_AxisColor;
        }

        // 设置坐标轴范围
        void SetAxisRange(float xMin, float xMax, float yMin, float yMax)
        {
            if (m_XMin != xMin || m_XMax != xMax || m_YMin != yMin || m_YMax != yMax)
            {
                m_XMin = xMin;
                m_XMax = xMax;
                m_YMin = yMin;
                m_YMax = yMax;
                m_CacheDirty = true;
            }
        }

        // 设置原点位置
        void SetOrigin(float x, float y)
        {
            if (m_OriginX != x || m_OriginY != y)
            {
                m_OriginX = x;
                m_OriginY = y;
                m_CacheDirty = true;
            }
        }

        // 设置坐标轴名称
        void SetAxisNames(const std::string& xName, const std::string& yName)
        {
            m_XAxisName = xName;
            m_YAxisName = yName;
        }

        // 设置坐标轴名称颜色
        void SetAxisNameColors(ImU32 xColor, ImU32 yColor)
        {
            m_XAxisNameColor = xColor;
            m_YAxisNameColor = yColor;
        }

        // 设置坐标轴名称偏移
        void SetAxisNameOffsets(const ImVec2& xOffset, const ImVec2& yOffset)
        {
            m_XAxisNameOffset = xOffset;
            m_YAxisNameOffset = yOffset;
        }

        // 设置刻度间隔
        void SetTickInterval(float xInterval, float yInterval)
        {
            m_XTickInterval = xInterval;
            m_YTickInterval = yInterval;
            // 刻度间隔不影响映射，不需要设置缓存过期
        }

        // 添加曲线
        void AddCurve(const std::vector<ImVec2>& points, ImU32 color, float thickness,
            const std::string& label = "")
        {
            CurveData curve;
            curve.points = points;
            curve.color = color;
            curve.thickness = thickness;
            curve.label = label;
            m_Curves.push_back(curve);
            m_CacheDirty = true;
        }

        // 清除所有曲线
        void ClearCurves()
        {
            m_Curves.clear();
            m_MappedCurvesCache.clear();
            m_CacheDirty = true;
        }

        // 设置坐标轴颜色
        void SetAxisColor(ImU32 color) {
            m_AxisColor = color;
            // 如果名称颜色与坐标轴颜色相同，则同时更新名称颜色
            if (m_XAxisNameColor == m_AxisColor) {
                m_XAxisNameColor = color;
            }
            if (m_YAxisNameColor == m_AxisColor) {
                m_YAxisNameColor = color;
            }
        }

        // 设置网格颜色
        void SetGridColor(ImU32 color) { m_GridColor = color; }

        // 设置背景颜色
        void SetBackgroundColor(ImU32 color) { m_BackgroundColor = color; }

        // 设置是否显示网格
        void ShowGrid(bool show) { m_ShowGrid = show; }

        // 设置是否显示刻度标签
        void ShowLabels(bool show) { m_ShowLabels = show; }

        // 重写位置设置方法
        virtual void SetPosition(ImVec2 Pos) override
        {
            if (Position.x != Pos.x || Position.y != Pos.y)
            {
                Position = Pos;
                m_CacheDirty = true; // 位置变化需要更新缓存
            }
        }

        // 重写尺寸设置方法
        virtual void SetSize(ImVec2 size) override
        {
            if (Size.x != size.x || Size.y != size.y)
            {
                Size = size;
                m_CacheDirty = true; // 尺寸变化需要更新缓存
            }
        }

        virtual ImVec2 GetMinSize() override
        {
            // 最小尺寸，比如200x200
            return ImVec2(200, 200);
        }

        virtual void Render() override
        {
            if (Size.x <= 0 || Size.y <= 0) return;

            // 检查并更新映射缓存
            if (IsCacheDirty())
            {
                UpdateMappedCurves();
            }

            ImDrawList* drawList = ImGui::GetWindowDrawList();

            DrawAxesAndGrid(drawList);

            // 绘制每条曲线
            for (const auto& curve : m_Curves)
            {
                const MappedCurve* mappedCurve = GetMappedCurve(curve);
                if (!mappedCurve || mappedCurve->mappedPoints.size() < 2) continue;

                // 绘制曲线
                drawList->AddPolyline(mappedCurve->mappedPoints.data(),
                    (int)mappedCurve->mappedPoints.size(),
                    curve.color, ImDrawFlags_None, curve.thickness);
            }

            DrawLegend(drawList);
        }

        // 属性系统
        virtual std::unordered_set<PropertyInfo, PropertyInfo::Hasher> GetProperties() override
        {
            auto props = ImWidget::GetProperties();

            // 添加坐标轴范围
            props.insert({
                "XMin", PropertyType::Float, "Axis",
                [this](void* val) { SetAxisRange(*static_cast<float*>(val), m_XMax, m_YMin, m_YMax); },
                [this]() { return static_cast<void*>(&m_XMin); }
                });

            props.insert({
                "XMax", PropertyType::Float, "Axis",
                [this](void* val) { SetAxisRange(m_XMin, *static_cast<float*>(val), m_YMin, m_YMax); },
                [this]() { return static_cast<void*>(&m_XMax); }
                });

            props.insert({
                "YMin", PropertyType::Float, "Axis",
                [this](void* val) { SetAxisRange(m_XMin, m_XMax, *static_cast<float*>(val), m_YMax); },
                [this]() { return static_cast<void*>(&m_YMin); }
                });

            props.insert({
                "YMax", PropertyType::Float, "Axis",
                [this](void* val) { SetAxisRange(m_XMin, m_XMax, m_YMin, *static_cast<float*>(val)); },
                [this]() { return static_cast<void*>(&m_YMax); }
                });

            // 原点位置
            props.insert({
                "OriginX", PropertyType::Float, "Axis",
                [this](void* val) { SetOrigin(*static_cast<float*>(val), m_OriginY); },
                [this]() { return static_cast<void*>(&m_OriginX); }
                });

            props.insert({
                "OriginY", PropertyType::Float, "Axis",
                [this](void* val) { SetOrigin(m_OriginX, *static_cast<float*>(val)); },
                [this]() { return static_cast<void*>(&m_OriginY); }
                });

            // 坐标轴名称
            props.insert({
                "XAxisName", PropertyType::String, "Axis Labels",
                [this](void* val) { m_XAxisName = *static_cast<std::string*>(val); },
                [this]() { return static_cast<void*>(&m_XAxisName); }
                });

            props.insert({
                "YAxisName", PropertyType::String, "Axis Labels",
                [this](void* val) { m_YAxisName = *static_cast<std::string*>(val); },
                [this]() { return static_cast<void*>(&m_YAxisName); }
                });

            // 坐标轴名称颜色
            props.insert({
                "XAxisNameColor", PropertyType::Color, "Axis Labels",
                [this](void* val) { m_XAxisNameColor = *static_cast<ImU32*>(val); },
                [this]() { return static_cast<void*>(&m_XAxisNameColor); }
                });

            props.insert({
                "YAxisNameColor", PropertyType::Color, "Axis Labels",
                [this](void* val) { m_YAxisNameColor = *static_cast<ImU32*>(val); },
                [this]() { return static_cast<void*>(&m_YAxisNameColor); }
                });

            // 坐标轴名称偏移
            props.insert({
                "XAxisNameOffset", PropertyType::Vec2, "Axis Labels",
                [this](void* val) { m_XAxisNameOffset = *static_cast<ImVec2*>(val); },
                [this]() { return static_cast<void*>(&m_XAxisNameOffset); }
                });

            props.insert({
                "YAxisNameOffset", PropertyType::Vec2, "Axis Labels",
                [this](void* val) { m_YAxisNameOffset = *static_cast<ImVec2*>(val); },
                [this]() { return static_cast<void*>(&m_YAxisNameOffset); }
                });

            // 刻度间隔
            props.insert({
                "XTickInterval", PropertyType::Float, "Axis",
                [this](void* val) { m_XTickInterval = *static_cast<float*>(val); },
                [this]() { return static_cast<void*>(&m_XTickInterval); }
                });

            props.insert({
                "YTickInterval", PropertyType::Float, "Axis",
                [this](void* val) { m_YTickInterval = *static_cast<float*>(val); },
                [this]() { return static_cast<void*>(&m_YTickInterval); }
                });

            // 显示选项
            props.insert({
                "ShowGrid", PropertyType::Bool, "Display",
                [this](void* val) { m_ShowGrid = *static_cast<bool*>(val); },
                [this]() { return static_cast<void*>(&m_ShowGrid); }
                });

            props.insert({
                "ShowLabels", PropertyType::Bool, "Display",
                [this](void* val) { m_ShowLabels = *static_cast<bool*>(val); },
                [this]() { return static_cast<void*>(&m_ShowLabels); }
                });

            // 颜色
            props.insert({
                "AxisColor", PropertyType::Color, "Color",
                [this](void* val) { m_AxisColor = *static_cast<ImU32*>(val); },
                [this]() { return static_cast<void*>(&m_AxisColor); }
                });

            props.insert({
                "GridColor", PropertyType::Color, "Color",
                [this](void* val) { m_GridColor = *static_cast<ImU32*>(val); },
                [this]() { return static_cast<void*>(&m_GridColor); }
                });

            props.insert({
                "BackgroundColor", PropertyType::Color, "Color",
                [this](void* val) { m_BackgroundColor = *static_cast<ImU32*>(val); },
                [this]() { return static_cast<void*>(&m_BackgroundColor); }
                });

            return props;
        }

        virtual std::string GetRegisterTypeName() override { return "ImCurve"; }

        virtual ImWidget* CopyWidget() override
        {
            return new ImCurve(*this);
        }

        // 标记缓存为过期（当外部修改曲线数据时调用）
        void MarkCacheDirty()
        {
            m_CacheDirty = true;
        }

    private:
        // 新增：存储上次原点位置用于缓存检测
        //float m_LastOriginX = 0.0f;
        //float m_LastOriginY = 0.0f;
    };
}