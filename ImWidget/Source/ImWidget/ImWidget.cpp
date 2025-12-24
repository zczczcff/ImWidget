#include "ImWidget/ImWidget.h"

#include "ImEvent/ImEvent.h"
// 事件系统新增：事件处理入口

void ImGuiWidget::ImWidget::HandleEvent(ImEvent* event)
{
	// 调用虚函数处理（子类可以重写）
	if (!event->IsHandled())
	{
		HandleEventInternal(event);
	}
}
