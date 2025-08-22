#pragma once
#include "ImWidget/ImUserWidget.h"
#include "ImWidget/ImTextBlock.h"
#include "ImWidget/ImButton.h"


#include "ExampleWidgetInfor.h"


class ExampleWidget : public ImGuiWidget::ImUserWidget
{
private:
	//ImGuiWidget::ImButton* m_Button;
	ImGuiWidget::ImTextBlock* m_Text;
	std::string Display;
	std::string widgettype;
public:
	ExampleWidget(const std::string& WidgetName, const std::string& DisplayText,const std::string& widgettype):ImUserWidget(WidgetName),widgettype(widgettype)
	{
		//m_Button = new ImGuiWidget::ImButton(WidgetName + "_Button");
		m_Text = new ImGuiWidget::ImTextBlock(WidgetName + "_Text");
		//m_Button->SetContent(m_Text);
		m_Text->SetText(DisplayText);
		Display = DisplayText;
		SetRootWidget(m_Text);
		SetAllowDrag(true);
		SetBackgroundColor(IM_COL32(200, 200, 200, 255));
		SetOnHovered(
			[this]()
			{
				SetBackgroundColor(IM_COL32(180, 180, 180, 255));
			}
		);
		SetOnUnhovered(
			[this]()
			{
				SetBackgroundColor(IM_COL32(200, 200, 200, 255));
			}
		);
		
	}

	virtual ImGuiWidget::ImDragHandle* OnDragBegin() override
	{
		ImGuiWidget::ImDragHandle* Handle = new ExampleWidgetDragHandle(Display, widgettype);

		return Handle;
	};

	virtual void OnDragCancel() override
	{
		printf("dragcancel");
	};
};