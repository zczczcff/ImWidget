#pragma once
#include "ImEvent/ImEvent.h"
namespace ImGuiWidget
{
	class ImEventSystem;
	class ImHoverInEvent :public ImEvent
	{
	private:
		ImEventSystem* m_EventSystem;
	public:
		ImHoverInEvent(ImEventSystem* EventSystem):
			ImEvent(ImEventType::HoverIn,false),
			m_EventSystem(EventSystem)
		{
		}
		virtual ImHoverInEvent* Clone()const 
		{
			return new ImHoverInEvent(*this);
		}
		//设置悬停计时;单位（s）
		void SetHoverTime_Second(float time);
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

	class ImHoverEvent :public ImEvent
	{
	private:
		ImEventSystem* m_EventSystem;
	public:
		ImHoverEvent(ImEventSystem* EventSystem) :
			ImEvent(ImEventType::Hover, false),
			m_EventSystem(EventSystem)
		{
		}
		virtual ImHoverEvent* Clone()const
		{
			return new ImHoverEvent(*this);
		}
		//重设悬停计时;单位（s）
		void ResetHoverTime_Second(float time);
	};
}