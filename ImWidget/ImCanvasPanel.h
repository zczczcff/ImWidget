#pragma once
#include "ImPanelWidget.h"

namespace ImGuiWidget
{
	class ImCanvasPanelSlot :public ImSlot
	{
	public:
		ImCanvasPanelSlot(ImWidget* Content):ImSlot(Content){}
		ImVec2 RelativePosition;
		ImVec2 SlotSize;
		void SetSlotPosAndSize(const ImVec2& relativepos, const ImVec2& size)
		{
			RelativePosition = relativepos;
			SlotSize = size;
		}
	};

	class ImCanvasPanel :public ImPanelWidget
	{
	private:

	protected:
		virtual void Relayout() override
		{
			for (int i = 0; i < GetSlotNum(); i++)
			{
				if (auto slot = (ImCanvasPanelSlot*)GetSlotAt(i))
				{
					slot->SetSlotPosition(Position + slot->RelativePosition);
					slot->SetSlotSize(slot->SlotSize);
					slot->ApplyLayout();
				}
			}
		}
	public:
		ImCanvasPanel(const std::string& WidgetName):ImPanelWidget(WidgetName){}

		ImCanvasPanelSlot* AddChildToCanvasPanel(ImWidget* Child)
		{
			auto slot = AddChildInternal<ImCanvasPanelSlot>(Child);
			slot->RelativePosition = ImVec2(0, 0);
			slot->SlotSize = Child->GetMinSize();
			return slot;
		}
		virtual ImSlot* AddChild(ImWidget* Child, ImVec2 RelativePosition = ImVec2(FLT_MIN, FLT_MIN))override
		{
			ImCanvasPanelSlot* slot = AddChildToCanvasPanel(Child);
			slot->RelativePosition = RelativePosition == ImVec2(FLT_MIN, FLT_MIN) ? ImVec2(0.f, 0.f) : RelativePosition;
			return  slot;
		}
		virtual ImVec2 GetMinSize()override
		{
			return Size;
		}
		virtual void Render() override
		{
			HandleLayout();
			RenderChild();
		}
		
	};
}