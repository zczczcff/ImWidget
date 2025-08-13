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
	};
}
