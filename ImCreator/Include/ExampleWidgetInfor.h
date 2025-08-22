#pragma once

#include "ImWidget/ImUserWidget.h"
#include "ImWidget/ImTextBlock.h"
#include "ImWidget/ImWidgetFactory.h"

//enum class WidgetType
//{
//	ImButton,
//	ImCanvasPanel,
//	ImCheckBox,
//	ImColorPicker,
//	ImComboBox,
//	ImDesignPanel,
//	ImExpandableBox,
//	ImHorizontalBox,
//	ImHorizontalSplitter,
//	ImImage,
//	ImInputText,
//	ImIntInput,
//	ImFloatInput,
//	ImMultiLineTextBlock,
//	ImResizableBox,
//	ImScrollBox,
//	ImScrollingTextList,
//	ImSlider,
//	ImTextBlock,
//	ImVerticalBox,
//	ImVerticalSplitter,
//	ImSimpleFigure,
//	ImBorder
//};

ImGuiWidget::ImWidget* CreateNewWidget(const std::string& type)
{
	static int count = 0;
	ImGuiWidget::ImWidget* NewWidget = nullptr;
	NewWidget = ImGuiWidget::ImWidgetFactory::GetInstance().CreateWidget(type, type + "_" + std::to_string(count));
	/*switch (type)
	{
	case WidgetType::ImButton:
	{
		NewWidget = new ImGuiWidget::ImButton("Button_" + std::to_string(count));
		break;
	}
	case WidgetType::ImTextBlock:
	{
		NewWidget = new ImGuiWidget::ImTextBlock("TextBlock_" + std::to_string(count));
		break;
	}
	case WidgetType::ImCheckBox:
	{
		NewWidget = new ImGuiWidget::ImCheckBox("ImCheckBox");
		break;
	}
	case WidgetType::ImImage:
	{
		NewWidget = new ImGuiWidget::ImImage("Image_" + std::to_string(count));
		break;
	}
	case WidgetType::ImCanvasPanel:
	{
		NewWidget = new ImGuiWidget::ImCanvasPanel("CanvasPanel_" + std::to_string(count));
		break;
	}
	case WidgetType::ImHorizontalBox:
	{
		NewWidget = new ImGuiWidget::ImHorizontalBox("HorizontalBox_" + std::to_string(count));
		break;
	}
	case WidgetType::ImVerticalBox:
	{
		NewWidget = new ImGuiWidget::ImVerticalBox("VerticalBox_" + std::to_string(count));
		break;
	}
	case WidgetType::ImComboBox:
	{
		NewWidget = new ImGuiWidget::ImComboBox("ComboBox_" + std::to_string(count));
		break;
	}
	case WidgetType::ImVerticalSplitter:
	{
		NewWidget = new ImGuiWidget::ImVerticalSplitter("VerticalSplitter");
		break;
	}
	case WidgetType::ImHorizontalSplitter:
	{
		NewWidget = new ImGuiWidget::ImHorizontalSplitter("ImHorizontalSplitter");
		break;
	}
	case WidgetType::ImExpandableBox:
	{
		NewWidget = new ImGuiWidget::ImExpandableBox("ImExpandableBox");
		break;
	}
	case WidgetType::ImInputText:
	{
		NewWidget = new ImGuiWidget::ImInputText("ImInputText");
		break;
	}
	case WidgetType::ImIntInput:
	{
		NewWidget = new ImGuiWidget::ImIntInput("ImIntInput");
		break;
	}
	case WidgetType::ImFloatInput:
	{
		NewWidget = new ImGuiWidget::ImFloatInput("ImFloatInput");
		break;
	}
	case WidgetType::ImMultiLineTextBlock:
	{
		NewWidget = new ImGuiWidget::ImMultiLineTextBlock("ImMultiLineTextBlock");
		break;
	}
	case WidgetType::ImScrollBox:
	{
		NewWidget = new ImGuiWidget::ImScrollBox("ImScrollBox");
		break;
	}
	case WidgetType::ImScrollingTextList:
	{
		NewWidget = new ImGuiWidget::ImScrollingTextList("ImScrollingTextList");
		break;
	}
	case WidgetType::ImSlider:
	{
		NewWidget = new ImGuiWidget::ImSlider("ImSlider");
		break;
	}
	case WidgetType::ImSimpleFigure:
	{
		NewWidget = new ImGuiWidget::ImSimpleFigure("ImSimpleFigure");
		break;
	}
	case WidgetType::ImBorder:
	{
		NewWidget = new ImGuiWidget::ImBorder("ImBorder");
		break;
	}
	default:
		break;
	}*/
	count++;
	return NewWidget;
}

class ExampleWidgetDragHandle :public ImGuiWidget::ImDragHandle
{
public:

	std::string widgettype;

	ExampleWidgetDragHandle(const std::string& DisplayText,std::string widgettype):widgettype(widgettype)
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