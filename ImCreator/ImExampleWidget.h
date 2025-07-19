#pragma once
#include "ImWidget/ImUserWidget.h"
#include "ImWidget/ImTextBlock.h"


class ExampleWidgetDragHandle:public ImGuiWidget::ImDragHandle
{
public:
	ExampleWidgetDragHandle(const std::string& DisplayText)
	{
		ImGuiWidget::ImTextBlock* PreviewText = new  ImGuiWidget::ImTextBlock("test");
		PreviewText->SetText(DisplayText);
		Preview = PreviewText;
		
	}
	virtual ~ExampleWidgetDragHandle()
	{
		delete Preview;
	}
};


class ExampleWidget : public ImGuiWidget::ImUserWidget
{
private:
	ImGuiWidget::ImTextBlock* m_Text;
	std::string Display;
public:
	ExampleWidget(const std::string& WidgetName, const std::string& DisplayText):ImUserWidget(WidgetName)
	{
		m_Text = new ImGuiWidget::ImTextBlock(WidgetName + "_Text");
		m_Text->SetText(DisplayText);
		Display = DisplayText;
		SetRootWidget(m_Text);
		SetAllowDrag(true);
	}

	virtual ImGuiWidget::ImDragHandle* OnDragBegin() override
	{
		ImGuiWidget::ImDragHandle* Handle = new ExampleWidgetDragHandle(Display);

		return Handle;
	};

	virtual void OnDragCancel() override
	{
		printf("dragcancel");
	};
};