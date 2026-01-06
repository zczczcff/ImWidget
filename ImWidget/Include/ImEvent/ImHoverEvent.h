#pragma once
#include "ImEvent/ImEvent.h"
namespace ImGuiWidget
{
	class ImHoverInEvent :public ImEvent
	{
	public:
		ImHoverInEvent():ImEvent(ImEventType::HoverIn,false)
		{
		}
		virtual ImHoverInEvent* Clone()const 
		{
			return new ImHoverInEvent(*this);
		}
	};

	class ImHoverOutEvent :public ImEvent
	{
	public:
		ImHoverOutEvent() :ImEvent(ImEventType::HoverOut, false)
		{
		}
		virtual ImHoverOutEvent* Clone()const
		{
			return new ImHoverOutEvent(*this);
		}
	};
}