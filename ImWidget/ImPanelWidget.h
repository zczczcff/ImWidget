#pragma once
#include "ImWidget.h"
#include "ImSolt.h"
#include <vector>

namespace ImGuiWidget
{
	class ImPanelWidget:public ImWidget
	{
	protected:
		std::vector<ImSlot*> m_Slots;
	public:
		ImPanelWidget(const std::string& WidgetName):ImWidget(WidgetName){}

		template<typename SlotType>
		SlotType* AddChild(ImWidget* child)
		{
			SlotType* newslot = new SlotType(child);
			m_Slots.push_back(newslot);
			return newslot;
		}
		virtual void Render() override
		{

		}
	};
}