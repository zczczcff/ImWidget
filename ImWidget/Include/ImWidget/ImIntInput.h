#pragma once
#include "ImWidget/ImInputText.h"

namespace ImGuiWidget
{
    class ImIntInput : public ImInputText
    {
    private:
        int m_MinValue = INT_MIN;
        int m_MaxValue = INT_MAX;
        int m_PreviousValue = 0;
        std::function<void(int)> OnIntValueChanged;

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

                if (pos != m_Text.size() || value < m_MinValue || value > m_MaxValue)
                {
                    SetValue(m_MinValue);
                    return;
                }

                // 重新格式化以去除前置零等
                std::string formatted = std::to_string(value);
                if (formatted != m_Text)
                {
                    SetText(formatted);
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
        ImIntInput(const std::string& WidgetName)
            : ImInputText(WidgetName), m_PreviousValue(0)
        {
            SetInputMode(ImInputTextMode::Integer);
            SetAllowNegative(true);
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
            if (m_Text.empty()) return m_MinValue;

            try
            {
                size_t pos;
                int value = std::stoi(m_Text, &pos);

                if (pos == m_Text.size() && value >= m_MinValue && value <= m_MaxValue)
                {
                    return value;
                }
            }
            catch (...)
            {
            }

            return m_MinValue;
        }

        void SetValue(int value)
        {
            value = (value < m_MinValue) ? m_MinValue :
                (value > m_MaxValue) ? m_MaxValue : value;

            SetText(std::to_string(value));
            CheckValueChanged();
        }

        void SetText(const std::string& text) override
        {
            ImInputText::SetText(text);
            ValidateAndFormat();
            CheckValueChanged();
        }

        void CheckTextChanged() override
        {
            ValidateAndFormat();
            ImInputText::CheckTextChanged();
            CheckValueChanged();
        }

        void HandleEventInternal(ImEvent* event) override
        {
            if (auto focusEvent = event->As<ImFocusEvent>())
            {
                if (focusEvent->GetType() == ImEventType::FocusOut)
                {
                    ValidateAndFormat();
                    CheckValueChanged();
                }
            }

            ImInputText::HandleEventInternal(event);
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
[this]() -> void* { return &m_MinValue; }
                }
            );

            props.insert(
                { "MaxValue", PropertyType::Int, "Data",
                    [this](void* v)
 {
m_MaxValue = *static_cast<int*>(v);
ValidateAndFormat();
},
[this]() -> void* { return &m_MaxValue; }
                }
            );

            return props;
        }

        virtual std::string GetRegisterTypeName() override { return "ImIntInput"; }

        virtual ImWidget* CopyWidget()
        {
            return new ImIntInput(*this);
        }

        void SetOnIntValueChanged(std::function<void(int)> callback)
        {
            OnIntValueChanged = callback;
        }
    };
}