#pragma once
#include "ImWidget.h"
#include <imgui_internal.h>
#include "Application/ImApplication.h"

namespace ImGuiWidget
{
	class ImTextBlock :public ImWidget
	{
	public:
		// 对齐方式枚举
		enum class TextAlignment_Horizontal {
			Left,
			Center,
			Right
		};

		enum class TextAlignment_Vertical {
			Top,
			Center,
			Bottom
		};
	protected:
		int TextSize = 14;
		std::string m_Text;
        ImU32 m_TextColor;
        TextAlignment_Horizontal halign = TextAlignment_Horizontal::Center;
        TextAlignment_Vertical valign = TextAlignment_Vertical::Center;
	public:
        ImTextBlock(const std::string& WidgetName):ImWidget(WidgetName), m_TextColor(IM_COL32(0, 0, 0, 255))
        {}
        virtual ImVec2 GetMinSize() 
        { 
			ImGui::PushFont(ImApplication::GetFont(TextSize));
			ImVec2 MinSize = ImVec2(ImGui::CalcTextSize(m_Text.c_str()).x, TextSize);
			ImGui::PopFont();
			return MinSize;
        }
		void SetText(const std::string& Text)
		{
			m_Text = Text;
		}
        void SetTextColor(ImU32 Color)
        {
            m_TextColor = Color;
        }
		virtual void Render() 
		{
			ImGui::PushFont(ImApplication::GetFont(TextSize));
            ImGuiContext& g = *GImGui;
            ImGuiWindow* window = g.CurrentWindow;
            // 计算文本尺寸（不换行）
            ImVec2 textSize = ImGui::CalcTextSize(m_Text.c_str(), nullptr, false, Size.x);

            // 计算绘制位置
            ImVec2 textPos = Position;

            // 水平对齐
            switch (halign) {
            case TextAlignment_Horizontal::Center:
                textPos.x = Position.x + (Size.x - textSize.x) * 0.5f;
                break;
            case TextAlignment_Horizontal::Right:
                textPos.x = Position.x + Size.x - textSize.x;
                break;
            default: // Left
                textPos.x = Position.x;
            }

            // 垂直对齐
            switch (valign) {
            case TextAlignment_Vertical::Center:
                textPos.y = Position.y + (Size.y - textSize.y) * 0.5f;
                break;
            case TextAlignment_Vertical::Bottom:
                textPos.y = Position.y + Size.y - textSize.y;
                break;
            default: // Top
                textPos.y = Position.y;
            }

            // 绘制文本（不限制绘制区域）
            window->DrawList->AddText(textPos, m_TextColor, m_Text.c_str());

			ImGui::PopFont();
		}

        void SetHorizontalAlignment(TextAlignment_Horizontal NewSetting)
        {
            halign = NewSetting;
        }

        void SetVerticalAlignment(TextAlignment_Vertical NewSetting)
        {
            valign = NewSetting;
        }

        virtual std::unordered_set<PropertyInfo, PropertyInfo::Hasher> GetProperties() override
        {
            auto props = ImWidget::GetProperties();

            // 文本内容
            props.insert({
                "Text",
                PropertyType::String,
                "Content",
                [this](void* val) { this->SetText(*static_cast<std::string*>(val)); },
                [this]() { return static_cast<void*>(&this->m_Text); }
                });
			props.insert
			({
				"TextFontSize",
				PropertyType::Int,
				"Content",
				[this](void* v) { this->TextSize = *static_cast<int*>(v); },
				[this]() -> void* { return &this->TextSize; }
				});
            // 文本颜色
            props.insert({
                "TextColor",
                PropertyType::Color,
                "Appearance",
                [this](void* val) { this->SetTextColor(*static_cast<ImU32*>(val)); },
                [this]() { return static_cast<void*>(&this->m_TextColor); }
                });

			props.insert
			({
				"HorizontalAlignment",
				PropertyType::Enum,
				"Layout",
				[this](void* val) {
					std::string str = *static_cast<std::string*>(val);
					if (str == "Left") halign = TextAlignment_Horizontal::Left;
					else if (str == "Center") halign = TextAlignment_Horizontal::Center;
					else if (str == "Right") halign = TextAlignment_Horizontal::Right;
				},
				[this]() {
					static std::vector<std::string> options;
					options = {"Left", "Center", "Right"}; // 枚举选项

					// 添加当前选中项
					switch (halign) {
						case TextAlignment_Horizontal::Left: options.push_back("Left"); break;
						case TextAlignment_Horizontal::Center: options.push_back("Center"); break;
						case TextAlignment_Horizontal::Right: options.push_back("Right"); break;
					}
					return static_cast<void*>(&options);
				}
			});

			// 垂直对齐 (使用Enum类型)
			props.insert
			({
				"VerticalAlignment",
				PropertyType::Enum,
				"Layout",
				[this](void* val) {
					std::string str = *static_cast<std::string*>(val);
					if (str == "Top") valign = TextAlignment_Vertical::Top;
					else if (str == "Center") valign = TextAlignment_Vertical::Center;
					else if (str == "Bottom") valign = TextAlignment_Vertical::Bottom;
				},
				[this]() {
					static std::vector<std::string> options;
					options = {"Top", "Center", "Bottom"}; // 枚举选项

					// 添加当前选中项
					switch (valign) {
						case TextAlignment_Vertical::Top: options.push_back("Top"); break;
						case TextAlignment_Vertical::Center: options.push_back("Center"); break;
						case TextAlignment_Vertical::Bottom: options.push_back("Bottom"); break;
					}
					return static_cast<void*>(&options);
				}
			});

			return props;
		}

        virtual std::string GetRegisterTypeName()override { return "ImTextBlock"; }

		virtual ImWidget* CopyWidget()
		{
			return new ImTextBlock(*this);
		}
	};
}