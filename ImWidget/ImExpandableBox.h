#pragma once
#include "ImPanelWidget.h"


namespace ImGuiWidget
{
	class ImExpandableBox :public ImPanelWidget
	{
	private:
		bool bIsExpanded;
		float TriangleSize = 7.f;
		float HeadPad;

		ImVec2 p0, p1, p2, p3, p4, p5;//TrianglePoints
		ImRect buttonrect_NotExpanded, buttonrect_Expanded;
		ImU32 TriangleColor;
	public:
		ImExpandableBox(const std::string& WidgetName):
			ImPanelWidget(WidgetName),
			bIsExpanded(false),
			TriangleColor(IM_COL32(0,0,0,255))
		{
			ReCaculateTriangle();
		}
		ImSlot* SetHead(ImWidget* HeadWidget)
		{
			if (m_Slots.size() >= 1)
			{
				if (m_Slots[0])
				{
					delete m_Slots[0];
				}
				m_Slots[0] = new ImSlot(HeadWidget);
			}
			else
			{
				m_Slots.push_back(new ImSlot(HeadWidget));
			}
			
			return m_Slots[0];
		}
		ImSlot* SetBody(ImWidget* BodyWidget)
		{
			if (m_Slots.size() >= 2)
			{
				if (m_Slots[1])
				{
					delete m_Slots[1];
				}
				m_Slots[1] = new ImSlot(BodyWidget);
			}
			else if (m_Slots.size() == 1)
			{
				m_Slots.push_back(new ImSlot(BodyWidget));
			}
			else
			{
				m_Slots.push_back(nullptr);
				m_Slots.push_back(new ImSlot(BodyWidget));
			}

			return m_Slots[1];
		}

		void ReCaculateTriangle()
		{
			HeadPad = 2 * TriangleSize;
			p0 = ImVec2
			(
				(2*TriangleSize-TriangleSize/1.414f)/2,
				(HeadPad - 1.414f * TriangleSize) / 2.f
			);
			p1 = ImVec2
			(
				(2 * TriangleSize + TriangleSize / 1.414f) / 2,
				HeadPad / 2.f
			);
			p2 = ImVec2
			(
				(2 * TriangleSize - TriangleSize / 1.414f) / 2,
				(HeadPad + 1.414f * TriangleSize) / 2.f
			);

			buttonrect_NotExpanded = ImRect
			(
				p0.x - 1.f,
				p0.y - 1.f,
				p1.x + 1.f,
				p2.y + 1.f
			);

			p3 = ImVec2
			(
				(HeadPad - 1.414f * TriangleSize) / 2.f,
				(HeadPad - TriangleSize / 1.414f) / 2.f
			);

			p4 = ImVec2
			(
				HeadPad/2.f,
				(HeadPad + TriangleSize / 1.414f) / 2.f
			);

			p5 = ImVec2
			(
				(HeadPad + 1.414f * TriangleSize) / 2.f,
				(HeadPad - TriangleSize / 1.414f) / 2.f
			);

			buttonrect_Expanded = ImRect
			(
				p3.x - 1.f,
				p3.y - 1.f,
				p5.x + 1.f,
				p4.y + 1.f
			);
		}

		virtual ImVec2 GetMinSize()
		{
			ImVec2 MinSize = ImVec2(HeadPad, HeadPad);
			if (m_Slots.size() >= 1 && m_Slots[0])
			{
				MinSize.x += m_Slots[0]->GetContent()->GetMinSize().x;
				MinSize.y = max(MinSize.y, m_Slots[0]->GetContent()->GetMinSize().y);
			}

			if (m_Slots.size() >= 2 && m_Slots[1] && bIsExpanded)
			{
				MinSize.x = max(MinSize.x, m_Slots[1]->GetContent()->GetMinSize().x);
				MinSize.y += m_Slots[1]->GetContent()->GetMinSize().y;
			}
			return MinSize;
		}

		virtual void Render()override
		{
			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			ImGuiWindow* window = ImGui::GetCurrentWindow();
			const ImGuiID id = window->GetID(m_WidgetName.c_str());
			bool hovered, held;
			if (bIsExpanded)
			{
				draw_list->AddTriangleFilled(p3 + Position, p4 + Position, p5 + Position, TriangleColor);
				ImRect bb(Position + buttonrect_Expanded.Min, Position + buttonrect_Expanded.Max);
				ImGui::ItemAdd(bb, id);
				bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, 0);
				if (pressed)
				{
					bIsExpanded = false;
				}
				
			}
			else
			{
				draw_list->AddTriangle(p0 + Position, p1 + Position, p2 + Position, TriangleColor);
				ImRect bb(Position + buttonrect_NotExpanded.Min, Position + buttonrect_NotExpanded.Max);
				ImGui::ItemAdd(bb, id);
				bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, 0);
				if (pressed)
				{
					bIsExpanded = true;
				}
			}
			

			float headhight = HeadPad;
			if(m_Slots.size()>=1&&m_Slots[0])
			{
				m_Slots[0]->GetContent()->SetPosition(Position + ImVec2(HeadPad, 0.f));
				ImVec2 minsize = m_Slots[0]->GetContent()->GetMinSize();
				headhight = max(headhight, minsize.y);
				m_Slots[0]->GetContent()->SetSize(ImVec2(minsize.x, headhight));
				m_Slots[0]->GetContent()->Render();
			}

			if (bIsExpanded && m_Slots.size() >= 2 && m_Slots[1])
			{
				m_Slots[1]->GetContent()->SetPosition(Position + ImVec2(0, headhight));
				ImVec2 minsize = m_Slots[1]->GetContent()->GetMinSize();
				m_Slots[1]->GetContent()->SetSize(minsize);
				m_Slots[1]->GetContent()->Render();
			}
		}
	};
}