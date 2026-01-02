#include "ImWidget/ImWidget.h"
#include "ImWidget/ImPanelWidget.h"
#include "ImEvent/ImEvent.h"
// 事件系统新增：事件处理入口


//控件最小尺寸发生变化时调用

void ImGuiWidget::ImWidget::MarkSizeDirty()
{
	ImWidget* p = GetParents();
	while (p)
	{
		p->HandleChildSizeDirty();
		p = p->GetParents();
	}
}

void ImGuiWidget::ImWidget::HandleEvent(ImEvent* event)
{
	// 调用虚函数处理（子类可以重写）
	if (!event->IsHandled())
	{
		HandleEventInternal(event);
	}
}
