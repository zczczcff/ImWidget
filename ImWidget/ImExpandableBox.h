#pragma once
#include "ImPanelWidget.h"
#include <functional>

namespace ImGuiWidget
{
	class ImExpandableBox :public ImPanelWidget
	{
	private:
		bool bIsExpanded;
		float TriangleSize = 7.f;
		float HeadPad;
		float BodyPad=6.f;

		ImVec2 p0, p1, p2, p3, p4, p5;//TrianglePoints
		ImRect buttonrect_NotExpanded, buttonrect_Expanded;
		ImU32 TriangleColor;

		std::function<void(bool)> OnExpandedStateChanged;
	protected:
		virtual ImSlot* CreateSlot(ImWidget* Content)
		{
			return new ImPaddingSlot(Content);
		}
		virtual void Relayout() override
		{
			float headhight = HeadPad;

			if (auto slot = GetSlotAt(0))
			{
				slot->SetSlotPosition(Position + ImVec2(HeadPad, 0.f));
				ImVec2 minsize(0.f, 0.f);
				if (auto widget = slot->GetContent())
				{
					minsize = widget->GetMinSize();
				}
				headhight = ImMax(headhight, minsize.y);
				float headlength = ImMax(minsize.x, Size.x - HeadPad);
				slot->SetSlotSize(ImVec2(minsize.x, headhight));
				slot->ApplyLayout();
			}
			if (bIsExpanded)
			{
				if (auto slot = GetSlotAt(1))
				{
					slot->SetSlotPosition(Position + ImVec2(BodyPad, headhight));

					
					if (auto widget = slot->GetContent())
					{
						float bodylength = ImMax(widget->GetMinSize().x, Size.x - BodyPad);
						slot->SetSlotSize(ImVec2(bodylength,widget->GetMinSize().y));
					}
					slot->ApplyLayout();
				}
			}
		}
	public:
		ImExpandableBox(const std::string& WidgetName):
			ImPanelWidget(WidgetName),
			bIsExpanded(false),
			TriangleColor(IM_COL32(0,0,0,255))
		{
			ReCaculateTriangle();
		}
		ImPaddingSlot* SetHead(ImWidget* HeadWidget)
		{
			SetChildAt(0, HeadWidget);
			return (ImPaddingSlot*)GetSlotAt(0);
		}
		ImPaddingSlot* SetBody(ImWidget* BodyWidget)
		{
			SetChildAt(1, BodyWidget);
			return (ImPaddingSlot*)GetSlotAt(1);
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
			ImVec2 minSize(HeadPad, HeadPad);

			if (ImSlot* headSlot = GetSlot(0))
			{
				if (ImWidget* content = headSlot->GetContent())
				{
					const ImVec2 headMin = content->GetMinSize();
					minSize.x += headMin.x;
					minSize.y = ImMax(minSize.y, headMin.y);
				}
			}
			if (bIsExpanded)
			{
				if (ImSlot* bodySlot = GetSlot(1))
				{
					if (ImWidget* content = bodySlot->GetContent())
					{
						const ImVec2 bodyMin = content->GetMinSize();
						minSize.y += bodyMin.y;
						minSize.x = ImMax(minSize.x, bodyMin.x);
					}
				}
			}
			return minSize;
		}

		virtual void Render()override
		{
			RenderBackGround();
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
					MarkSizeDirty();
					SetLayoutDirty();
					if (OnExpandedStateChanged)
					{
						OnExpandedStateChanged(bIsExpanded);
					}
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
					MarkSizeDirty();
					SetLayoutDirty();
					if (OnExpandedStateChanged)
					{
						OnExpandedStateChanged(bIsExpanded);
					}
				}
			}


			HandleLayout();
			
			if (auto head = GetChildAt(0))
			{
				head->Render();
			}

			if (bIsExpanded)
			{
				if (auto body = GetChildAt(1))
				{
					body->Render();
				}
			}
			//if(m_Slots.size()>=1&&m_Slots[0])
			//{
			//	m_Slots[0]->GetContent()->SetPosition(Position + ImVec2(HeadPad, 0.f));
			//	ImVec2 minsize = m_Slots[0]->GetContent()->GetMinSize();
			//	headhight = max(headhight, minsize.y);
			//	m_Slots[0]->GetContent()->SetSize(ImVec2(minsize.x, headhight));
			//	m_Slots[0]->GetContent()->Render();
			//}

			//if (bIsExpanded && m_Slots.size() >= 2 && m_Slots[1])
			//{
			//	m_Slots[1]->GetContent()->SetPosition(Position + ImVec2(0, headhight));
			//	ImVec2 minsize = m_Slots[1]->GetContent()->GetMinSize();
			//	m_Slots[1]->GetContent()->SetSize(minsize);
			//	m_Slots[1]->GetContent()->Render();
			//}
		}
		
		bool GetIfExpanded() { return bIsExpanded; }

		void SetExpandedState(bool newstate)
		{
			bIsExpanded = newstate; 
			MarkSizeDirty();
			SetLayoutDirty();
			if (OnExpandedStateChanged)
			{
				OnExpandedStateChanged(bIsExpanded);
			}
		}

		void SetOnExpandedStateChanged(std::function<void(bool)> callback) { OnExpandedStateChanged = callback; }
	};
}