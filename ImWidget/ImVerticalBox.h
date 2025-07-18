#pragma once
#include "ImPanelWidget.h"


namespace ImGuiWidget
{
	class ImVerticalBoxSlot :public ImSlot
	{
	public:
		ImVerticalBoxSlot(ImWidget* Content):ImSlot(Content){}

		float SizeRatio = 1.f;
	};


	class ImVerticalBox :public ImPanelWidget
	{
	private:
		
	public:
		ImVerticalBox(const std::string& WidgetName):ImPanelWidget(WidgetName){}
		ImVerticalBoxSlot* AddChildToVerticalBox(ImWidget* child)
		{
			return AddChild<ImVerticalBoxSlot>(child);
		}

		virtual void Render() override
		{

			ReLayOut();

			RenderBackGround();

			for (auto& slot : m_Slots)
			{
				slot->GetContent()->Render();
			}

		}

		virtual ImVec2 GetMinSize()
		{
			float minheight = 0.f;
			float minlength = 0.f;
			for (auto& child : m_Slots)
			{
				ImVec2 childminsize = child->GetContent()->GetMinSize();
				float childminwidth = child->PaddingLeft + child->PaddingRight+ childminsize.x;
				minlength = ImMax(minlength, childminwidth);
				minheight += child->PaddingBottom;
				minheight += child->PaddingTop;
				minheight += childminsize.y;
			}

			return ImVec2(minlength, minheight);
		}



		void ReLayOut()
		{
			float RequiredHight = 0.f;
			float RemainedHight = 0.f;
			float SumSizeRatio = 0.f;

			std::vector<float> AutoSizeSlotsHight;

			for (auto& slot : m_Slots)
			{
				ImVerticalBoxSlot* VSlot = (ImVerticalBoxSlot*)slot;
				if (!slot->GetIfAutoSize())
				{
					ImVec2 MinSize = slot->GetContent()->GetMinSize();
					RequiredHight += (MinSize.y + VSlot->PaddingTop + VSlot->PaddingBottom);
					slot->GetContent()->SetSize(ImVec2(Size.x - VSlot->PaddingLeft - VSlot->PaddingRight, MinSize.y));
				}
				else
				{
					SumSizeRatio += VSlot->SizeRatio;
				}
			}
			RemainedHight = Size.y - RequiredHight;

			ImVec2 CurrentPos = Position;
			if (RemainedHight > 0.f)
			{
				for (auto& slot : m_Slots)
				{
					ImVerticalBoxSlot* VSlot = (ImVerticalBoxSlot*)slot;
					slot->GetContent()->SetPosition(ImVec2(CurrentPos.x + VSlot->PaddingLeft, CurrentPos.y + VSlot->PaddingTop));
					if (!slot->GetIfAutoSize())
					{
						CurrentPos.y += (slot->GetContent()->GetMinSize().y + VSlot->PaddingTop + VSlot->PaddingBottom);
					}
					else
					{
						float RatioHight = (VSlot->SizeRatio / SumSizeRatio) * RemainedHight;
						VSlot->GetContent()->SetSize(ImVec2(Size.x - VSlot->PaddingLeft - VSlot->PaddingRight, RatioHight - VSlot->PaddingTop - VSlot->PaddingBottom));
						CurrentPos.y += RatioHight;
					}
				}
			}
			else
			{
				for (auto& slot : m_Slots)
				{
					ImVerticalBoxSlot* VSlot = (ImVerticalBoxSlot*)slot;
					slot->GetContent()->SetPosition(ImVec2(CurrentPos.x + VSlot->PaddingLeft, CurrentPos.y + VSlot->PaddingTop));
					if (!slot->GetIfAutoSize())
					{
						CurrentPos.y += (slot->GetContent()->GetMinSize().y + VSlot->PaddingTop + VSlot->PaddingBottom);
					}
					else
					{
						slot->GetContent()->SetSize(ImVec2(0.f, 0.f));
					}
				}
			}
		}
	};
}