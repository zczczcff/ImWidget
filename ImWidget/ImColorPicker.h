#pragma once
#include "ImWidget.h"
#include "ImInputText.h"    // 包含输入文本控件
#include "ImTextBlock.h"     // 包含文本块控件
#include <vector>
#include <functional>
#include <string>
#include <sstream>
#include <iomanip>

namespace ImGuiWidget
{
    class ImColorPicker : public ImWidget
    {
    private:
        // 颜色表示
        ImVec4 m_Color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // RGBA (0-1)

        // 布局参数
        float m_SVAreaSize = 120.0f;
        float m_HueBarHeight = 15.0f;
        float m_AlphaBarHeight = 15.0f;
        float m_PreviewHeight = 30.0f;
        float m_Spacing = 5.0f;
        float m_InputWidth = 70.0f;

        // 输入控件
        ImFloatInput m_RInput;
        ImFloatInput m_GInput;
        ImFloatInput m_BInput;
        ImFloatInput m_AInput;
        ImInputText m_HexInput;

        // 文本标签
        ImTextBlock m_RLabel;
        ImTextBlock m_GLabel;
        ImTextBlock m_BLabel;
        ImTextBlock m_ALabel;
        //ImTextBlock m_HexLabel;

        // 交互状态
        bool m_IsDraggingHue = false;
        bool m_IsDraggingSV = false;
        bool m_IsDraggingAlpha = false;

        // 回调函数
        std::function<void(const ImU32&)> OnColorChanged;

        // 颜色空间转换
        static void RGBtoHSV(float r, float g, float b, float& h, float& s, float& v)
        {
            float min = r < g ? (r < b ? r : b) : (g < b ? g : b);
            float max = r > g ? (r > b ? r : b) : (g > b ? g : b);
            float delta = max - min;

            v = max;
            if (delta < 0.00001f) {
                s = 0;
                h = 0; // undefined, maybe 0
                return;
            }

            s = delta / max;

            if (r >= max) h = (g - b) / delta;
            else if (g >= max) h = 2.0f + (b - r) / delta;
            else h = 4.0f + (r - g) / delta;

            h *= 60.0f;
            if (h < 0.0f) h += 360.0f;
        }

        static void HSVtoRGB(float h, float s, float v, float& r, float& g, float& b)
        {
            if (s <= 0.0f) {
                r = g = b = v;
                return;
            }

            h = fmodf(h, 360.0f);
            if (h < 0) h += 360.0f;
            h /= 60.0f;

            int i = (int)h;
            float f = h - i;
            float p = v * (1.0f - s);
            float q = v * (1.0f - s * f);
            float t = v * (1.0f - s * (1.0f - f));

            switch (i) {
            case 0: r = v; g = t; b = p; break;
            case 1: r = q; g = v; b = p; break;
            case 2: r = p; g = v; b = t; break;
            case 3: r = p; g = q; b = v; break;
            case 4: r = t; g = p; b = v; break;
            case 5: r = v; g = p; b = q; break;
            }
        }

        // 获取当前颜色的HSV值
        void GetHSV(float& h, float& s, float& v) const
        {
            RGBtoHSV(m_Color.x, m_Color.y, m_Color.z, h, s, v);
        }

        // 从HSV设置RGB颜色
        void SetFromHSV(float h, float s, float v)
        {
            float r, g, b;
            HSVtoRGB(h, s, v, r, g, b);
            SetColor(ImVec4(r, g, b, m_Color.w));
        }

        // 绘制HSV颜色条
        void DrawHueBar(ImDrawList* drawList, const ImVec2& pos, float width, float height)
        {
            const int segments = 6;
            const float segmentWidth = width / segments;

            ImVec2 p0, p1;
            p0.y = pos.y;
            p1.y = pos.y + height;

            // 绘制色相条 (红->黄->绿->青->蓝->紫->红)
            for (int i = 0; i < segments; i++) {
                p0.x = pos.x + i * segmentWidth;
                p1.x = pos.x + (i + 1) * segmentWidth;

                ImU32 col1, col2;
                switch (i) {
                case 0: col1 = IM_COL32(255, 0, 0, 255); col2 = IM_COL32(255, 255, 0, 255); break; // 红->黄
                case 1: col1 = IM_COL32(255, 255, 0, 255); col2 = IM_COL32(0, 255, 0, 255); break; // 黄->绿
                case 2: col1 = IM_COL32(0, 255, 0, 255); col2 = IM_COL32(0, 255, 255, 255); break; // 绿->青
                case 3: col1 = IM_COL32(0, 255, 255, 255); col2 = IM_COL32(0, 0, 255, 255); break; // 青->蓝
                case 4: col1 = IM_COL32(0, 0, 255, 255); col2 = IM_COL32(255, 0, 255, 255); break; // 蓝->紫
                case 5: col1 = IM_COL32(255, 0, 255, 255); col2 = IM_COL32(255, 0, 0, 255); break; // 紫->红
                }

                drawList->AddRectFilledMultiColor(p0, p1, col1, col2, col2, col1);
            }

            // 绘制当前色相标记
            float h, s, v;
            GetHSV(h, s, v);
            float huePos = pos.x + (h / 360.0f) * width;
            drawList->AddRectFilled(
                ImVec2(huePos - 2, pos.y - 2),
                ImVec2(huePos + 2, pos.y + height + 2),
                IM_COL32(255, 255, 255, 255)
            );
        }

        // 绘制饱和度-明度区域
        void DrawSVArea(ImDrawList* drawList, const ImVec2& pos, float size)
        {
            float h, s, v;
            GetHSV(h, s, v);

            // 修复：使用9x9网格而不是10x10
            for (int i = 0; i < 9; i++) {
                float y0 = pos.y + (size * i) / 9.0f;
                float y1 = pos.y + (size * (i + 1)) / 9.0f;

                for (int j = 0; j < 9; j++) {
                    float x0 = pos.x + (size * j) / 9.0f;
                    float x1 = pos.x + (size * (j + 1)) / 9.0f;

                    float cellS = j / 8.0f;  // 修正：使用8.0f来覆盖0-1范围
                    float cellV = 1.0f - i / 8.0f; // 修正：使用8.0f

                    float r, g, b;
                    HSVtoRGB(h, cellS, cellV, r, g, b);

                    drawList->AddRectFilled(
                        ImVec2(x0, y0),
                        ImVec2(x1, y1),
                        IM_COL32(r * 255, g * 255, b * 255, 255)
                    );
                }
            }

            // 绘制当前SV位置标记
            ImVec2 markerPos = ImVec2(
                pos.x + s * size,
                pos.y + (1.0f - v) * size
            );

            // 根据背景亮度决定标记颜色
            float luminance = 0.299f * m_Color.x + 0.587f * m_Color.y + 0.114f * m_Color.z;
            ImU32 markerColor = luminance > 0.5f ? IM_COL32(0, 0, 0, 255) : IM_COL32(255, 255, 255, 255);

            drawList->AddCircle(markerPos, 5.0f, markerColor, 12, 2.0f);
        }

        // 绘制透明度条
        void DrawAlphaBar(ImDrawList* drawList, const ImVec2& pos, float width, float height)
        {
            // 绘制棋盘背景
            const float squareSize = 8.0f;
            bool light = true;
            for (float x = 0; x < width; x += squareSize) {
                for (float y = 0; y < height; y += squareSize) {
                    ImVec2 p0 = ImVec2(pos.x + x, pos.y + y);
                    ImVec2 p1 = ImVec2(pos.x + x + squareSize, pos.y + y + squareSize);

                    drawList->AddRectFilled(
                        p0, p1,
                        light ? IM_COL32(200, 200, 200, 255) : IM_COL32(100, 100, 100, 255)
                    );

                    light = !light;
                }
                light = !light;
            }

            // 绘制透明度渐变
            drawList->AddRectFilledMultiColor(
                pos,
                ImVec2(pos.x + width, pos.y + height),
                IM_COL32(0, 0, 0, 0),
                IM_COL32(m_Color.x * 255, m_Color.y * 255, m_Color.z * 255, 255),
                IM_COL32(m_Color.x * 255, m_Color.y * 255, m_Color.z * 255, 255),
                IM_COL32(0, 0, 0, 0)

            );

            // 绘制当前透明度标记
            float alphaPos = pos.x + m_Color.w * width;
            drawList->AddRectFilled(
                ImVec2(alphaPos - 2, pos.y - 2),
                ImVec2(alphaPos + 2, pos.y + height + 2),
                IM_COL32(255, 255, 255, 255)
            );
        }

        // 处理颜色分量输入
        void DrawColorInputs(ImDrawList* drawList, const ImVec2& pos, float width)
        {
            ImGuiIO& io = ImGui::GetIO();
            ImVec2 currentPos = pos;

            // 创建临时输入区域
            ImGui::SetCursorScreenPos(currentPos);
            ImGui::PushItemWidth(width);

            // RGBA输入
            float col[4] = { m_Color.x, m_Color.y, m_Color.z, m_Color.w };
            if (ImGui::DragFloat4("##RGBA", col, 0.01f, 0.0f, 1.0f, "%.2f")) {
                SetColor(ImVec4(col[0], col[1], col[2], col[3]));
            }

            // 十六进制输入
            currentPos.y += ImGui::GetItemRectSize().y + m_Spacing;
            ImGui::SetCursorScreenPos(currentPos);

            char hexBuf[9];
            snprintf(hexBuf, sizeof(hexBuf), "%02X%02X%02X%02X",
                (int)(m_Color.x * 255),
                (int)(m_Color.y * 255),
                (int)(m_Color.z * 255),
                (int)(m_Color.w * 255));

            if (ImGui::InputText("##Hex", hexBuf, sizeof(hexBuf),
                ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase)) {
                // 尝试解析十六进制值
                unsigned int hexValue;
                if (sscanf_s(hexBuf, "%08X", &hexValue) == 1) {
                    SetColor(ImVec4(
                        ((hexValue >> 24) & 0xFF) / 255.0f,
                        ((hexValue >> 16) & 0xFF) / 255.0f,
                        ((hexValue >> 8) & 0xFF) / 255.0f,
                        (hexValue & 0xFF) / 255.0f
                    ));
                }
            }
        }

        // 处理SV区域交互
        void HandleSVInteraction(const ImVec2& areaPos, float areaSize)
        {
            if (m_IsDraggingAlpha || m_IsDraggingHue)return;
            ImGuiIO& io = ImGui::GetIO();
            ImRect areaRect(areaPos, ImVec2(areaPos.x + areaSize, areaPos.y + areaSize));

            if (ImGui::IsMouseDown(0)) {
                if (areaRect.Contains(io.MousePos)) {
                    m_IsDraggingSV = true;
                }
            }

            if (m_IsDraggingSV) {
                if (ImGui::IsMouseReleased(0)) {
                    m_IsDraggingSV = false;
                }
                else {
                    float relX = (io.MousePos.x - areaPos.x) / areaSize;
                    float relY = (io.MousePos.y - areaPos.y) / areaSize;

                    float h, s, v;
                    GetHSV(h, s, v);  // 保持当前色相值不变

                    s = ImClamp(relX, 0.0f, 1.0f);
                    v = ImClamp(1.0f - relY, 0.0f, 1.0f);

                    SetFromHSV(h, s, v);  // 仅更新饱和度和明度
                }
            }
        }

        // 处理色相条交互
        void HandleHueInteraction(const ImVec2& barPos, float barWidth, float barHeight)
        {
            if (m_IsDraggingSV || m_IsDraggingAlpha)return;
            ImGuiIO& io = ImGui::GetIO();
            ImRect barRect(barPos, ImVec2(barPos.x + barWidth, barPos.y + barHeight));

            if (ImGui::IsMouseDown(0)) {
                if (barRect.Contains(io.MousePos)) {
                    m_IsDraggingHue = true;
                }
            }

            if (m_IsDraggingHue) {
                if (ImGui::IsMouseReleased(0)) {
                    m_IsDraggingHue = false;
                }
                else {
                    float relX = (io.MousePos.x - barPos.x) / barWidth;
                    float hue = ImClamp(relX, 0.0f, 1.0f) * 360.0f;

                    float h, s, v;
                    GetHSV(h, s, v);
                    SetFromHSV(hue, s, v);
                }
            }
        }

        // 处理透明度条交互
        void HandleAlphaInteraction(const ImVec2& barPos, float barWidth, float barHeight)
        {
            if (m_IsDraggingSV || m_IsDraggingHue)return;
            ImGuiIO& io = ImGui::GetIO();
            ImRect barRect(barPos, ImVec2(barPos.x + barWidth, barPos.y + barHeight));

            if (ImGui::IsMouseDown(0)) {
                if (barRect.Contains(io.MousePos)) {
                    m_IsDraggingAlpha = true;
                }
            }

            if (m_IsDraggingAlpha) {
                if (ImGui::IsMouseReleased(0)) {
                    m_IsDraggingAlpha = false;
                }
                else {
                    float relX = (io.MousePos.x - barPos.x) / barWidth;
                    float alpha = ImClamp(relX, 0.0f, 1.0f);

                    ImVec4 newColor = m_Color;
                    newColor.w = alpha;
                    SetColor(newColor);
                }
            }
        }

        // 更新输入控件值
        void UpdateInputValues() {
            m_RInput.SetValue(m_Color.x);
            m_GInput.SetValue(m_Color.y);
            m_BInput.SetValue(m_Color.z);
            m_AInput.SetValue(m_Color.w);

            char hexBuf[9];
            snprintf(hexBuf, sizeof(hexBuf), "%02X%02X%02X%02X",
                (int)(m_Color.x * 255),
                (int)(m_Color.y * 255),
                (int)(m_Color.z * 255),
                (int)(m_Color.w * 255));
            m_HexInput.SetText(hexBuf);
        }

    public:
        ImColorPicker(const std::string& WidgetName)
            : ImWidget(WidgetName),
            m_RInput(WidgetName + "_R"), m_GInput(WidgetName + "_G"),
            m_BInput(WidgetName + "_B"), m_AInput(WidgetName + "_A"),
            m_HexInput(WidgetName + "_Hex"),
            m_RLabel(WidgetName + "_RLabel"), m_GLabel(WidgetName + "_GLabel"),
            m_BLabel(WidgetName + "_BLabel"), m_ALabel(WidgetName + "_ALabel")
            //m_HexLabel(WidgetName + "_HexLabel")
        {
            // 设置标签文本
            m_RLabel.SetText("R:");
            m_GLabel.SetText("G:");
            m_BLabel.SetText("B:");
            m_ALabel.SetText("A:");
            //m_HexLabel.SetText("Hex:");

            // 设置浮点输入范围
            m_RInput.SetRange(0.0f, 1.0f);
            m_GInput.SetRange(0.0f, 1.0f);
            m_BInput.SetRange(0.0f, 1.0f);
            m_AInput.SetRange(0.0f, 1.0f);

            // 设置浮点输入回调
            m_RInput.SetOnFloatValueChanged([this](float value) {
                m_Color.x = value;
                if (OnColorChanged) OnColorChanged(ImGui::ColorConvertFloat4ToU32(m_Color));
                });

            m_GInput.SetOnFloatValueChanged([this](float value) {
                m_Color.y = value;
                if (OnColorChanged) OnColorChanged(ImGui::ColorConvertFloat4ToU32(m_Color));
                });

            m_BInput.SetOnFloatValueChanged([this](float value) {
                m_Color.z = value;
                if (OnColorChanged) OnColorChanged(ImGui::ColorConvertFloat4ToU32(m_Color));
                });

            m_AInput.SetOnFloatValueChanged([this](float value) {
                m_Color.w = value;
                if (OnColorChanged) OnColorChanged(ImGui::ColorConvertFloat4ToU32(m_Color));
                });

            // 设置十六进制输入回调
            m_HexInput.SetOnTextChanged([this](const std::string& text) {
                unsigned int hexValue;
                if (sscanf_s(text.c_str(), "%08X", &hexValue) == 1) {
                    SetColor(ImVec4(
                        ((hexValue >> 24) & 0xFF) / 255.0f,
                        ((hexValue >> 16) & 0xFF) / 255.0f,
                        ((hexValue >> 8) & 0xFF) / 255.0f,
                        (hexValue & 0xFF) / 255.0f
                    ));
                }
                });
        }

        // 设置当前颜色
        void SetColor(const ImVec4& color) {
            if (memcmp(&m_Color, &color, sizeof(ImVec4)) != 0) {
                m_Color = color;
                UpdateInputValues();
                if (OnColorChanged) {
                    OnColorChanged(ImGui::ColorConvertFloat4ToU32(m_Color));
                }
            }
        }

        void SetColor(const ImU32& color) {
            SetColor(ImGui::ColorConvertU32ToFloat4(color));
        }

        const ImVec4& GetColor() const { return m_Color; }

        void SetOnColorChanged(std::function<void(const ImU32&)> callback) {
            OnColorChanged = callback;
        }

        void SetSVAreaSize(float size) { m_SVAreaSize = size; }

        // 获取最小尺寸 (已调整布局)
        virtual ImVec2 GetMinSize() override {
            float width = m_SVAreaSize + m_InputWidth + m_Spacing * 3;

            // 计算输入区域高度
            //float inputHeight =
            //    ImGui::GetTextLineHeight() * 5 +  // RGBA+Hex标签和输入框
            //    m_Spacing * 4;                   // 间距

            float height = m_PreviewHeight +
                m_SVAreaSize +
                m_HueBarHeight +
                m_AlphaBarHeight +
                m_Spacing * 4 
                ;

            return ImVec2(width, height);
        }

        // 渲染控件 (重构布局)
        virtual void Render() override {
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            ImVec2 currentPos = Position;

            // 1. 绘制颜色预览
            ImVec2 previewSize(Size.x, m_PreviewHeight);
            drawList->AddRectFilled(currentPos, currentPos + previewSize, IM_COL32(50, 50, 50, 255));
            drawList->AddRectFilled(currentPos, ImVec2(currentPos.x + previewSize.x / 2, currentPos.y + previewSize.y),
                ImGui::ColorConvertFloat4ToU32(m_Color));
            drawList->AddRectFilled(ImVec2(currentPos.x + previewSize.x / 2, currentPos.y),
                currentPos + previewSize, IM_COL32(30, 30, 30, 255));
            currentPos.y += m_PreviewHeight + m_Spacing;

            // 2. 主内容区域 (水平布局)
            ImVec2 contentSize(Size.x, m_SVAreaSize);
            ImVec2 svAreaPos = currentPos;
            ImVec2 inputAreaPos(currentPos.x + m_SVAreaSize + m_Spacing, currentPos.y);

            // 2.1 绘制SV区域
            DrawSVArea(drawList, svAreaPos, m_SVAreaSize);
            

            // 2.2 绘制输入区域
            ImVec2 inputCurrentPos = inputAreaPos;

            // RGBA输入
            float inputHeight = ImGui::GetTextLineHeight()+7;
            float inputWidth = m_InputWidth;

            // R
            m_RLabel.SetPosition(inputCurrentPos);
            m_RLabel.SetSize(ImVec2(15, inputHeight));
            m_RLabel.Render();

            m_RInput.SetPosition(ImVec2(inputCurrentPos.x + 20, inputCurrentPos.y));
            m_RInput.SetSize(ImVec2(inputWidth - 20, inputHeight));
            m_RInput.Render();
            inputCurrentPos.y += inputHeight + m_Spacing;

            // G
            m_GLabel.SetPosition(inputCurrentPos);
            m_GLabel.SetSize(ImVec2(15, inputHeight));
            m_GLabel.Render();

            m_GInput.SetPosition(ImVec2(inputCurrentPos.x + 20, inputCurrentPos.y));
            m_GInput.SetSize(ImVec2(inputWidth - 20, inputHeight));
            m_GInput.Render();
            inputCurrentPos.y += inputHeight + m_Spacing;

            // B
            m_BLabel.SetPosition(inputCurrentPos);
            m_BLabel.SetSize(ImVec2(15, inputHeight));
            m_BLabel.Render();

            m_BInput.SetPosition(ImVec2(inputCurrentPos.x + 20, inputCurrentPos.y));
            m_BInput.SetSize(ImVec2(inputWidth - 20, inputHeight));
            m_BInput.Render();
            inputCurrentPos.y += inputHeight + m_Spacing;

            // A
            m_ALabel.SetPosition(inputCurrentPos);
            m_ALabel.SetSize(ImVec2(15, inputHeight));
            m_ALabel.Render();

            m_AInput.SetPosition(ImVec2(inputCurrentPos.x + 20, inputCurrentPos.y));
            m_AInput.SetSize(ImVec2(inputWidth - 20, inputHeight));
            m_AInput.Render();
            inputCurrentPos.y += inputHeight + m_Spacing;

            // Hex
            //m_HexLabel.SetPosition(inputCurrentPos);
            //m_HexLabel.SetSize(ImVec2(30, inputHeight));
            //m_HexLabel.Render();

            m_HexInput.SetPosition(inputCurrentPos);
            m_HexInput.SetSize(ImVec2(inputWidth , inputHeight));
            m_HexInput.Render();

            // 3. 色相条 (在SV区域下方)
            currentPos.y += m_SVAreaSize + m_Spacing;
            ImVec2 hueBarPos = currentPos;
            DrawHueBar(drawList, hueBarPos, Size.x, m_HueBarHeight);
            

            // 4. 透明度条 (在色相条下方)
            currentPos.y += m_HueBarHeight + m_Spacing;
            ImVec2 alphaBarPos = currentPos;
            DrawAlphaBar(drawList, alphaBarPos, Size.x, m_AlphaBarHeight);

            //if (m_IsDraggingAlpha)
            //{
            //    HandleAlphaInteraction(alphaBarPos, Size.x, m_AlphaBarHeight);
            //}
            //else if (m_IsDraggingHue)
            //{
            //    HandleHueInteraction(hueBarPos, Size.x, m_HueBarHeight);
            //}
            //else if (m_IsDraggingSV)
            //{
            //    HandleSVInteraction(svAreaPos, m_SVAreaSize);
            //}
            //else
            //{
            //    HandleSVInteraction(svAreaPos, m_SVAreaSize);
            //    HandleHueInteraction(hueBarPos, Size.x, m_HueBarHeight);
            //    HandleAlphaInteraction(alphaBarPos, Size.x, m_AlphaBarHeight);
            //}
            HandleSVInteraction(svAreaPos, m_SVAreaSize);
            HandleHueInteraction(hueBarPos, Size.x, m_HueBarHeight);
            HandleAlphaInteraction(alphaBarPos, Size.x, m_AlphaBarHeight);
        }

        // 属性列表 (更新为使用输入控件)
        virtual std::unordered_set<PropertyInfo> GetProperties() override {
            return {
                {"Color", PropertyType::Color, "Data",
                    [this](void* v) {
                        ImU32 col32 = *static_cast<ImU32*>(v);
                        SetColor(col32);
                    },
                    [this]() -> void* {
                        static ImU32 col32;
                        col32 = ImGui::ColorConvertFloat4ToU32(m_Color);
                        return &col32;
                    }},
                {"SVAreaSize", PropertyType::Float, "Layout",
                    [this](void* v) { SetSVAreaSize(*static_cast<float*>(v)); },
                    [this]() -> void* { return &m_SVAreaSize; }}
            };
        }
        virtual std::string GetRegisterTypeName()override { return "ImColorPicker"; }
    };
}