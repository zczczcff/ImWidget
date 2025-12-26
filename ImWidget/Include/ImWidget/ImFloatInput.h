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
		float m_PreviousValue = 0.0f; // 记录上次失去焦点时的值
		bool m_IsValid = true;
		// 浮点数值改变时的回调
		std::function<void(float)> OnFloatValueChanged;


		void FormatValue(double value)
		{
			std::ostringstream oss;
			oss << std::fixed << std::setprecision(m_DecimalPlaces) << value;
			std::string formatted = oss.str();

			// 移除多余的尾随零
			size_t dotPos = formatted.find('.');
			if (dotPos != std::string::npos)
			{
				// 移除尾随零
				size_t lastNonZero = formatted.find_last_not_of('0');
				if (lastNonZero != std::string::npos && lastNonZero > dotPos)
				{
					formatted.erase(lastNonZero + 1);
				}

				// 如果小数点后没有数字，移除小数点
				if (formatted.back() == '.')
				{
					formatted.pop_back();
				}
			}

			// 更新文本（仅当格式化后不同）
			if (formatted != m_Text)
			{
				m_Text = formatted;
				m_CursorPos = static_cast<int>(m_Text.size());
				m_SelectionStart = -1;
				m_SelectionEnd = -1;
			}
		}

	public:
		ImFloatInput(const std::string& WidgetName)
			: ImInputText(WidgetName), m_PreviousValue(0.0f)
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

			if (end == start + m_Text.size() && value >= m_MinValue && value <= m_MaxValue)
			{
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
			if (std::abs(value - oldValue) < epsilon && !m_Text.empty())
			{
				return; // 值未变化，无需更新
			}

			FormatValue(value);
			m_PreviousText = m_Text;
			m_PreviousValue = value;
		}

		void SetText(const std::string& text) override
		{
			ImInputText::SetText(text);
			ValidateAndFormat();
			m_PreviousValue = GetValue();
		}

		void Render() override
		{
			if (m_IsFocused)
			{
				m_IsValid = IsTextValid();
			}
			ImInputText::Render();
			if (!m_IsValid && m_IsFocused)
			{
				ImDrawList* drawList = ImGui::GetWindowDrawList();
				drawList->AddRect(
					Position,
					Position + Size,
					IM_COL32(255, 0, 0, 255),
					m_Rounding,
					0,
					2.0f
				);
			}
		}

		void CheckTextChanged() 
		{
			ValidateAndFormat();
			ImInputText::CheckTextChanged();
			CheckValueChanged();
		}

		void HandleInput() 
		{
			ImGuiIO& io = ImGui::GetIO();

			if (ImGui::IsKeyPressed(ImGuiKey_Enter) ||
				ImGui::IsKeyPressed(ImGuiKey_KeypadEnter))
			{
				m_IsFocused = false;
				ValidateAndFormat();
				CheckValueChanged();
				return;
			}

			if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_V))
			{
				const char* clipboard = ImGui::GetClipboardText();
				if (clipboard)
				{
					std::string filtered;
					bool hasDecimal = false;
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
						else if (*c == '.' && !hasDecimal)
						{
							filtered += *c;
							hasDecimal = true;
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

			for (int i = 0; i < io.InputQueueCharacters.Size; i++)
			{
				unsigned int c = io.InputQueueCharacters[i];

				if (!(c >= '0' && c <= '9') &&
					!(c == '-' && m_CursorPos == 0) &&
					!(c == '.' && m_Text.find('.') == std::string::npos))
				{
					io.InputQueueCharacters.Data[i] = 0;
				}
			}

			//ImInputText::HandleInput();
		}

	private:
		bool IsTextValid() const
		{
			if (m_Text.empty()) return true;
			if (m_Text == "-") return true;
			if (m_Text == ".") return true;
			if (m_Text == "-.") return true;

			try
			{
				size_t pos;
				float value = std::stof(m_Text, &pos);
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
				float value = std::stof(m_Text, &pos);

				// 无效格式或超出范围
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

		void FormatValue(float value)
		{
			std::ostringstream oss;
			oss << std::fixed << std::setprecision(m_DecimalPlaces) << value;
			std::string formatted = oss.str();

			// 移除多余的尾随零
			size_t dotPos = formatted.find('.');
			if (dotPos != std::string::npos)
			{
				// 移除尾随零
				size_t lastNonZero = formatted.find_last_not_of('0');
				if (lastNonZero != std::string::npos && lastNonZero > dotPos)
				{
					formatted.erase(lastNonZero + 1);
				}

				// 如果小数点后没有数字，移除小数点
				if (formatted.back() == '.')
				{
					formatted.pop_back();
				}
			}

			if (formatted != m_Text)
			{
				m_Text = formatted;
				m_CursorPos = static_cast<int>(m_Text.size());
				ClearSelection();
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
					[this]() -> void* { return &m_MinValue; } }
			);

			props.insert(
				{ "MaxValue", PropertyType::Float, "Data",
					[this](void* v)
					{
					m_MaxValue = *static_cast<float*>(v);
					ValidateAndFormat();
					},
					[this]() -> void* { return &m_MaxValue; } }
								);

			props.insert(
				{ "DecimalPlaces", PropertyType::Int, "Data",
					[this](void* v)
					{
					SetDecimalPlaces(*static_cast<int*>(v));
					},
					[this]() -> void* { return &m_DecimalPlaces; } }
								);

			return props;
		}

		virtual std::string GetRegisterTypeName()override { return "ImFloatInput"; }

		virtual ImWidget* CopyWidget()
		{
			return new ImFloatInput(*this);
		}
		// 设置浮点数值改变回调
		void SetOnFloatValueChanged(std::function<void(float)> callback)
		{
			OnFloatValueChanged = callback;
		}

	};
}