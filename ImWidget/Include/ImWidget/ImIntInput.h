#pragma once
#include "ImWidget/ImInputText.h"
namespace ImGuiWidget
{

    class ImIntInput : public ImInputText
    {
    private:
        int m_MinValue = INT_MIN;
        int m_MaxValue = INT_MAX;
        int m_PreviousValue = 0; // 记录上次失去焦点时的值
        bool m_IsValid = true;
        // 整数值改变时的回调
        std::function<void(int)> OnIntValueChanged;


    public:
        ImIntInput(const std::string& WidgetName)
            : ImInputText(WidgetName), m_PreviousValue(0)
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

            if (end == start + m_Text.size() && value >= m_MinValue && value <= m_MaxValue)
            {
                return static_cast<int>(value);
            }
            return m_MinValue;
        }

        void SetValue(int value)
        {
            int oldValue = GetValue();
            value = (value < m_MinValue) ? m_MinValue :
                (value > m_MaxValue) ? m_MaxValue : value;

            if (value == oldValue && !m_Text.empty())
            {
                return; // 值未变化，无需更新
            }

            m_Text = std::to_string(value);
            m_PreviousText = m_Text;
            m_CursorPos = static_cast<int>(m_Text.size());
            m_SelectionStart = -1;
            m_SelectionEnd = -1;
        }

        void SetText(const std::string& text) override
        {
            ImInputText::SetText(text);
            ValidateAndFormat();
            m_PreviousValue = GetValue();
        }
        void Render() override
        {
            // 检查输入有效性
            if (m_IsFocused)
            {
                m_IsValid = IsTextValid();
            }

            // 基类渲染
            ImInputText::Render();

            // 无效状态视觉反馈
            if (!m_IsValid && m_IsFocused)
            {
                ImDrawList* drawList = ImGui::GetWindowDrawList();
                drawList->AddRect(
                    Position,
                    Position + Size,
                    IM_COL32(255, 0, 0, 255), // 红色边框
                    m_Rounding,
                    0,
                    2.0f
                );
            }
        }

        void CheckTextChanged() 
        {
            // 提交时验证并格式化
            ValidateAndFormat();

            // 然后调用基类检查
            ImInputText::CheckTextChanged();

            // 检查整数值变化
            CheckValueChanged();
        }

        void HandleInput() 
        {
            ImGuiIO& io = ImGui::GetIO();

            // 处理回车键
            if (ImGui::IsKeyPressed(ImGuiKey_Enter) ||
                ImGui::IsKeyPressed(ImGuiKey_KeypadEnter))
            {
                m_IsFocused = false;
                ValidateAndFormat();
                CheckValueChanged();
                return;
            }

            // 处理粘贴
            if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_V))
            {
                const char* clipboard = ImGui::GetClipboardText();
                if (clipboard)
                {
                    std::string filtered;
                    bool hasSign = false;

                    for (const char* c = clipboard; *c != 0; c++)
                    {
                        if (std::isdigit(static_cast<unsigned char>(*c)))
                        {
                            filtered += *c;
                        }
                        else if (*c == '-' && filtered.empty())
                        {
                            filtered += *c;
                            hasSign = true;
                        }
                    }

                    if (!filtered.empty())
                    {
                        if (m_SelectionStart != -1 && m_SelectionEnd != -1)
                        {
                            DeleteSelection();
                        }

                        m_Text.insert(m_CursorPos, filtered);
                        m_CursorPos += static_cast<int>(filtered.size());
                    }
                }
            }

            // 过滤输入字符
            for (int i = 0; i < io.InputQueueCharacters.Size; i++)
            {
                unsigned int c = io.InputQueueCharacters[i];

                // 只允许数字和负号（在开头）
                if (!(c >= '0' && c <= '9') &&
                    !(c == '-' && m_CursorPos == 0))
                {
                    io.InputQueueCharacters.Data[i] = 0;
                }
            }

            // 调用基类处理其他按键
            //ImInputText::HandleInput();
        }

    private:
        bool IsTextValid() const
        {
            if (m_Text.empty()) return true; // 允许空输入（临时状态）
            if (m_Text == "-") return true;  // 允许单个负号（临时状态）

            try
            {
                size_t pos;
                int value = std::stoi(m_Text, &pos);
                if (pos != m_Text.size()) return false;
                return (value >= m_MinValue && value <= m_MaxValue);
            }
            catch (...)
            {
                return false;
            }
        }

        void ValidateAndFormat()
        {
            if (m_Text.empty())
            {
                SetValue(m_MinValue);
                return;
            }

            try
            {
                size_t pos;
                int value = std::stoi(m_Text, &pos);

                // 无效格式或超出范围
                if (pos != m_Text.size() || value < m_MinValue || value > m_MaxValue)
                {
                    SetValue(m_MinValue);
                    return;
                }

                // 如果值有效但文本格式不正确（如前置零），则重新格式化
                std::string formatted = std::to_string(value);
                if (formatted != m_Text)
                {
                    m_Text = formatted;
                    m_CursorPos = static_cast<int>(m_Text.size());
                    ClearSelection();
                }
            }
            catch (...)
            {
                SetValue(m_MinValue);
            }
        }

        void CheckValueChanged()
        {
            int currentValue = GetValue();
            if (currentValue != m_PreviousValue)
            {
                if (OnIntValueChanged)
                {
                    OnIntValueChanged(currentValue);
                }
                m_PreviousValue = currentValue;
            }
        }
    public:
        virtual std::unordered_set<PropertyInfo, PropertyInfo::Hasher> GetProperties() override
        {
            auto props = ImInputText::GetProperties();

            props.insert(
                { "MinValue", PropertyType::Int, "Data",
                    [this](void* v)
    {
    m_MinValue = *static_cast<int*>(v);
    ValidateAndFormat();
    },
    [this]() -> void* { return &m_MinValue; } }
            );

            props.insert(
                { "MaxValue", PropertyType::Int, "Data",
                    [this](void* v)
    {
    m_MaxValue = *static_cast<int*>(v);
    ValidateAndFormat();
    },
    [this]() -> void* { return &m_MaxValue; } }
            );

            return props;
        }

        virtual std::string GetRegisterTypeName()override { return "ImIntInput"; }

        virtual ImWidget* CopyWidget()
        {
            return new ImIntInput(*this);
        }

        // 设置整数值改变回调
        void SetOnIntValueChanged(std::function<void(int)> callback)
        {
            OnIntValueChanged = callback;
        }

    };
}