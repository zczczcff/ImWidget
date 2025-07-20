#pragma once
#include "ImWidget.h"
#include "ImSolt.h"
#include <imgui_internal.h>
#include <vector>

namespace ImGuiWidget
{
	class ImPanelWidget:public ImWidget
	{
	protected:
		std::vector<ImSlot*> m_Slots;
		ImU32 BgColor;
		ImU32 BorderColor;
		bool bHaveBorder;

	public:
		ImPanelWidget(const std::string& WidgetName)
			:ImWidget(WidgetName),
			BgColor(IM_COL32(255, 255, 255, 255)),
			BorderColor(IM_COL32(0, 0, 0, 255)),
			bHaveBorder(true)
		{}

		template<typename SlotType>
		SlotType* AddChild(ImWidget* child)
		{
			SlotType* newslot = new SlotType(child);
			m_Slots.push_back(newslot);
			return newslot;
		}
		virtual void Render() override
		{
			
		}

		void RenderBackGround()
		{
			ImGuiWindow* window = ImGui::GetCurrentWindow();
			window->DrawList->AddRectFilled(Position, Position + Size, BgColor);
			if (bHaveBorder)
			{
				window->DrawList->AddRect(Position, Position + Size, BorderColor);
			}
		}

		void SetBackGroundColor(ImU32 color) { BgColor = color; }

		void SetBorderColor(ImU32 color) { BorderColor = color; }


		int GetChildNum() { return m_Slots.size(); }

		ImSlot* GetSlot(int Index) { return m_Slots[Index]; }
	};
}