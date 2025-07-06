#pragma once

#include "ImWidget.h"
#include <imgui_internal.h>
#include <imstb_textedit.h>

namespace ImGuiWidget
{
	class ImSingleLineInPutText :public ImWidget
	{
	private:
		char m_buf[128];
		std::string LabelSuffix;
		std::string Label;
		ImVec4 BgColor;
	public:
		ImSingleLineInPutText(const std::string& WidgetName):ImWidget(WidgetName), m_buf(),BgColor(0.8f, 0.2f, 0.2f, 0.5f)
		{
			Label = "##" + WidgetName;
		}
		virtual void Render() 
		{
			//ImDrawList* draw_list = ImGui::GetWindowDrawList();
			//draw_list->AddRectFilled(Position, Position+Size,ImGui::ColorConvertFloat4ToU32(BgColor));
			const ImVec2 text_pos(Position.x, Position.y + (Size.y - 16.f) * 0.5f);
			ImGui::SetCursorPos(text_pos);
			ImGui::PushStyleColor(ImGuiCol_FrameBg, BgColor); // RGBA (红色)
			ImGui::InputTextEx(Label.c_str(), "", m_buf, 128, Size, ImGuiInputTextFlags_NoHorizontalScroll);
			ImGui::PopStyleColor(); // 恢复之前的颜色
		}
		virtual ImVec2 GetMinSize() { return ImVec2(10.f, 20.f); }
	};
}