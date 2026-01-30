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
			if (GetChildNum() > 0 && GetSlotAt(0) && GetSlotAt(0)->GetContent()) 
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
			return new ImPaddingSlot(Content,this);
		}
		virtual int GetAllowMaxChildNum()override { return 1; }

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

		DECLARE_IMOBJECT(ImBorder, ImPanelWidget)
		END_DECLARE_IMOBJECT()
	};
}
