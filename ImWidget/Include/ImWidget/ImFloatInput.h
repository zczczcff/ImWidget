#pragma once
#include "ImWidget/ImInputText.h"

namespace ImGuiWidget
{
    class ImFloatInput : public ImInputText
    {
    private:
        float m_MinValue = -FLT_MAX;
        float m_MaxValue = FLT_MAX;
        int m_DecimalPlaces = 2;
        float m_PreviousValue = 0.0f;
        std::function<void(float)> OnFloatValueChanged;

        void FormatValue(float value)
        {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(m_DecimalPlaces) << value;
            std::string formatted = oss.str();

            // ÒÆ³ýÎ²ËæÁã
            size_t dotPos = formatted.find('.');
            if (dotPos != std::string::npos)
            {
                size_t lastNonZero = formatted.find_last_not_of('0');
                if (lastNonZero != std::string::npos && lastNonZero > dotPos)
                {
                    formatted.erase(lastNonZero + 1);
                }
                if (formatted.back() == '.')
                {
                    formatted.pop_back();
                }
            }

            if (formatted != m_Text)
            {
                SetText(formatted);
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
                float value = std::stof(m_Text, &pos);

                if (pos != m_Text.size() || value < m_MinValue || value > m_MaxValue)
                {
                    SetValue(m_MinValue);
                    return;
                }

                FormatValue(value);
            }
            catch (...)
            {
                SetValue(m_MinValue);
            }
        }

        void CheckValueChanged()
        {
            float currentValue = GetValue();
            const float epsilon = 1e-6f;
            if (std::abs(currentValue - m_PreviousValue) > epsilon)
            {
                if (OnFloatValueChanged)
                {
                    OnFloatValueChanged(currentValue);
                }
                m_PreviousValue = currentValue;
            }
        }

    public:
        ImFloatInput(const std::string& WidgetName)
            : ImInputText(WidgetName), m_PreviousValue(0.0f)
        {
            SetInputMode(ImInputTextMode::Decimal);
            SetAllowNegative(true);
            SetValue(0.0f);
        }

        void SetRange(float minVal, float maxVal)
        {
            m_MinValue = minVal;
            m_MaxValue = maxVal;
            ValidateAndFormat();
        }

        void SetDecimalPlaces(int places)
        {
            m_DecimalPlaces = std::clamp(places, 0, 10);
            SetMaxDecimalDigits(m_DecimalPlaces);
            ValidateAndFormat();
        }

        float GetValue() const
        {
            if (m_Text.empty()) return m_MinValue;

            try
            {
                size_t pos;
                float value = std::stof(m_Text, &pos);

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

        void SetValue(float value)
        {
            value = std::clamp(value, m_MinValue, m_MaxValue);
            FormatValue(value);
            m_PreviousValue = value;
        }

        void SetText(const std::string& text) override
        {
            ImInputText::SetText(text);
            ValidateAndFormat();
            m_PreviousValue = GetValue();
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
                { "MinValue", PropertyType::Float, "Data",
                    [this](void* v)
 {
m_MinValue = *static_cast<float*>(v);
ValidateAndFormat();
},
[this]() -> void* { return &m_MinValue; }
                }
            );

            props.insert(
                { "MaxValue", PropertyType::Float, "Data",
                    [this](void* v)
 {
m_MaxValue = *static_cast<float*>(v);
ValidateAndFormat();
},
[this]() -> void* { return &m_MaxValue; }
                }
            );

            props.insert(
                { "DecimalPlaces", PropertyType::Int, "Data",
                    [this](void* v)
 {
SetDecimalPlaces(*static_cast<int*>(v));
},
[this]() -> void* { return &m_DecimalPlaces; }
                }
            );

            return props;
        }

        virtual std::string GetRegisterTypeName() override { return "ImFloatInput"; }

        virtual ImWidget* CopyWidget()
        {
            return new ImFloatInput(*this);
        }

        void SetOnFloatValueChanged(std::function<void(float)> callback)
        {
            OnFloatValueChanged = callback;
        }
    };
}