#pragma once
#include <imgui.h>
#include <string>

namespace ImGuiWidget
{
	class ImWidget;
	class ImSlot
	{
	protected:
		ImWidget* Content;
		bool bAutoSize = true;
		//用于缓存布局信息
		ImVec2 SlotPosition;
		ImVec2 SlotSize;
	public:
		ImSlot():Content(nullptr) {}
		ImSlot(ImWidget* Content):Content(Content)
		{
			if (Content)
			{
				Content->SetSlot(this);
			}
		}
		ImWidget* GetContent() { return Content; }
		virtual void ApplyLayout()
		{
			if (!Content)return;
			Content->SetPosition(SlotPosition);
			Content->SetSize(SlotSize);
		}
		bool GetIfAutoSize() { return bAutoSize; }
		void SetIfAutoSize(bool Value) { bAutoSize = Value; }
		void Render()
		{
			if (Content)
			{
				Content->Render();
			}
		}
		bool IsValid()
		{
			return Content;
		}
		void SetSlotPosition(ImVec2 Position)
		{
			SlotPosition = Position;
		}
		void SetSlotSize(ImVec2 Size)
		{
			SlotSize = Size;
		}

		virtual ~ImSlot()
		{
			//delete Content;
		}
	};

	class ImPaddingSlot :public ImSlot
	{
	public:
		float PaddingTop = 0.f;
		float PaddingBottom = 0.f;
		float PaddingLeft = 0.f;
		float PaddingRight = 0.f;

		ImPaddingSlot(){}
		ImPaddingSlot(ImWidget* Content):ImSlot(Content){}

		virtual void ApplyLayout()
		{
			if (!Content)return;
			ImVec2 Rect_Min(SlotPosition.x + PaddingLeft, SlotPosition.y + PaddingTop);
			ImVec2 Rect_Max(SlotPosition.x + SlotSize.x - PaddingRight, SlotPosition.y + SlotSize.y - PaddingBottom);
			ImVec2 WidgetSize = Rect_Max - Rect_Min;
			WidgetSize.x = ImMax(0.f, WidgetSize.x);
			WidgetSize.y = ImMax(0.f, WidgetSize.y);
			Content->SetPosition(Rect_Min);
			Content->SetSize(WidgetSize);
		}
	};


}