#include "Widget_ExampleWidgetButton.h"
#include "ImBasicWidgetList.h"
#include "UI/DragObject_ExampleWidget.h"


void Widget_ExampleWidgetButton::Init2()
{
	ImButton_ExampleButton->SetFocusable(false);
	bAllowDrag = true;
	ImImage_Icon->SetOriginalSize(24, 24);
}


void Widget_ExampleWidgetButton::SetDisplayName(const std::string& displayname)
{
	ImTextBlock_WidgetTypeName->SetText(displayname);
	m_DisplayTypeName = displayname;
}

void Widget_ExampleWidgetButton::SetIcon(ImTextureID texture)
{
	ImImage_Icon->SetTextureID(texture);
}

void Widget_ExampleWidgetButton::OnDragStart(ImGuiWidget::ImDragStartEvent& e)
{
	if (e.GetPhase() == ImGuiWidget::ImEventPhase::Capture) return;
	
	e.SetDragData(std::make_shared<DragObject_ExampleWidget>(m_WidgetTypeName, m_DisplayTypeName));
}