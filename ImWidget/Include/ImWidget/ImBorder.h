#pragma once
#include "ImPanelWidget.h"

namespace ImGuiWidget
{
	class ImBorder :public ImPanelWidget
	{
	private:

	protected:
		virtual void Relayout() override
		{
			if (GetSlotNum() > 0 && GetSlotAt(0) && GetSlotAt(0)->GetContent()) 
			{
				ImSlot* slot = GetSlotAt(0);
				ImWidget* content = slot->GetContent();
				if (bHaveBorder)
				{
					slot->SetSlotPosition(Position + ImVec2(BorderThickness, BorderThickness));
					slot->SetSlotSize(Size - ImVec2(BorderThickness, BorderThickness) * 2);
				}
				else
				{
					slot->SetSlotPosition(Position);
					slot->SetSlotSize(Size);
				}

				slot->ApplyLayout();
			}
		}
	public:
		ImBorder(const std::string& WidgetName):ImPanelWidget(WidgetName){}
		virtual ImSlot* CreateSlot(ImWidget* Content)
		{
			return new ImPaddingSlot(Content);
		}

		virtual ImSlot* AddChild(ImWidget* child, ImVec2 RelativePosition = ImVec2(FLT_MIN, FLT_MIN))
		{
			if (GetSlotNum() >= 1)
			{
				return nullptr;
			}

			return AddChildInternal<ImPaddingSlot>(child);
		}

		void SetContent(ImWidget* Content,bool DeleteOld = true)
		{
			SetChildAt(0, Content, DeleteOld);
		}
		ImWidget* GetContent()
		{
			return GetChildAt(0);
		}
		virtual void Render()override
		{
			HandleLayout();
			RenderBackGround();
			RenderChild();
		}

		virtual std::string GetRegisterTypeName() override { return "ImBorder"; }

		virtual ImWidget* CopyWidget() override
		{
			return new ImBorder(*this);
		}
	};
}
