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
	public:
		ImCanvasPanel(const std::string& WidgetName):ImPanelWidget(WidgetName){}

		ImCanvasPanelSlot* AddChildToCanvasPanel(ImWidget* Child)
		{
			return AddChild<ImCanvasPanelSlot>(Child);
		}
		virtual ImVec2 GetMinSize()override
		{
			return Size;
		}
		virtual void Render() override
		{
			for (auto& slot : m_Slots)
			{
				ImCanvasPanelSlot* CPSlot = (ImCanvasPanelSlot*)slot;
				CPSlot->GetContent()->SetPosition(Position + CPSlot->RelativePosition);
				CPSlot->GetContent()->SetSize(CPSlot->SlotSize);
				CPSlot->GetContent()->Render();
			}
		}

	};
}