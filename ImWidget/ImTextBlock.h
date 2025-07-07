#pragma once
#include "ImWidget.h"
#include <imgui_internal.h>


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
		std::string m_Text;
        TextAlignment_Horizontal halign = TextAlignment_Horizontal::Center;
        TextAlignment_Vertical valign = TextAlignment_Vertical::Center;
	public:
        ImTextBlock(const std::string& WidgetName):ImWidget(WidgetName){}
        virtual ImVec2 GetMinSize() 
        { 
            return ImVec2(ImGui::CalcTextSize(m_Text.c_str()).x, 16.f);//需根据字体计算，待修改
        }
		void SetText(const std::string& Text)
		{
			m_Text = Text;
		}

		virtual void Render() 
		{
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
            window->DrawList->AddText(textPos, ImGui::GetColorU32(ImGuiCol_Text), m_Text.c_str());
		}
	};
}