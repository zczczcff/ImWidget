#include "ImWidget/ImWidget.h"
#include "ImWidget/ImPanelWidget.h"
#include "ImEvent/ImEvent.h"
#include "ImEvent/ImHoverEvent.h"
#include "ImGlobal.h"
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
		if (event->GetType() == ImEventType::HoverIn&&bHoverable)
		{
			bHovered = true;
			OnHoverStart();
			event->As<ImHoverInEvent>()->SetHoverTime_Second(m_HoverTime);
		}
		else if (event->GetType() == ImEventType::Hover && bHoverable)
		{
			OnHover();
		}
		else if (event->GetType() == ImEventType::HoverOut && bHoverable)
		{
			bHovered = false;
			OnHoverEnd();
		}
		HandleEventInternal(event);
	}
}

// 请求焦点

bool ImGuiWidget::ImWidget::RequestFocus()
{
	if (!IsFocusable()) return false;

	ImGuiWidget::RequestFocus(this);
}
