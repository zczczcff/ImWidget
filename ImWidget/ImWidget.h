#pragma once
#include <imgui.h>
#include <string>



namespace ImGuiWidget
{
	class ImWidget
	{
	protected:
		std::string m_WidgetName;
		ImVec2 Position = { 0, 0 };  // 相对位置
		ImVec2 Size = { 0, 0 };      // 控件尺寸
		bool Visibility = true;     // 可见性
	public:
		ImWidget(const std::string& WidgetName):m_WidgetName(WidgetName)
		{

		}
		void SetPosition(ImVec2 Pos) { Position = Pos; }
		void SetSize(ImVec2 size) { Size = size; }
		ImVec2 GetPosition() { return Position; }
		ImVec2 GetSize() { return Size; }
		virtual void Render(){}
		virtual ImVec2 GetMinSize() { return ImVec2(-1.f, -1.f); }
	};
}