#include "ImEvent/ImHoverEvent.h"
#include "ImEvent/ImEventSystem.h"
void ImGuiWidget::ImHoverInEvent::SetHoverTime_Second(float time)
{
	m_EventSystem->ResetHoverTime(time);
}

void ImGuiWidget::ImHoverEvent::ResetHoverTime_Second(float time)
{
	m_EventSystem->ResetHoverTime(time);
}
