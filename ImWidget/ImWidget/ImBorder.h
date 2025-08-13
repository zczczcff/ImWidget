#pragma once
#include "ImPanelWidget.h"

namespace ImGuiWidget
{
	class ImBorder :public ImPanelWidget
	{
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

		void SetContent(ImWidget* Content)
		{
			SetChildAt(0, Content);
		}
	};
}
