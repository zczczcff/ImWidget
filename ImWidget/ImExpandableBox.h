#pragma once
#include "ImPanelWidget.h"


namespace ImGuiWidget
{
	class ImExpandableBox :public ImPanelWidget
	{
	private:
		bool bIsExpanded;
		float TriangleSize = 5.f;
		float HeadPad;

		ImVec2 p0, p1, p2;
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
			HeadPad = 1.414f * TriangleSize;
			p0 = ImVec2
			(
				HeadPad / 4.f,
				(HeadPad - 1.414f * TriangleSize) / 2.f
			);
			p1 = ImVec2
			(
				HeadPad * 3 / 4.f,
				HeadPad / 2.f
			);
			p2 = ImVec2
			(
				HeadPad / 4.f,
				(HeadPad + 1.414f * TriangleSize) / 2.f
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
			draw_list->AddTriangle(p0 + Position, p1 + Position, p2 + Position, TriangleColor);

			float headhight = HeadPad;
			if(m_Slots.size()>=1&&m_Slots[0])
			{
				m_Slots[0]->GetContent()->SetPosition(Position + ImVec2(HeadPad, 0.f));
				headhight = max(headhight, m_Slots[0]->GetContent()->GetMinSize().y);
				m_Slots[0]->GetContent()->Render();
			}

			if (bIsExpanded && m_Slots.size() >= 2 && m_Slots[1])
			{
				m_Slots[1]->GetContent()->SetPosition(Position + ImVec2(0, headhight));
				m_Slots[1]->GetContent()->Render();
			}
		}
	};
}