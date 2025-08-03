#pragma once
#include <imgui.h>
#include <string>
#include <imgui_internal.h>
#include <vector>

#include "ImWidgetProperty.h"
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
		class ImWidget* m_Parents;
		bool bSizeDirty;
		//处理子控件最小尺寸发生变化的情况
		virtual void HandleChildSizeDirty(){}

		//控件最小尺寸发生变化时调用
		virtual void MarkSizeDirty()
		{
			if (m_Parents)
			{
				m_Parents->HandleChildSizeDirty();
			}
		}
	public:
		ImWidget(const std::string& WidgetName)
			:m_WidgetName(WidgetName),
			m_Slot(nullptr),
			m_Parents(nullptr),
			bSizeDirty(false)
		{
			static int counter = 0;
			m_WidgetName += std::to_string(counter);
			counter++;
		}
		void SetParents(ImWidget* parents)
		{
			m_Parents = parents;
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
			ImVec2 Min = Position;
			ImVec2 Max = Position + Size;
			//if (Size.x > WidgetHitTestPadding*2&&Size.y> WidgetHitTestPadding * 2)
			//{
			//	Min.x += WidgetHitTestPadding;
			//	Min.y += WidgetHitTestPadding;
			//	Max.x -= WidgetHitTestPadding;
			//	Max.y -= WidgetHitTestPadding;
			//}
			if (ImRect(Min, Max).Contains(Pos))
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
		std::string GetWidgetName() { return m_WidgetName; }

		virtual std::vector<PropertyInfo> GetProperties()
		{
			return {};
		}
	};
}