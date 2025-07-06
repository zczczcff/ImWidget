#pragma once
#include "ImPanelWidget.h"

namespace ImGuiWidget
{

	class ImScrollBox :public ImPanelWidget
	{
	public:
		ImScrollBox(const std::string& WidgetName):ImPanelWidget(WidgetName){}

		ImSlot* SetChild(ImWidget* child)
		{
			if (m_Slots.size() >= 1 && m_Slots[0])
			{
				if (m_Slots[0])
				{
					delete m_Slots[0];
				}
				m_Slots[0] = new ImSlot(child);
			}
			else
			{
				m_Slots.push_back(new ImSlot(child));
			}

			return m_Slots[0];
		}


		virtual void Render()override
		{
			ImVec2 MinSize(10.f, 10.f);
			ImGuiID id = ImGui::GetID(m_WidgetName.c_str());
			
			

			if (m_Slots.size() >= 1 && m_Slots[0])
			{
				MinSize = m_Slots[0]->GetContent()->GetMinSize();

				ImGui::SetCursorPos(Position);
				ImGui::BeginChild(id, Size, ImGuiChildFlags_Borders, ImGuiWindowFlags_HorizontalScrollbar);
				ImVec2 ScrollPosition(ImGui::GetScrollX(), ImGui::GetScrollY());
				m_Slots[0]->GetContent()->SetPosition(Position- ScrollPosition);
				m_Slots[0]->GetContent()->SetSize(MinSize);
				m_Slots[0]->GetContent()->Render();
				ImGui::SetCursorPos(ImVec2(0,0));
				ImGui::Dummy(MinSize);
				ImGui::EndChild();

			}
		}

	};
}