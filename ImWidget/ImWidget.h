#pragma once
#include <imgui.h>
#include <string>
#include <imgui_internal.h>


namespace ImGuiWidget
{
	class ImWidget
	{
	protected:
		std::string m_WidgetName;
		ImVec2 Position = { 0, 0 };  // 相对位置
		ImVec2 Size = { 0, 0 };      // 控件尺寸
		bool Visibility = true;     // 可见性
		class ImSlot* m_Slot;
	public:
		ImWidget(const std::string& WidgetName)
			:m_WidgetName(WidgetName),
			m_Slot(nullptr)
		{

		}
		void SetSlot(ImSlot* slot)
		{
			m_Slot = slot;
		}
		ImSlot* GetSlot()
		{
			return m_Slot;
		}
		virtual ImWidget* ChildHitTest(ImVec2 Pos)
		{
			if (ImRect(Position, Position + Size).Contains(Pos))
				return this;
			else
				return nullptr;
		}
		virtual void SetPosition(ImVec2 Pos) { Position = Pos; }
		virtual void SetSize(ImVec2 size) { Size = size; }
		ImVec2 GetPosition() { return Position; }
		ImVec2 GetSize() { return Size; }
		ImRect GetRect() { return ImRect(Position, Position + Size); }
		virtual void Render(){}
		virtual ImVec2 GetMinSize() { return ImVec2(-1.f, -1.f); }
	};
}