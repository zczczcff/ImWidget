#pragma once
#include "ImWidget.h"
#include <string>
#include <functional>
#include <algorithm> // for std::min, std::max
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>

namespace ImGuiWidget
{
    class ImInputText : public ImWidget
    {
    protected:
        std::string m_Text;
        ImU32 m_TextColor = IM_COL32(0, 0, 0, 255);
        ImU32 m_BackgroundColor = IM_COL32(255, 255, 255, 255);
        ImU32 m_BorderColor = IM_COL32(100, 100, 100, 255);
        float m_BorderThickness = 1.0f;
        float m_Rounding = 4.0f;

        bool m_IsFocused = false;
        int m_CursorPos = 0;
        int m_SelectionStart = -1;
        int m_SelectionEnd = -1;

        // 光标闪烁计时器
        float m_CursorBlinkTimer = 0.0f;
        const float CURSOR_BLINK_RATE = 0.5f; // 闪烁频率

        std::function<void(const std::string&)> OnTextChanged;

        //void HandleInput();
        //void HandleSelection();
        //void CopySelection();
        //void DeleteSelection();
        //void MoveCursor(int direction);
        //void UpdateCursorBlink();

    public:
        ImInputText(const std::string& WidgetName)
            : ImWidget(WidgetName)
        {}

        virtual void SetText(const std::string& text) {
            m_Text = text;
            m_CursorPos = static_cast<int>(m_Text.size());
            m_SelectionStart = -1;
            m_SelectionEnd = -1;
        }

        const std::string& GetText() const { return m_Text; }

        void SetTextColor(ImU32 color) { m_TextColor = color; }
        void SetBackgroundColor(ImU32 color) { m_BackgroundColor = color; }
        void SetBorderColor(ImU32 color) { m_BorderColor = color; }
        void SetBorderThickness(float thickness) { m_BorderThickness = thickness; }
        void SetRounding(float rounding) { m_Rounding = rounding; }

        void SetOnTextChanged(std::function<void(const std::string&)> callback) {
            OnTextChanged = callback;
        }

        virtual ImVec2 GetMinSize() override {
            ImFont* font = ImGui::GetFont();
            float width = font->CalcTextSizeA(
                ImGui::GetFontSize(),
                FLT_MAX,
                0.0f,
                "W" // 使用单个字符估算宽度
            ).x * 10; // 默认10个字符宽度

            float height = ImGui::GetTextLineHeight() + 8; // 上下留空

            return ImVec2(width, height);
        }

        virtual void Render() override {
            ImDrawList* drawList = ImGui::GetWindowDrawList();

            // 绘制背景和边框
            drawList->AddRectFilled(
                Position,
                Position + Size,
                m_BackgroundColor,
                m_Rounding
            );

            drawList->AddRect(
                Position,
                Position + Size,
                m_BorderColor,
                m_Rounding,
                0,
                m_BorderThickness
            );

            // 处理焦点逻辑
            ImGuiIO& io = ImGui::GetIO();
            bool clicked = ImGui::IsMouseClicked(0);
            bool isHovered = ImRect(Position, Position + Size).Contains(io.MousePos);

            if (clicked) {
                m_IsFocused = isHovered;
                if (m_IsFocused) {
                    // 设置光标位置
                    ImVec2 textPos = Position + ImVec2(4, (Size.y - ImGui::GetTextLineHeight()) * 0.5f);
                    float mouseX = io.MousePos.x - textPos.x;

                    // 计算鼠标点击处的字符位置
                    m_CursorPos = 0;
                    float currentWidth = 0.0f;
                    for (int i = 0; i < static_cast<int>(m_Text.size()); ++i) {
                        char c = m_Text[i];
                        float charWidth = ImGui::CalcTextSize(&c, &c + 1).x;
                        if (mouseX < currentWidth + charWidth * 0.5f) {
                            break;
                        }
                        currentWidth += charWidth;
                        m_CursorPos++;
                    }
                    m_CursorBlinkTimer = 0.0f; // 重置光标闪烁
                }
            }

            // 处理键盘输入
            if (m_IsFocused) {
                HandleInput();
                UpdateCursorBlink();
            }

            // 绘制文本
            ImVec2 textPos = Position + ImVec2(4, (Size.y - ImGui::GetTextLineHeight()) * 0.5f);
            drawList->AddText(textPos, m_TextColor, m_Text.c_str());

            // 绘制选择区域
            if (m_SelectionStart != -1 && m_SelectionEnd != -1) {
                int start = ImMin(m_SelectionStart, m_SelectionEnd);
                int end = ImMax(m_SelectionStart, m_SelectionEnd);

                ImVec2 selectionStartPos = textPos;
                for (int i = 0; i < start; ++i) {
                    char c = m_Text[i];
                    selectionStartPos.x += ImGui::CalcTextSize(&c, &c + 1).x;
                }

                ImVec2 selectionEndPos = selectionStartPos;
                for (int i = start; i < end; ++i) {
                    char c = m_Text[i];
                    selectionEndPos.x += ImGui::CalcTextSize(&c, &c + 1).x;
                }

                drawList->AddRectFilled(
                    ImVec2(selectionStartPos.x, Position.y + 2),
                    ImVec2(selectionEndPos.x, Position.y + Size.y - 2),
                    IM_COL32(100, 150, 250, 100) // 半透明蓝色
                );
            }

            // 绘制光标（聚焦且闪烁可见时）
            if (m_IsFocused && (static_cast<int>(m_CursorBlinkTimer / CURSOR_BLINK_RATE) % 2 == 0)) {
                ImVec2 cursorStart = textPos;
                for (int i = 0; i < m_CursorPos; ++i) {
                    char c = m_Text[i];
                    cursorStart.x += ImGui::CalcTextSize(&c, &c + 1).x;
                }

                drawList->AddLine(
                    ImVec2(cursorStart.x, Position.y + 4),
                    ImVec2(cursorStart.x, Position.y + Size.y - 4),
                    m_TextColor,
                    1.0f
                );
            }
        }

        virtual std::vector<PropertyInfo> GetProperties() override {
            std::vector<PropertyInfo> props = {
                {"Text", PropertyType::String, "Data",
                    [this](void* v) { m_Text = *static_cast<std::string*>(v); },
                    [this]() -> void* { return &m_Text; }},
                {"TextColor", PropertyType::Color, "Style",
                    [this](void* v) { m_TextColor = *static_cast<ImU32*>(v); },
                    [this]() -> void* { return &m_TextColor; }},
                {"BackgroundColor", PropertyType::Color, "Style",
                    [this](void* v) { m_BackgroundColor = *static_cast<ImU32*>(v); },
                    [this]() -> void* { return &m_BackgroundColor; }},
                {"BorderColor", PropertyType::Color, "Style",
                    [this](void* v) { m_BorderColor = *static_cast<ImU32*>(v); },
                    [this]() -> void* { return &m_BorderColor; }},
                {"BorderThickness", PropertyType::Float, "Style",
                    [this](void* v) { m_BorderThickness = *static_cast<float*>(v); },
                    [this]() -> void* { return &m_BorderThickness; }},
                {"Rounding", PropertyType::Float, "Style",
                    [this](void* v) { m_Rounding = *static_cast<float*>(v); },
                    [this]() -> void* { return &m_Rounding; }}
            };
            return props;
        }

    protected:
        virtual void HandleInput() {
            ImGuiIO& io = ImGui::GetIO();

            // 处理字符输入
            for (int i = 0; i < io.InputQueueCharacters.Size; i++) {
                unsigned int c = io.InputQueueCharacters[i];
                if (c < 256&&c!=8) { // ASCII字符
                    // 删除选中文本
                    if (m_SelectionStart != -1 && m_SelectionEnd != -1) {
                        DeleteSelection();
                    }

                    // 插入字符
                    m_Text.insert(m_CursorPos, 1, static_cast<char>(c));
                    m_CursorPos++;

                    if (OnTextChanged) {
                        OnTextChanged(m_Text);
                    }
                }
            }

            // 处理退格键
            if (ImGui::IsKeyPressed(ImGuiKey_Backspace)) {
                if (m_SelectionStart != -1 && m_SelectionEnd != -1) {
                    DeleteSelection();
                }
                else if (m_CursorPos > 0) {
                    m_Text.erase(m_CursorPos - 1, 1);
                    m_CursorPos--;

                    if (OnTextChanged) {
                        OnTextChanged(m_Text);
                    }
                }
            }

            // 处理删除键
            if (ImGui::IsKeyPressed(ImGuiKey_Delete)) {
                if (m_SelectionStart != -1 && m_SelectionEnd != -1) {
                    DeleteSelection();
                }
                else if (m_CursorPos < static_cast<int>(m_Text.size())) {
                    m_Text.erase(m_CursorPos, 1);

                    if (OnTextChanged) {
                        OnTextChanged(m_Text);
                    }
                }
            }

            // 处理方向键
            if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow)) {
                MoveCursor(-1);
            }
            if (ImGui::IsKeyPressed(ImGuiKey_RightArrow)) {
                MoveCursor(1);
            }

            // 处理Ctrl+C复制
            if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_C)) {
                CopySelection();
            }
        }

        void MoveCursor(int direction) {
            if (direction < 0 && m_CursorPos > 0) {
                m_CursorPos--;
            }
            else if (direction > 0 && m_CursorPos < static_cast<int>(m_Text.size())) {
                m_CursorPos++;
            }
            m_CursorBlinkTimer = 0.0f; // 重置光标闪烁
        }

        void UpdateCursorBlink() {
            ImGuiIO& io = ImGui::GetIO();
            m_CursorBlinkTimer += io.DeltaTime;
            if (m_CursorBlinkTimer > 2 * CURSOR_BLINK_RATE) {
                m_CursorBlinkTimer = 0.0f;
            }
        }

        void CopySelection() {
            if (m_SelectionStart != -1 && m_SelectionEnd != -1) {
                int start = ImMin(m_SelectionStart, m_SelectionEnd);
                int end = ImMax(m_SelectionStart, m_SelectionEnd);
                std::string selectedText = m_Text.substr(start, end - start);
                ImGui::SetClipboardText(selectedText.c_str());
            }
        }

        void DeleteSelection() {
            if (m_SelectionStart == -1 || m_SelectionEnd == -1) return;

            int start = ImMin(m_SelectionStart, m_SelectionEnd);
            int end = ImMax(m_SelectionStart, m_SelectionEnd);

            m_Text.erase(start, end - start);
            m_CursorPos = start;
            m_SelectionStart = -1;
            m_SelectionEnd = -1;

            if (OnTextChanged) {
                OnTextChanged(m_Text);
            }
        }
    };


    class ImIntInput : public ImInputText
    {
    private:
        int m_MinValue = INT_MIN;
        int m_MaxValue = INT_MAX;

        // 整数值改变时的回调
        std::function<void(int)> OnIntValueChanged;

        void ValidateAndFormat()
        {
            if (m_Text.empty()) {
                SetValue(m_MinValue);
                return;
            }

            const char* start = m_Text.c_str();
            char* end = nullptr;
            long value = std::strtol(start, &end, 10);

            if (end != start + m_Text.size() || value < m_MinValue || value > m_MaxValue) {
                SetValue(m_MinValue);
            }
            else {
                // 如果值有效，但文本格式不正确（如前置零），则重新格式化
                std::string formatted = std::to_string(static_cast<int>(value));
                if (formatted != m_Text) {
                    m_Text = formatted;
                    m_CursorPos = static_cast<int>(m_Text.size());
                    m_SelectionStart = -1;
                    m_SelectionEnd = -1;

                    if (OnTextChanged) {
                        OnTextChanged(m_Text);
                    }

                    // 触发整数值改变回调
                    if (OnIntValueChanged) {
                        OnIntValueChanged(static_cast<int>(value));
                    }
                }
            }
        }

    public:
        ImIntInput(const std::string& WidgetName)
            : ImInputText(WidgetName)
        {
            SetValue(0);
        }

        void SetRange(int minVal, int maxVal)
        {
            m_MinValue = minVal;
            m_MaxValue = maxVal;
            ValidateAndFormat();
        }

        int GetValue() const
        {
            const char* start = m_Text.c_str();
            char* end = nullptr;
            long value = std::strtol(start, &end, 10);

            if (end == start + m_Text.size() && value >= m_MinValue && value <= m_MaxValue) {
                return static_cast<int>(value);
            }
            return m_MinValue;
        }

        void SetValue(int value)
        {
            int oldValue = GetValue();
            value = (value < m_MinValue) ? m_MinValue :
                (value > m_MaxValue) ? m_MaxValue : value;

            if (value == oldValue && !m_Text.empty()) {
                return; // 值未变化，无需更新
            }

            m_Text = std::to_string(value);
            m_CursorPos = static_cast<int>(m_Text.size());
            m_SelectionStart = -1;
            m_SelectionEnd = -1;

            if (OnTextChanged) {
                OnTextChanged(m_Text);
            }

            // 触发整数值改变回调
            if (OnIntValueChanged) {
                OnIntValueChanged(value);
            }
        }

        void SetText(const std::string& text) override
        {
            ImInputText::SetText(text);
            ValidateAndFormat();
        }

        virtual void Render() override
        {
            bool wasFocused = m_IsFocused;
            ImInputText::Render();
            if (wasFocused && !m_IsFocused) {
                ValidateAndFormat();
            }
        }

        void HandleInput() override
        {
            ImGuiIO& io = ImGui::GetIO();

            if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_V)) {
                const char* clipboard = ImGui::GetClipboardText();
                if (clipboard) {
                    std::string filtered;
                    bool hasSign = false;

                    for (const char* c = clipboard; *c != 0; c++) {
                        if (std::isdigit(static_cast<unsigned char>(*c))) {
                            filtered += *c;
                        }
                        else if (*c == '-' && filtered.empty()) {
                            filtered += *c;
                            hasSign = true;
                        }
                    }

                    if (!filtered.empty()) {
                        if (m_SelectionStart != -1 && m_SelectionEnd != -1) {
                            DeleteSelection();
                        }

                        m_Text.insert(m_CursorPos, filtered);
                        m_CursorPos += static_cast<int>(filtered.size());

                        if (OnTextChanged) {
                            OnTextChanged(m_Text);
                        }

                        // 输入后立即验证（不同于基类行为）
                        ValidateAndFormat();
                    }
                }
                return;
            }

            for (int i = 0; i < io.InputQueueCharacters.Size; i++) {
                unsigned int c = io.InputQueueCharacters[i];

                if (!(c >= '0' && c <= '9') &&
                    !(c == '-' && m_CursorPos == 0))
                {
                    io.InputQueueCharacters.Data[i] = 0;
                    continue;
                }
            }

            ImInputText::HandleInput();

            // 输入后立即验证（不同于基类行为）
            if (io.InputQueueCharacters.Size > 0) {
                ValidateAndFormat();
            }
        }

        virtual std::vector<PropertyInfo> GetProperties() override
        {
            auto props = ImInputText::GetProperties();

            props.push_back(
                { "MinValue", PropertyType::Int, "Data",
                    [this](void* v) {
                        m_MinValue = *static_cast<int*>(v);
                        ValidateAndFormat();
                    },
                    [this]() -> void* { return &m_MinValue; } }
            );

            props.push_back(
                { "MaxValue", PropertyType::Int, "Data",
                    [this](void* v) {
                        m_MaxValue = *static_cast<int*>(v);
                        ValidateAndFormat();
                    },
                    [this]() -> void* { return &m_MaxValue; } }
            );

            return props;
        }

        // 设置整数值改变回调
        void SetOnIntValueChanged(std::function<void(int)> callback)
        {
            OnIntValueChanged = callback;
        }
    };

    class ImFloatInput : public ImInputText
    {
    private:
        float m_MinValue = -FLT_MAX;
        float m_MaxValue = FLT_MAX;
        int m_DecimalPlaces = 2;

        // 浮点数值改变时的回调
        std::function<void(float)> OnFloatValueChanged;

        void ValidateAndFormat()
        {
            if (m_Text.empty()) {
                SetValue(m_MinValue);
                return;
            }

            const char* start = m_Text.c_str();
            char* end = nullptr;
            double value = std::strtod(start, &end);

            if (end != start + m_Text.size() || value < m_MinValue || value > m_MaxValue) {
                SetValue(m_MinValue);
            }
            else {
                FormatValue(value);
            }
        }

        void FormatValue(double value)
        {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(m_DecimalPlaces) << value;
            std::string formatted = oss.str();

            // 移除多余的尾随零
            size_t dotPos = formatted.find('.');
            if (dotPos != std::string::npos) {
                // 移除尾随零
                size_t lastNonZero = formatted.find_last_not_of('0');
                if (lastNonZero != std::string::npos && lastNonZero > dotPos) {
                    formatted.erase(lastNonZero + 1);
                }

                // 如果小数点后没有数字，移除小数点
                if (formatted.back() == '.') {
                    formatted.pop_back();
                }
            }

            // 更新文本（仅当格式化后不同）
            if (formatted != m_Text) {
                m_Text = formatted;
                m_CursorPos = static_cast<int>(m_Text.size());
                m_SelectionStart = -1;
                m_SelectionEnd = -1;

                if (OnTextChanged) {
                    OnTextChanged(m_Text);
                }

                // 触发浮点数值改变回调
                if (OnFloatValueChanged) {
                    OnFloatValueChanged(value);
                }
            }
        }

    public:
        ImFloatInput(const std::string& WidgetName)
            : ImInputText(WidgetName)
        {
            SetValue(0.0);
        }

        void SetRange(double minVal, double maxVal)
        {
            m_MinValue = minVal;
            m_MaxValue = maxVal;
            ValidateAndFormat();
        }

        void SetDecimalPlaces(int places)
        {
            m_DecimalPlaces = std::clamp(places, 0, 10);
            ValidateAndFormat();
        }

        double GetValue() const
        {
            const char* start = m_Text.c_str();
            char* end = nullptr;
            double value = std::strtod(start, &end);

            if (end == start + m_Text.size() && value >= m_MinValue && value <= m_MaxValue) {
                return value;
            }
            return m_MinValue;
        }

        void SetValue(float value)
        {
            float oldValue = GetValue();
            value = std::clamp(value, m_MinValue, m_MaxValue);

            // 使用epsilon比较浮点数
            const double epsilon = 1e-9;
            if (std::abs(value - oldValue) < epsilon && !m_Text.empty()) {
                return; // 值未变化，无需更新
            }

            FormatValue(value);
        }

        void SetText(const std::string& text) override
        {
            ImInputText::SetText(text);
            ValidateAndFormat();
        }

        virtual void Render() override
        {
            bool wasFocused = m_IsFocused;
            ImInputText::Render();
            if (wasFocused && !m_IsFocused) {
                ValidateAndFormat();
            }
        }

        void HandleInput() override
        {
            ImGuiIO& io = ImGui::GetIO();

            if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_V)) {
                const char* clipboard = ImGui::GetClipboardText();
                if (clipboard) {
                    std::string filtered;
                    bool hasDecimal = false;
                    bool hasSign = false;

                    for (const char* c = clipboard; *c != 0; c++) {
                        if (std::isdigit(static_cast<unsigned char>(*c))) {
                            filtered += *c;
                        }
                        else if (*c == '-' && filtered.empty()) {
                            filtered += *c;
                            hasSign = true;
                        }
                        else if (*c == '.' && !hasDecimal) {
                            filtered += *c;
                            hasDecimal = true;
                        }
                    }

                    if (!filtered.empty()) {
                        if (m_SelectionStart != -1 && m_SelectionEnd != -1) {
                            DeleteSelection();
                        }

                        m_Text.insert(m_CursorPos, filtered);
                        m_CursorPos += static_cast<int>(filtered.size());

                        if (OnTextChanged) {
                            OnTextChanged(m_Text);
                        }

                        // 输入后立即验证
                        ValidateAndFormat();
                    }
                }
                return;
            }

            for (int i = 0; i < io.InputQueueCharacters.Size; i++) {
                unsigned int c = io.InputQueueCharacters[i];

                if (!(c >= '0' && c <= '9') &&
                    !(c == '-' && m_CursorPos == 0) &&
                    !(c == '.' && m_Text.find('.') == std::string::npos))
                {
                    io.InputQueueCharacters.Data[i] = 0;
                    continue;
                }
            }

            ImInputText::HandleInput();

            // 输入后立即验证
            if (io.InputQueueCharacters.Size > 0) {
                ValidateAndFormat();
            }
        }

        virtual std::vector<PropertyInfo> GetProperties() override
        {
            auto props = ImInputText::GetProperties();

            props.push_back(
                { "MinValue", PropertyType::Float, "Data",
                    [this](void* v) {
                        m_MinValue = *static_cast<float*>(v);
                        ValidateAndFormat();
                    },
                    [this]() -> void* { return &m_MinValue; } }
            );

            props.push_back(
                { "MaxValue", PropertyType::Float, "Data",
                    [this](void* v) {
                        m_MaxValue = *static_cast<float*>(v);
                        ValidateAndFormat();
                    },
                    [this]() -> void* { return &m_MaxValue; } }
            );

            props.push_back(
                { "DecimalPlaces", PropertyType::Int, "Data",
                    [this](void* v) {
                        SetDecimalPlaces(*static_cast<int*>(v));
                    },
                    [this]() -> void* { return &m_DecimalPlaces; } }
            );

            return props;
        }

        // 设置浮点数值改变回调
        void SetOnFloatValueChanged(std::function<void(float)> callback)
        {
            OnFloatValueChanged = callback;
        }
    };
}