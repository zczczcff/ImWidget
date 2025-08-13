#pragma once

#include "ImWidget/ImUserWidget.h"
#include "ImWidget/ImTextBlock.h"


enum class WidgetType
{
	ImButton,
	ImCanvasPanel,
	ImCheckBox,
	ImColorPicker,
	ImComboBox,
	ImDesignPanel,
	ImExpandableBox,
	ImHorizontalBox,
	ImHorizontalSplitter,
	ImImage,
	ImInputText,
	ImIntInput,
	ImFloatInput,
	ImMultiLineTextBlock,
	ImResizableBox,
	ImScrollBox,
	ImScrollingTextList,
	ImSlider,
	ImTextBlock,
	ImVerticalBox,
	ImVerticalSplitter,
	ImSimpleFigure
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