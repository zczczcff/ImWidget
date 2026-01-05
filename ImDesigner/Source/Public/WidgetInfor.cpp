#include "Public/WidgetInfor.h"
#include "ImWidget/ImWidgetFactory.h"

std::vector<WidgetInfor> BasicWidgetList::GetBasicWidgetList()
{
	std::vector<WidgetInfor> BasicWidgetList;

	// 添加所有基本控件到列表中
	BasicWidgetList.push_back({ "Button", u8"按钮", "ImButton" });
	BasicWidgetList.push_back({ "CheckBox", u8"勾选框", "ImCheckBox" });
	BasicWidgetList.push_back({ "ComboBox", u8"组合框", "ImComboBox" });
	BasicWidgetList.push_back({ "Image", u8"图片", "ImImage" });
	BasicWidgetList.push_back({ "InputText", u8"文本输入框", "ImInputText" });
	BasicWidgetList.push_back({ "IntInput", u8"整数输入框", "ImIntInput" });
	BasicWidgetList.push_back({ "FloatInput", u8"浮点数输入框", "ImFloatInput" });
	BasicWidgetList.push_back({ "MultiLineTextBlock", u8"多行文本块", "ImMultiLineTextBlock" });
	BasicWidgetList.push_back({ "ScrollingTextList", u8"滚动文本列表", "ImScrollingTextList" });
	BasicWidgetList.push_back({ "Slider", u8"滑动条", "ImSlider" });
	BasicWidgetList.push_back({ "TextBlock", u8"文本块", "ImTextBlock" });
	BasicWidgetList.push_back({ "SimpleFigure", u8"简单图形", "ImSimpleFigure" });
	//BasicWidgetList.push_back({ "Curve", u8"曲线图", "ImCurve" });

	// 添加布局和容器控件
	BasicWidgetList.push_back({ "Border", u8"边框", "ImBorder" });
	BasicWidgetList.push_back({ "CanvasPanel", u8"画布面板", "ImCanvasPanel" });
	BasicWidgetList.push_back({ "HorizontalBox", u8"水平盒子", "ImHorizontalBox" });
	BasicWidgetList.push_back({ "HorizontalSplitter", u8"水平分割器", "ImHorizontalSplitter" });
	BasicWidgetList.push_back({ "VerticalBox", u8"垂直盒子", "ImVerticalBox" });
	BasicWidgetList.push_back({ "VerticalSplitter", u8"垂直分割器", "ImVerticalSplitter" });
	BasicWidgetList.push_back({ "ExpandableBox", u8"可展开盒子", "ImExpandableBox" });
	BasicWidgetList.push_back({ "ScrollBox", u8"滚动盒子", "ImScrollBox" });


	return BasicWidgetList;
}
