#include "ImWidgetFactory.h"

#include "ImBasicWidgetList.h"

/**
* @brief 初始化工厂，注册所有内置控件类型
*/

void ImGuiWidget::ImWidgetFactory::Initialize()
{
    // 注册所有内置控件类型
    RegisterType<ImWidget>("ImWidget");
    RegisterType<ImPanelWidget>("ImPanelWidget");
    RegisterType<ImCanvasPanel>("ImCanvasPanel");
    RegisterType<ImHorizontalBox>("ImHorizontalBox");
    RegisterType<ImVerticalBox>("ImVerticalBox");
    RegisterType<ImHorizontalSplitter>("ImHorizontalSplitter");
    RegisterType<ImVerticalSplitter>("ImVerticalSplitter");
    RegisterType<ImButton>("ImButton");
    RegisterType<ImCheckBox>("ImCheckBox");
    RegisterType<ImColorPicker>("ImColorPicker");
    RegisterType<ImComboBox>("ImComboBox");
    RegisterType<ImDesignPanel>("ImDesignPanel");
    RegisterType<ImExpandableBox>("ImExpandableBox");
    RegisterType<ImImage>("ImImage");
    RegisterType<ImInputText>("ImInputText");
    RegisterType<ImMultiLineTextBlock>("ImMultiLineTextBlock");
    RegisterType<ImResizableBox>("ImResizableBox");
    RegisterType<ImScrollBox>("ImScrollBox");
    RegisterType<ImScrollingTextList>("ImScrollingTextList");
    RegisterType<ImSlider>("ImSlider");
    RegisterType<ImTextBlock>("ImTextBlock");
}
