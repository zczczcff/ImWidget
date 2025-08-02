#pragma once

#include "ImWidget/ImUserWidget.h"
#include "ImWidget/ImTextBlock.h"


enum class WidgetType
{
	ImButton,
	ImTextBlock,
	ImImage,
	ImCanvasPanel,
	ImVerticalBox,
	ImHorizontalBox
};


class ExampleWidgetDragHandle :public ImGuiWidget::ImDragHandle
{
public:

	WidgetType widgettype;

	ExampleWidgetDragHandle(const std::string& DisplayText,WidgetType widgettype):widgettype(widgettype)
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