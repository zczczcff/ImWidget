#include "Public/WidgetInfor.h"
#include "ImWidget/ImWidgetFactory.h"

std::vector<WidgetInfor> BasicWidgetList::GetBasicWidgetList()
{
	std::vector<WidgetInfor> BasicWidgetList;

	// 添加所有基本控件到列表中
	BasicWidgetList.push_back({ "Button", u8"按钮", "ImButton", ImDesignerIcon::Button });
	BasicWidgetList.push_back({ "CheckBox", u8"勾选框", "ImCheckBox", ImDesignerIcon::CheckBox });
	BasicWidgetList.push_back({ "ComboBox", u8"组合框", "ImComboBox", ImDesignerIcon::ComboBox });
	BasicWidgetList.push_back({ "Image", u8"图片", "ImImage", ImDesignerIcon::Image });
	BasicWidgetList.push_back({ "InputText", u8"文本输入框", "ImInputText", ImDesignerIcon::InputText });
	BasicWidgetList.push_back({ "IntInput", u8"整数输入框", "ImIntInput", ImDesignerIcon::IntInput });
	BasicWidgetList.push_back({ "FloatInput", u8"浮点数输入框", "ImFloatInput", ImDesignerIcon::FloatInput });
	BasicWidgetList.push_back({ "MultiLineTextBlock", u8"多行文本块", "ImMultiLineTextBlock", ImDesignerIcon::MultiLineTextBlock });
	BasicWidgetList.push_back({ "ScrollingTextList", u8"滚动文本列表", "ImScrollingTextList", ImDesignerIcon::ScrollTextList });
	BasicWidgetList.push_back({ "Slider", u8"滑动条", "ImSlider", ImDesignerIcon::Slider });
	BasicWidgetList.push_back({ "TextBlock", u8"文本块", "ImTextBlock", ImDesignerIcon::TextBlock });
	BasicWidgetList.push_back({ "SimpleFigure", u8"简单图形", "ImSimpleFigure", ImDesignerIcon::SimpleFigure });
	//BasicWidgetList.push_back({ "Curve", u8"曲线图", "ImCurve" });

	// 添加布局和容器控件
	BasicWidgetList.push_back({ "Border", u8"边框", "ImBorder", ImDesignerIcon::Border });
	BasicWidgetList.push_back({ "CanvasPanel", u8"画布面板", "ImCanvasPanel", ImDesignerIcon::CanvasPanel });
	BasicWidgetList.push_back({ "HorizontalBox", u8"水平盒子", "ImHorizontalBox", ImDesignerIcon::HorizontalBox });
	BasicWidgetList.push_back({ "HorizontalSplitter", u8"水平分割器", "ImHorizontalSplitter", ImDesignerIcon::HorizontalSplitter });
	BasicWidgetList.push_back({ "VerticalBox", u8"垂直盒子", "ImVerticalBox", ImDesignerIcon::VerticalBox });
	BasicWidgetList.push_back({ "VerticalSplitter", u8"垂直分割器", "ImVerticalSplitter", ImDesignerIcon::VerticalSplitter });
	BasicWidgetList.push_back({ "ExpandableBox", u8"可展开盒子", "ImExpandableBox", ImDesignerIcon::ExpandableBox });
	BasicWidgetList.push_back({ "ScrollBox", u8"滚动盒子", "ImScrollBox", ImDesignerIcon::ScrollBox });


	return BasicWidgetList;
}
