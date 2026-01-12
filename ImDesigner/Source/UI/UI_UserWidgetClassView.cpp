#include "UI/UI_UserWidgetClassView.h"

inline UI_UserWidgetClassView::UI_UserWidgetClassView(const std::string& name, ImGuiWidget::ImUserWidgetClass* target)
	:ImUserWidget(name),
	TargetClass(target)
{

}

void UI_UserWidgetClassView::UpdateView()
{
}
