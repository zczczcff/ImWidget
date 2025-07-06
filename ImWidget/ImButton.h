#pragma once

#include "ImPanelWidget.h"
#include "imgui_internal.h"
#include <functional>



namespace ImGuiWidget
{
	class ImButton :public ImPanelWidget
	{
	protected:
		std::function<void(void)> OnClicked;
		ImGuiButtonFlags m_ButtonFlag = 0;
	public:
		ImButton(const std::string& WidgetName) :ImPanelWidget(WidgetName) {}
		void SetChild(ImWidget* child)
		{
			ImSlot* newslot = new ImSlot(child);
			if (m_Slots.size() == 1)
			{
				delete m_Slots[0];
				m_Slots[0] = newslot;
			}
			else
			{
				m_Slots.push_back(newslot);
			}
		}

		virtual ImVec2 GetMinSize()
		{
			return ImVec2(10.f, 30.f);
		}
		virtual void Render()
		{
			ImGuiWindow* window = ImGui::GetCurrentWindow();
			ImGuiContext& g = *GImGui;
			const ImGuiStyle& style = g.Style;
			const ImGuiID id = window->GetID(m_WidgetName.c_str());

			ImRect bb(Position, Position + Size);

			ImGui::ItemSize(Size, style.FramePadding.y);

			if (!ImGui::ItemAdd(bb, id))
			{

			}

			bool hovered, held;
			bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, m_ButtonFlag);
			window->DrawList->AddRectFilled(bb.Min, bb.Max, ImGui::GetColorU32(ImGuiCol_ButtonActive), 2);
			//window->DrawList->AddRect(bb.Min + ImVec2(1, 1), p_max + ImVec2(1, 1), GetColorU32(ImGuiCol_BorderShadow), rounding, 0, border_size);


			if (m_Slots.size() == 1)
			{
				m_Slots[0]->GetContent()->SetPosition(Position);
				m_Slots[0]->GetContent()->SetSize(Size);
				m_Slots[0]->GetContent()->Render();
			}

		}
	};
}